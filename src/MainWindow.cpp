#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QMenu>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), m_engine(new TranslatorEngine(this))
{
    setupUi();
    
    connect(m_engine, &TranslatorEngine::logMessage, this, &MainWindow::onLog);
    connect(m_engine, &TranslatorEngine::progressUpdated, this, &MainWindow::onProgress);
    connect(m_engine, &TranslatorEngine::translationFinished, this, &MainWindow::onFinished);
    connect(m_engine, &TranslatorEngine::errorOccurred, this, &MainWindow::onError);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUi()
{
    // Apply Modern Stylesheet (Indigo & Slate Theme)
    QString styleSheet = R"(
        QWidget {
            font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;
            font-size: 14px;
            color: #374151; /* Gray 700 */
            background-color: #F3F4F6; /* Gray 100 */
        }
        
        /* Transparent Labels */
        QLabel {
            background-color: transparent;
        }
        
        /* Card-like Group Boxes */
        QGroupBox {
            background-color: #FFFFFF;
            border: 1px solid #E5E7EB; /* Gray 200 */
            border-radius: 12px;
            margin-top: 1em;
            padding: 24px 20px 20px 20px;
            font-weight: 600;
            font-size: 15px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 16px;
            padding: 0 8px;
            color: #4B5563; /* Gray 600 */
            background-color: transparent;
        }

        /* Inputs */
        QLineEdit, QComboBox {
            border: 1px solid #D1D5DB; /* Gray 300 */
            border-radius: 8px;
            padding: 8px 12px;
            background-color: #F9FAFB; /* Gray 50 */
            selection-background-color: #6366F1;
            min-height: 20px;
        }
        QLineEdit:focus, QComboBox:focus {
            border: 2px solid #6366F1; /* Indigo 500 */
            background-color: #FFFFFF;
            padding: 7px 11px; /* Adjust for 2px border */
        }
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 30px; /* Wider button */
            border-left-width: 1px;
            border-left-color: #D1D5DB;
            border-left-style: solid;
            border-top-right-radius: 8px;
            border-bottom-right-radius: 8px;
            background-color: #E5E7EB; /* Distinct background color */
        }
        QComboBox::down-arrow {
            /* You can provide an image here or style it implicitly */
            width: 10px;
            height: 10px;
            image: none; /* Reset if needed */
            border-left: 2px solid #6366F1; /* Indigo color to match theme */
            border-bottom: 2px solid #6366F1;
            transform: rotate(-45deg); /* Simple CSS arrow */
            margin-top: -3px; /* Adjust position */
        }
        /* Hover effect for drop-down */
        QComboBox::drop-down:hover {
            background-color: #D1D5DB;
        }

        /* Primary Buttons (Indigo) */
        QPushButton {
            background-color: #6366F1; /* Indigo 500 */
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 24px;
            font-weight: 600;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #4F46E5; /* Indigo 600 */
        }
        QPushButton:pressed {
            background-color: #4338CA; /* Indigo 700 */
            padding-top: 11px; /* Press effect */
        }
        QPushButton:disabled {
            background-color: #9CA3AF; /* Gray 400 */
        }

        /* Progress Bar */
        QProgressBar {
            border: none;
            background-color: #E5E7EB;
            border-radius: 6px;
            text-align: center;
            color: #111827;
            font-weight: bold;
            height: 18px;
        }
        QProgressBar::chunk {
            background-color: #6366F1;
            border-radius: 6px;
        }

        /* Log Console (Dark Mode) */
        QTextEdit {
            background-color: #1F2937; /* Gray 800 */
            color: #E5E7EB; /* Gray 200 */
            border: 1px solid #374151;
            border-radius: 10px;
            font-family: 'Consolas', 'Monaco', monospace;
            font-size: 13px;
            padding: 12px;
            selection-background-color: #4B5563;
        }
    )";
    this->setStyleSheet(styleSheet);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    
    // --- File Selection Group ---
    QGroupBox *fileGroup = new QGroupBox(QString::fromUtf8("\xE6\x96\x87\xE4\xBB\xB6\xE9\x80\x89\xE6\x8B\xA9")); // "File Selection"
    QHBoxLayout *fileLayout = new QHBoxLayout(fileGroup);
    fileLayout->setSpacing(12);
    
    m_pathEdit = new QLineEdit();
    m_pathEdit->setPlaceholderText(QString::fromUtf8(".ts \xE6\x96\x87\xE4\xBB\xB6\xE8\xB7\xAF\xE5\xBE\x84")); // ".ts file path"
    m_browseBtn = new QPushButton("Browse");
    m_browseBtn->setCursor(Qt::PointingHandCursor);
    // Make browse button slightly different (secondary style)
    m_browseBtn->setStyleSheet("background-color: #4B5563;"); // Gray 600
    
    fileLayout->addWidget(new QLabel("TS File:"));
    fileLayout->addWidget(m_pathEdit);
    fileLayout->addWidget(m_browseBtn);
    
    mainLayout->addWidget(fileGroup);
    
    // --- Settings Group ---
    QGroupBox *settingsGroup = new QGroupBox(QString::fromUtf8("\xE5\x8F\x82\xE6\x95\xB0\xE9\x85\x8D\xE7\xBD\xAE")); // "Parameter Config"
    QGridLayout *settingsLayout = new QGridLayout(settingsGroup);
    settingsLayout->setVerticalSpacing(16);
    settingsLayout->setHorizontalSpacing(20);
    
    m_langCombo = new QComboBox();
    m_langCombo->addItems({"Chinese", "English", "Japanese", "French", "German", "Spanish", "Korean", "Russian", "Vietnamese", "Malay", "Thai"});
    m_langCombo->setEditable(true);
    
    m_apiEdit = new QLineEdit("http://localhost:11434/api/generate");
    m_modelEdit = new QLineEdit("qwen3:14b");
    
    // Add Labels with better styling if needed, but default is fine
    settingsLayout->addWidget(new QLabel(QString::fromUtf8("\xE7\x9B\xAE\xE6\xA0\x87\xE8\xAF\xAD\xE8\xA8\x80:")), 0, 0); // Target Lang
    settingsLayout->addWidget(m_langCombo, 0, 1);
    settingsLayout->addWidget(new QLabel("Ollama API URL:"), 1, 0);
    settingsLayout->addWidget(m_apiEdit, 1, 1);
    settingsLayout->addWidget(new QLabel(QString::fromUtf8("\xE6\xA8\xA1\xE5\x9E\x8B\xE5\x90\x8D\xE7\xA7\xB0:")), 2, 0); // Model Name
    settingsLayout->addWidget(m_modelEdit, 2, 1);
    
    // Add Retranslate All Checkbox
    m_retranslateCheck = new QCheckBox(QString::fromUtf8("\xE9\x87\x8D\xE6\x96\xB0\xE7\xBF\xBB\xE8\xAF\x91\xE6\x89\x80\xE6\x9C\x89\xE6\x9D\xA1\xE7\x9B\xAE (Retranslate All)"));
    // Add some styling or spacing if needed
    settingsLayout->addWidget(m_retranslateCheck, 3, 1);
    
    mainLayout->addWidget(settingsGroup);
    
    // --- Controls ---
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(20);
    btnLayout->addStretch(); 
    
    m_startBtn = new QPushButton(QString::fromUtf8("\xE5\xBC\x80\xE5\xA7\x8B\xE7\xBF\xBB\xE8\xAF\x91")); // "Start Translation"
    m_startBtn->setCursor(Qt::PointingHandCursor);
    m_startBtn->setMinimumWidth(160);
    m_startBtn->setMinimumHeight(45); // Taller button
    
    m_saveBtn = new QPushButton(QString::fromUtf8("\xE4\xBF\x9D\xE5\xAD\x98\xE6\x96\x87\xE4\xBB\xB6")); // "Save File"
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    m_saveBtn->setEnabled(false);
    m_saveBtn->setMinimumWidth(120);
    m_saveBtn->setMinimumHeight(45);
    
    // Success Color (Emerald) for Save Button
    m_saveBtn->setStyleSheet(R"(
        QPushButton { background-color: #10B981; } 
        QPushButton:hover { background-color: #059669; } 
        QPushButton:pressed { background-color: #047857; }
        QPushButton:disabled { background-color: #A7F3D0; }
    )");

    btnLayout->addWidget(m_startBtn);
    btnLayout->addWidget(m_saveBtn);
    btnLayout->addStretch();
    
    mainLayout->addLayout(btnLayout);
    
    // --- Progress ---
    m_progressBar = new QProgressBar();
    m_progressBar->setTextVisible(true);
    m_progressBar->setFixedHeight(20);
    mainLayout->addWidget(m_progressBar);
    
    // --- Log ---
    m_logEdit = new QTextEdit();
    m_logEdit->setReadOnly(true);
    m_logEdit->setPlaceholderText(QString::fromUtf8("\xE6\x97\xA5\xE5\xBF\x97\xE5\xB0\x86\xE6\x98\xBE\xE7\xA4\xBA\xE5\x9C\xA8\xE8\xBF\x99\xE9\x87\x8C...")); // "Logs will appear here..."
    m_logEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_logEdit, &QTextEdit::customContextMenuRequested, this, [this](const QPoint &pos) {
        QMenu *menu = new QMenu(this);
        
        QAction *copyAct = menu->addAction("Copy");
        copyAct->setEnabled(m_logEdit->textCursor().hasSelection());
        connect(copyAct, &QAction::triggered, m_logEdit, &QTextEdit::copy);
        
        menu->addAction("Select All", m_logEdit, &QTextEdit::selectAll);
        
        menu->addSeparator();
        
        menu->addAction(QString::fromUtf8("\xE6\xB8\x85\xE7\xA9\xBA\xE6\x97\xA5\xE5\xBF\x97"), m_logEdit, &QTextEdit::clear); // "Clear Log"
        
        menu->exec(m_logEdit->mapToGlobal(pos));
        delete menu;
    });
    mainLayout->addWidget(m_logEdit);
    
    connect(m_browseBtn, &QPushButton::clicked, this, &MainWindow::onBrowse);
    connect(m_startBtn, &QPushButton::clicked, this, &MainWindow::onStart);
    connect(m_saveBtn, &QPushButton::clicked, this, &MainWindow::onSave);
    
    resize(750, 650);
    setWindowTitle("LLM Translator (Ollama)");
    setWindowIcon(QIcon(":/icons/app_icon.ico")); // Or load from file if not in resource system yet
    // Since we didn't set up qrc, let's try to load from local file for dev, 
    // but the .rc file handles the exe icon.
    // To show icon in title bar during dev without qrc:
    setWindowIcon(QIcon("app_icon.ico")); 
}

void MainWindow::onBrowse()
{
    QString path = QFileDialog::getOpenFileName(this, "Open TS File", "", "Qt Translation Files (*.ts);;All Files (*)");
    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
    }
}

void MainWindow::onStart()
{
    QString path = m_pathEdit->text();
    if (path.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a TS file first.");
        return;
    }
    
    m_startBtn->setEnabled(false);
    m_saveBtn->setEnabled(false);
    m_logEdit->clear();
    
    if (m_engine->loadFile(path)) {
        m_progressBar->setMaximum(m_engine->getUnfinishedCount());
        m_progressBar->setValue(0);
        
        QString targetLang = m_langCombo->currentText();
        QString apiUrl = m_apiEdit->text();
        QString modelName = m_modelEdit->text();
        bool retranslateAll = m_retranslateCheck->isChecked();
        
        m_engine->startTranslation(targetLang, apiUrl, modelName, retranslateAll);
    } else {
        m_startBtn->setEnabled(true);
    }
}

void MainWindow::onSave()
{
    QString path = m_pathEdit->text();
    if (path.isEmpty()) return;
    
    // Maybe prompt for new path? For now overwrite or save as new
    // Let's ask user where to save
    QString savePath = QFileDialog::getSaveFileName(this, "Save Translated File", path, "Qt Translation Files (*.ts)");
    if (!savePath.isEmpty()) {
        m_engine->saveFile(savePath);
    }
}

void MainWindow::onLog(const QString &msg)
{
    m_logEdit->append(msg);
}

void MainWindow::onProgress(int current, int total)
{
    m_progressBar->setMaximum(total);
    m_progressBar->setValue(current);
}

void MainWindow::onFinished()
{
    m_startBtn->setEnabled(true);
    m_saveBtn->setEnabled(true);
    QMessageBox::information(this, "Done", "Translation process finished.");
}

void MainWindow::onError(const QString &err)
{
    m_logEdit->append("ERROR: " + err);
    QMessageBox::critical(this, "Error", err);
    m_startBtn->setEnabled(true);
}
