#include "TranslatorEngine.h"
#include <QDebug>
#include <QTextStream>

TranslatorEngine::TranslatorEngine(QObject *parent)
    : QObject(parent), m_currentIndex(0), m_isRunning(false), m_networkManager(new QNetworkAccessManager(this))
{
    // Note: We handle replies individually using lambda or direct connection in sendRequest if needed,
    // but here we might connect globally if we track the active reply.
    // For simplicity, we'll use lambda in sendRequest.
}

bool TranslatorEngine::loadFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred("Failed to open file: " + filePath);
        return false;
    }

    QString errorMsg;
    int errorLine, errorColumn;
    if (!m_doc.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
        emit errorOccurred(QString("XML Parse error: %1 at line %2 col %3").arg(errorMsg).arg(errorLine).arg(errorColumn));
        file.close();
        return false;
    }
    file.close();

    m_itemsToTranslate.clear();
    
    QDomElement root = m_doc.documentElement(); // TS
    QDomNode contextNode = root.firstChild();
    
    while (!contextNode.isNull()) {
        if (contextNode.nodeName() == "context") {
            QDomElement contextElem = contextNode.toElement();
            QString contextName;
            
            QDomNode n = contextElem.firstChild();
            while (!n.isNull()) {
                if (n.nodeName() == "name") {
                    contextName = n.toElement().text();
                } else if (n.nodeName() == "message") {
                    QDomElement messageElem = n.toElement();
                    QDomElement sourceElem = messageElem.firstChildElement("source");
                    QDomElement translationElem = messageElem.firstChildElement("translation");
                    
                    if (!sourceElem.isNull() && !translationElem.isNull()) {
                        QString type = translationElem.attribute("type");
                        QString text = translationElem.text();
                        
                        if (type == "unfinished" || text.isEmpty()) {
                            TranslationItem item;
                            item.context = contextName;
                            item.source = sourceElem.text();
                            item.element = translationElem;
                            m_itemsToTranslate.append(item);
                        }
                    }
                }
                n = n.nextSibling();
            }
        }
        contextNode = contextNode.nextSibling();
    }
    
    emit logMessage(QString("Loaded file. Found %1 items to translate.").arg(m_itemsToTranslate.size()));
    return true;
}

bool TranslatorEngine::saveFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred("Failed to save file: " + filePath);
        return false;
    }
    
    QTextStream out(&file);
    out.setCodec("UTF-8");
    m_doc.save(out, 4);
    file.close();
    emit logMessage("File saved successfully.");
    return true;
}

int TranslatorEngine::getUnfinishedCount() const
{
    return m_itemsToTranslate.size();
}

void TranslatorEngine::startTranslation(const QString &targetLang, const QString &apiUrl, const QString &modelName)
{
    if (m_itemsToTranslate.isEmpty()) {
        emit logMessage("Nothing to translate.");
        emit translationFinished();
        return;
    }
    
    m_targetLang = targetLang;
    m_apiUrl = apiUrl;
    m_modelName = modelName;
    m_currentIndex = 0;
    m_isRunning = true;
    
    // 默认使用分批处理，每批 50 条
    // 这样可以避免一次性请求过大导致模型上下文溢出或响应截断
    int batchSize = 50;
    emit logMessage(QString("Starting translation: %1 items total, processing in batches of %2...").arg(m_itemsToTranslate.size()).arg(batchSize));
    emit progressUpdated(0, m_itemsToTranslate.size());
    
    processBatch(0, batchSize);
}

void TranslatorEngine::stopTranslation()
{
    m_isRunning = false;
    emit logMessage("Translation stopped by user.");
}

// 分批处理方法（当一次性请求失败时自动回退使用）
void TranslatorEngine::processBatch(int startIdx, int batchSize)
{
    if (!m_isRunning) return;
    
    if (startIdx >= m_itemsToTranslate.size()) {
        emit logMessage("All items processed.");
        emit translationFinished();
        m_isRunning = false;
        return;
    }
    
    int endIndex = qMin(startIdx + batchSize, m_itemsToTranslate.size());
    QJsonArray batchArray;
    
    for (int i = startIdx; i < endIndex; ++i) {
        QJsonObject itemObj;
        itemObj["id"] = i;
        itemObj["text"] = m_itemsToTranslate[i].source;
        batchArray.append(itemObj);
    }
    
    emit progressUpdated(startIdx, m_itemsToTranslate.size());
    emit logMessage(QString("Processing batch: items %1-%2 of %3...").arg(startIdx + 1).arg(endIndex).arg(m_itemsToTranslate.size()));
    
    sendBatchRequest(batchArray, startIdx, endIndex - startIdx);
}

void TranslatorEngine::sendBatchRequest(const QJsonArray &batchArray, int startIdx, int count)
{
    QNetworkRequest request;
    request.setUrl(QUrl(m_apiUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject json;
    json["model"] = m_modelName;
    json["stream"] = false;
    
    // 使用 JSON schema 来强制返回正确的格式
    QJsonObject formatSchema;
    formatSchema["type"] = "object";
    QJsonObject properties;
    QJsonObject translationsProp;
    translationsProp["type"] = "array";
    QJsonObject items;
    items["type"] = "object";
    QJsonObject itemProperties;
    QJsonObject idProp;
    idProp["type"] = "integer";
    itemProperties["id"] = idProp;
    QJsonObject translationProp;
    translationProp["type"] = "string";
    itemProperties["translation"] = translationProp;
    items["properties"] = itemProperties;
    items["required"] = QJsonArray::fromStringList({"id", "translation"});
    translationsProp["items"] = items;
    properties["translations"] = translationsProp;
    formatSchema["properties"] = properties;
    formatSchema["required"] = QJsonArray::fromStringList({"translations"});
    
    json["format"] = formatSchema; // 使用 JSON schema 而不是简单的 "json"
    
    QJsonDocument batchDoc(batchArray);
    QString jsonString = batchDoc.toJson(QJsonDocument::Compact);
    
    // Prompt 强调 JSON 格式
    QString promptText = QString(
        "Translate ALL %2 items in this JSON array to %1.\n\n"
        "You MUST return a valid JSON object with this exact structure:\n"
        "{\"translations\": [{\"id\": 1, \"translation\": \"text1\"}, {\"id\": 2, \"translation\": \"text2\"}, ...]}\n\n"
        "Input: %3\n\n"
        "Return ONLY the JSON object:"
    ).arg(m_targetLang).arg(count).arg(jsonString);
    
    // Ollama API 使用 "prompt" 参数（根据官方文档）
    json["prompt"] = promptText;
    
    QByteArray data = QJsonDocument(json).toJson();
    
    int requestSizeKB = data.size() / 1024;
    emit logMessage(QString("Sending batch of %1 items (Request size: ~%2 KB)...").arg(count).arg(requestSizeKB));
    
    // 如果请求过大（超过 100KB），给出警告
    if (requestSizeKB > 100) {
        emit logMessage(QString("Warning: Request size is large (%1 KB). This might exceed model context limits.").arg(requestSizeKB));
    }
    
    QNetworkReply *reply = m_networkManager->post(request, data);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, startIdx, count]() {
        reply->deleteLater();
        if (!m_isRunning) return;
        
        if (reply->error() != QNetworkReply::NoError) {
            emit logMessage("Network Error: " + reply->errorString());
            emit errorOccurred("Network Error: " + reply->errorString());
            m_isRunning = false;
            emit translationFinished();
            return;
        }
        
        QByteArray responseData = reply->readAll();
        QString rawResponse = QString::fromUtf8(responseData);
        emit logMessage("Raw API Response: " + rawResponse.left(500) + (rawResponse.length() > 500 ? "..." : ""));
        
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObj = jsonDoc.object();
        
        // 检查是否有错误信息
        if (jsonObj.contains("error")) {
            QString errorMsg = jsonObj["error"].toString();
            emit logMessage("API Error: " + errorMsg);
            emit errorOccurred("API Error: " + errorMsg);
            m_isRunning = false;
            emit translationFinished();
            return;
        }
        
        // 检查响应状态码（自定义 API 格式）
        if (jsonObj.contains("code")) {
            int code = jsonObj["code"].toInt();
            if (code != 200) {
                QString msg = jsonObj.value("msg").toString();
                emit logMessage(QString("API returned error code %1: %2").arg(code).arg(msg));
                emit errorOccurred(QString("API Error: %1").arg(msg));
                m_isRunning = false;
                emit translationFinished();
                return;
            }
        }
        
        QString responseContent;
        QJsonValue responseValue;
        bool hasResponse = false;
        bool isDirectData = false;
        
        // 支持多种响应格式
        if (jsonObj.contains("response")) {
            // Ollama 标准格式
            QJsonValue respValue = jsonObj["response"];
            if (respValue.isString()) {
                QString respStr = respValue.toString().trimmed();
                // 如果 response 为空，检查 thinking 字段（thinking models）
                if (respStr.isEmpty() && jsonObj.contains("thinking")) {
                    QJsonValue thinkingValue = jsonObj["thinking"];
                    if (thinkingValue.isString()) {
                        responseContent = thinkingValue.toString().trimmed();
                        emit logMessage("Using 'thinking' field as response (thinking model detected)");
                        hasResponse = true;
                    }
                } else if (!respStr.isEmpty()) {
                    responseContent = respStr;
                    // qDebug() << responseContent; // Avoid printing huge content
                    hasResponse = true;
                }
            } else {
                // response 可能是对象或数组
                responseValue = respValue;
                isDirectData = true;
                hasResponse = true;
            }
        }
        
        // 如果 response 为空，尝试从 thinking 字段获取（thinking models）
        if (!hasResponse && jsonObj.contains("thinking")) {
            QJsonValue thinkingValue = jsonObj["thinking"];
            if (thinkingValue.isString()) {
                responseContent = thinkingValue.toString().trimmed();
                emit logMessage("Using 'thinking' field as response (thinking model detected)");
                hasResponse = true;
            }
        }
        
        if (!hasResponse && jsonObj.contains("data")) {
            // 自定义格式：data 可能是字符串、对象或数组
            QJsonValue dataValue = jsonObj["data"];
            if (dataValue.isString()) {
                responseContent = dataValue.toString().trimmed();
                hasResponse = true;
            } else {
                // data 是对象或数组，直接使用
                responseValue = dataValue;
                isDirectData = true;
                hasResponse = true;
            }
        }
        
        if (!hasResponse) {
            emit logMessage("Invalid response format from API.");
            emit logMessage("Response keys: " + jsonObj.keys().join(", "));
            
            // 如果是分批处理模式，跳过当前批次继续处理
            if (count < m_itemsToTranslate.size()) {
                emit logMessage(QString("Skipping batch %1-%2 due to invalid response, continuing...").arg(startIdx + 1).arg(startIdx + count));
                int nextIdx = startIdx + count;
                if (nextIdx < m_itemsToTranslate.size()) {
                    processBatch(nextIdx, 50);
                    return;
                }
            }
            
            emit errorOccurred("Invalid response format from API. Missing 'response' or 'data' field.");
            m_isRunning = false;
            emit translationFinished();
            return;
        }
        
        QJsonDocument responseJsonDoc;
        QJsonArray resultArray;
        
        if (isDirectData) {
            // 如果响应是直接的对象或数组，直接使用
            if (responseValue.isArray()) {
                resultArray = responseValue.toArray();
            } else if (responseValue.isObject()) {
                QJsonObject rootObj = responseValue.toObject();
                if (rootObj.contains("translations") && rootObj["translations"].isArray()) {
                    resultArray = rootObj["translations"].toArray();
                }
            }
        } else {
            // 需要解析字符串内容
            emit logMessage("Response content length: " + QString::number(responseContent.length()));
            
            // 由于使用了 format: "json" 参数，Ollama 应该返回纯 JSON 格式
            // 但为了兼容性和健壮性，仍保留清理步骤以处理可能的边缘情况
            // Clean up markdown if present (shouldn't happen with format: "json", but keep for safety)
            if (responseContent.startsWith("```json")) {
                responseContent = responseContent.mid(7);
            } else if (responseContent.startsWith("```")) { // Handle cases where language isn't specified
                 responseContent = responseContent.mid(3);
            }
            if (responseContent.endsWith("```")) {
                responseContent.chop(3);
            }
            
            // Try to find the first '{' and last '}' to handle extra text (shouldn't be needed with format: "json")
            int firstBrace = responseContent.indexOf('{');
            int lastBrace = responseContent.lastIndexOf('}');
            
            if (firstBrace != -1 && lastBrace != -1 && lastBrace > firstBrace) {
                responseContent = responseContent.mid(firstBrace, lastBrace - firstBrace + 1);
            } else if (firstBrace == -1) {
                // 如果找不到外层大括号，尝试查找是否是纯数组（有些模型可能直接返回数组）
                int firstBracket = responseContent.indexOf('[');
                int lastBracket = responseContent.lastIndexOf(']');
                if (firstBracket != -1 && lastBracket != -1 && lastBracket > firstBracket) {
                     responseContent = responseContent.mid(firstBracket, lastBracket - firstBracket + 1);
                }
            }

            responseJsonDoc = QJsonDocument::fromJson(responseContent.toUtf8());
            
            if (responseJsonDoc.isObject()) {
                 QJsonObject rootObj = responseJsonDoc.object();
                 
                 // 检查是否有 translations 字段
                 if (rootObj.contains("translations") && rootObj["translations"].isArray()) {
                     resultArray = rootObj["translations"].toArray();
                 }
                 // 检查是否有 data 字段（自定义格式）
                 else if (rootObj.contains("data")) {
                     QJsonValue dataValue = rootObj["data"];
                     if (dataValue.isArray()) {
                         resultArray = dataValue.toArray();
                     } else if (dataValue.isObject()) {
                         // data 是对象，可能包含 translations
                         QJsonObject dataObj = dataValue.toObject();
                         if (dataObj.contains("translations") && dataObj["translations"].isArray()) {
                             resultArray = dataObj["translations"].toArray();
                         }
                     }
                 }
            } else if (responseJsonDoc.isArray()) {
                resultArray = responseJsonDoc.array();
            }
        }

            if (!resultArray.isEmpty()) {
                emit logMessage(QString("Received %1 items in response.").arg(resultArray.size()));
                
                // 检查第一个元素的格式
                if (!resultArray.isEmpty()) {
                    QJsonObject firstObj = resultArray[0].toObject();
                    QStringList keys = firstObj.keys();
                    if (!keys.contains("translation")) {
                        emit logMessage("Warning: Response items don't have 'translation' field. Keys: " + keys.join(", "));
                        emit logMessage("Expected format: Objects with 'id' and 'translation' fields.");
                    }
                }
                
                int successCount = 0;
                for (const QJsonValue &val : resultArray) {
                    if (!val.isObject()) continue;
                    
                    QJsonObject obj = val.toObject();
                    int id = obj["id"].toInt();
                    QString translation = obj["translation"].toString();
                    
                    if (id >= 0 && id < m_itemsToTranslate.size() && !translation.isEmpty()) {
                        TranslationItem &item = m_itemsToTranslate[id];
                        
                        // Update DOM
                        // Remove all existing children (text, comments, etc.) to ensure clean replacement
                        while (!item.element.firstChild().isNull()) {
                            item.element.removeChild(item.element.firstChild());
                        }
                        QDomText textNode = m_doc.createTextNode(translation);
                        item.element.appendChild(textNode);
                        
                        if (item.element.hasAttribute("type")) {
                            item.element.removeAttribute("type");
                        }
                        
                        successCount++;
                    }
                }
                
                if (successCount == 0 && !resultArray.isEmpty()) {
                    emit logMessage("Warning: No valid translations found in response. Check if the response format matches expected format.");
                }
                
                emit logMessage(QString("Successfully translated %1 items.").arg(successCount));
                
                // 如果是分批处理模式，继续处理下一批
                if (count < m_itemsToTranslate.size()) {
                    int nextIdx = startIdx + count;
                    if (nextIdx < m_itemsToTranslate.size()) {
                        processBatch(nextIdx, 50); // 继续处理下一批
                        return;
                    }
                }
                
                emit progressUpdated(startIdx + count, m_itemsToTranslate.size());
                
                // 如果这是最后一批，标记为完成
                if (startIdx + count >= m_itemsToTranslate.size()) {
                    m_isRunning = false;
                    emit translationFinished();
                }
            } else {
                emit logMessage("Error: API response is not a valid JSON array or doesn't contain translations.");
                
                // 检查返回的数据格式
                if (isDirectData && responseValue.isArray()) {
                    QJsonArray dataArray = responseValue.toArray();
                    if (!dataArray.isEmpty()) {
                        QJsonObject firstItem = dataArray[0].toObject();
                        QStringList keys = firstItem.keys();
                        emit logMessage("Returned data format: Array with keys: " + keys.join(", "));
                        emit logMessage("Expected format: Array of objects with 'id' and 'translation' fields.");
                        
                        if (!keys.contains("translation")) {
                            emit errorOccurred("API returned data in wrong format. Expected translation results with 'id' and 'translation' fields, but got: " + keys.join(", "));
                        }
                    }
                } else if (!isDirectData) {
                    emit logMessage("Response content (first 1000 chars): " + responseContent.left(1000));
                    if (responseJsonDoc.isObject()) {
                        QJsonObject parsedObj = responseJsonDoc.object();
                        QStringList keys = parsedObj.keys();
                        emit logMessage("Parsed JSON object keys: " + keys.join(", "));
                        
                        // 检查是否有 data 字段
                        if (parsedObj.contains("data")) {
                            QJsonValue dataValue = parsedObj["data"];
                            emit logMessage("Found 'data' field in response.");
                            if (dataValue.isObject()) {
                                QJsonObject dataObj = dataValue.toObject();
                                emit logMessage("Data object keys: " + dataObj.keys().join(", "));
                                emit logMessage("Warning: Model returned data in wrong format. Expected: {\"translations\": [...]}, but got data object with different structure.");
                            } else if (dataValue.isArray()) {
                                QJsonArray dataArray = dataValue.toArray();
                                emit logMessage(QString("Data is an array with %1 items.").arg(dataArray.size()));
                                if (!dataArray.isEmpty()) {
                                    QJsonObject firstItem = dataArray[0].toObject();
                                    emit logMessage("First data item keys: " + firstItem.keys().join(", "));
                                }
                            }
                        }
                    }
                }
                
                // 如果响应是空对象，可能是请求太大，自动回退到分批处理
                if ((!isDirectData && (responseContent == "{}" || responseContent.isEmpty())) && count == m_itemsToTranslate.size()) {
                    emit logMessage("Request too large, falling back to batch processing...");
                    m_currentIndex = 0;
                    processBatch(0, 50); // 使用 50 个一批
                    return;
                } else {
                    // 如果是分批处理模式，跳过当前批次继续处理
                    if (count < m_itemsToTranslate.size()) {
                        emit logMessage(QString("Skipping batch %1-%2 due to error, continuing...").arg(startIdx + 1).arg(startIdx + count));
                        int nextIdx = startIdx + count;
                        if (nextIdx < m_itemsToTranslate.size()) {
                            processBatch(nextIdx, 50);
                            return;
                        }
                    }
                    
                    if (!isDirectData && (responseContent == "{}" || responseContent.isEmpty())) {
                        emit errorOccurred("API returned empty response. The request might be too large.");
                    } else {
                        emit errorOccurred("API response is not a valid translation result format.");
                    }
                }
            }
        
        // 只有在不是分批处理模式或所有批次都完成时才结束
        // 注意：如果是分批处理，processBatch 会继续调用 sendBatchRequest，所以这里不需要结束
        // 修改：因为上面已经在成功处理后判断了是否完成，这里只处理非分批（一次性）的情况或者异常退出的情况
        // 但由于我们现在强制分批，这里的逻辑其实在上面已经处理了。
        // 为了安全起见，我们保留这个检查，但确保不会重复触发
        if (m_isRunning && count >= m_itemsToTranslate.size()) {
            m_isRunning = false;
            emit translationFinished();
        }
    });
}
