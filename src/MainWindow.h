#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include "TranslatorEngine.h"

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBrowse();
    void onStart();
    void onSave();
    void onLog(const QString &msg);
    void onProgress(int current, int total);
    void onFinished();
    void onError(const QString &err);

private:
    void setupUi();

    QLineEdit *m_pathEdit;
    QPushButton *m_browseBtn;
    
    QComboBox *m_langCombo;
    QLineEdit *m_apiEdit;
    QLineEdit *m_modelEdit;
    QCheckBox *m_retranslateCheck; // Checkbox for retranslating all items
    
    QTextEdit *m_logEdit;
    QProgressBar *m_progressBar;
    
    QPushButton *m_startBtn;
    QPushButton *m_saveBtn;
    
    TranslatorEngine *m_engine;
};

#endif // MAINWINDOW_H
