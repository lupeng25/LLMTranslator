#ifndef TRANSLATORENGINE_H
#define TRANSLATORENGINE_H

#include <QObject>
#include <QDomDocument>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

struct TranslationItem {
    QString context;
    QString source;
    QString translation;
    QDomElement element; // Reference to the XML element to update
};

class TranslatorEngine : public QObject {
    Q_OBJECT

public:
    explicit TranslatorEngine(QObject *parent = nullptr);
    
    bool loadFile(const QString &filePath);
    bool saveFile(const QString &filePath);
    
    // Returns total unfinished items count
    int getUnfinishedCount() const;
    
    // Start translation process
    void startTranslation(const QString &targetLang, const QString &apiUrl, const QString &modelName);
    void stopTranslation();

signals:
    void progressUpdated(int current, int total);
    void logMessage(const QString &msg);
    void translationFinished();
    void errorOccurred(const QString &err);

private:
    void sendBatchRequest(const QJsonArray &batchArray, int startIdx, int count);
    void processBatch(int startIdx, int batchSize);

    QDomDocument m_doc;
    QList<TranslationItem> m_itemsToTranslate;
    int m_currentIndex;
    bool m_isRunning;
    
    QString m_targetLang;
    QString m_apiUrl;
    QString m_modelName;
    
    QNetworkAccessManager *m_networkManager;
};

#endif // TRANSLATORENGINE_H
