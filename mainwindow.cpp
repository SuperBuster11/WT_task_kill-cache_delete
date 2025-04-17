#include "mainwindow.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <QTextStream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    loadTranslations();
    setLanguage(QLocale::system().name().startsWith("pl") ? "pl" : "en");
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    languageSelector = new QComboBox();
    languageSelector->addItem("Polski", "pl");
    languageSelector->addItem("English", "en");
    connect(languageSelector, &QComboBox::currentTextChanged, this, [=](const QString&) {
        setLanguage(languageSelector->currentData().toString());
        });

    layout->addWidget(languageSelector);

    closeButton = new QPushButton();
    deleteButton = new QPushButton();
    clearButton = new QPushButton();
    connect(closeButton, &QPushButton::clicked, this, &MainWindow::closeProcesses);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteCache);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearLog);

    layout->addWidget(closeButton);
    layout->addWidget(deleteButton);

    infoLabel = new QLabel();
    layout->addWidget(infoLabel);

    logText = new QTextEdit();
    logText->setReadOnly(true);
    layout->addWidget(logText);

    layout->addWidget(clearButton);
    central->setLayout(layout);
    setCentralWidget(central);
}

void MainWindow::loadTranslations() {
    translations["en"] = {
        {"close_processes_option", "Close War Thunder processes"},
        {"delete_cache_option", "Delete cache folder"},
        {"select_folder", "Select the main War Thunder folder"},
        {"closing_processes", "Closing War Thunder processes..."},
        {"processes_closed", "Closed %1 War Thunder processes."},
        {"no_processes_found", "No War Thunder processes found to close."},
        {"confirm_delete_cache", "Are you sure you want to delete the 'cache' folder?"},
        {"cache_deleted", "The 'cache' folder has been deleted."},
        {"cache_not_exists", "The 'cache' folder does not exist."},
        {"delete_cache_error", "An error occurred while deleting the 'cache' folder."},
        {"delete_cache_cancelled", "Deletion of the 'cache' folder has been cancelled."},
        {"clear_log", "Clear log"},
        {"no_folder_selected", "No folder was selected."}
    };

    translations["pl"] = {
        {"close_processes_option", "Zamknij procesy War Thunder"},
        {"delete_cache_option", "Usuń folder cache"},
        {"select_folder", "Wybierz główny folder War Thunder"},
        {"closing_processes", "Zamykanie procesów War Thunder..."},
        {"processes_closed", "Zamknięto %1 procesów War Thunder."},
        {"no_processes_found", "Nie znaleziono procesów War Thunder do zamknięcia."},
        {"confirm_delete_cache", "Czy na pewno chcesz usunąć folder 'cache'?"},
        {"cache_deleted", "Folder 'cache' został usunięty."},
        {"cache_not_exists", "Folder 'cache' nie istnieje."},
        {"delete_cache_error", "Błąd przy usuwaniu folderu 'cache'."},
        {"delete_cache_cancelled", "Usunięcie folderu 'cache' zostało anulowane."},
        {"clear_log", "Wyczyść log"},
        {"no_folder_selected", "Nie wybrano folderu."}
    };
}

void MainWindow::setLanguage(const QString& lang) {
    currentLanguage = lang;
    closeButton->setText(tr("close_processes_option"));
    deleteButton->setText(tr("delete_cache_option"));
    clearButton->setText(tr("clear_log"));
}

QString MainWindow::tr(const QString& key, const QVariantMap& args) {
    QString text = translations.value(currentLanguage).value(key, key);
    for (auto it = args.begin(); it != args.end(); ++it) {
        text.replace(QString("%%1").arg(it.key()), it.value().toString());
    }
    return text;
}

void MainWindow::log(const QString& text) {
    logText->append(text);
}

int MainWindow::killProcessByName(const QString& name) {
    int count = 0;
    QProcess p;
    p.start("tasklist", QStringList() << "/FI" << QString("IMAGENAME eq %1").arg(name));
    p.waitForFinished();
    QString output = p.readAllStandardOutput();

    if (output.contains(name, Qt::CaseInsensitive)) {
        QProcess::execute("taskkill", QStringList() << "/F" << "/IM" << name);
        log(QString("✔ Terminated %1").arg(name));
        count++;
    }
    return count;
}

void MainWindow::closeProcesses() {
    infoLabel->setText(tr("closing_processes"));
    log(tr("closing_processes"));

    QStringList processes = {
        "launcher.exe", "aces.exe", "aces-min-cpu.exe", "cefprocess.exe", "gjagent.exe",
        "aces_be.exe", "beac_wt_mlauncher.exe", "beservice.exe", "beservice_x64.exe",
        "eac_wt_mlauncher.exe", "easyanticheat.exe", "easyanticheat_setup.exe", "eac_launcher.exe"
    };

    int total = 0;
    for (const QString& name : processes) {
        total += killProcessByName(name);
    }

    QString msg = (total > 0) ? tr("processes_closed", { {"1", QString::number(total)} }) : tr("no_processes_found");
    infoLabel->setText(msg);
    log(msg);
}

void MainWindow::deleteCache() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("select_folder"));
    if (dir.isEmpty()) {
        log(tr("no_folder_selected"));
        return;
    }

    QDir cacheDir(dir + "/cache");
    if (!cacheDir.exists()) {
        infoLabel->setText(tr("cache_not_exists"));
        log(tr("cache_not_exists"));
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("confirm_delete_cache"), tr("confirm_delete_cache"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (cacheDir.removeRecursively()) {
            log(QString("✔ %1 (%2)").arg(tr("cache_deleted")).arg(cacheDir.absolutePath()));
            infoLabel->setText(tr("cache_deleted"));
        }
        else {
            log(tr("delete_cache_error"));
            infoLabel->setText(tr("delete_cache_error"));
        }
    }
    else {
        log(tr("delete_cache_cancelled"));
    }
}

void MainWindow::clearLog() {
    logText->clear();
}
