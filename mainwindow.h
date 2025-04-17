#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QComboBox>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void setLanguage(const QString& lang);
    void closeProcesses();
    void deleteCache();
    void clearLog();

private:
    QString currentLanguage;
    QMap<QString, QMap<QString, QString>> translations;
    QTextEdit* logText;
    QLabel* infoLabel;
    QPushButton* closeButton, * deleteButton, * clearButton;
    QComboBox* languageSelector;

    QString tr(const QString& key, const QVariantMap& args = {});
    void log(const QString& text);
    int killProcessByName(const QString& name);
    void setupUi();
    void loadTranslations();
};

#endif // MAINWINDOW_H
