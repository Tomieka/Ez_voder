#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QProgressBar>
#include <QTextEdit>
#include <QGroupBox>
#include <QDir>
#include <QMessageBox>
#include <QFile>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), process(nullptr)
{
    setupUi();
    setupStyle();
}

MainWindow::~MainWindow()
{
    if (process) {
        process->kill();
        delete process;
    }
}

void MainWindow::setupUi()
{
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    setupHeader(mainLayout);
    setupTwitchSection(mainLayout);
    setupYoutubeSection(mainLayout);

    setCentralWidget(centralWidget);
    setMinimumSize(500, 600);
}

void MainWindow::setupHeader(QVBoxLayout* mainLayout)
{
    QLabel* titleLabel = new QLabel("Ez_voder", this);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setMinimumHeight(50);
    titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mainLayout->addWidget(titleLabel);

    QLabel* descLabel = new QLabel("Download Twitch clips/VODs and upload them to YouTube", this);
    descLabel->setObjectName("descriptionLabel");
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    mainLayout->addWidget(descLabel);
}

void MainWindow::setupTwitchSection(QVBoxLayout* mainLayout)
{
    QGroupBox* twitchGroup = new QGroupBox("Twitch Download", this);
    QVBoxLayout* twitchLayout = new QVBoxLayout(twitchGroup);
    twitchLayout->setSpacing(10);
    twitchLayout->setContentsMargins(10, 20, 10, 10);

    QHBoxLayout* urlLayout = new QHBoxLayout();
    urlLayout->addWidget(new QLabel("URL:", this));
    twitchUrlInput = new QLineEdit(this);
    urlLayout->addWidget(twitchUrlInput);
    twitchLayout->addLayout(urlLayout);

    QHBoxLayout* qualityLayout = new QHBoxLayout();
    qualityLayout->addWidget(new QLabel("Quality:", this));
    qualityChoice = new QComboBox(this);
    qualityChoice->addItems({"1080p", "720p", "480p", "360p"});
    qualityLayout->addWidget(qualityChoice);
    twitchLayout->addLayout(qualityLayout);

    downloadBtn = new QPushButton("Download", this);
    connect(downloadBtn, &QPushButton::clicked, this, &MainWindow::onTwitchDownload);
    twitchLayout->addWidget(downloadBtn);

    statusLabel = new QLabel(this);
    statusLabel->setObjectName("statusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    twitchLayout->addWidget(statusLabel);

    progressBar = new QProgressBar(this);
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->hide();
    twitchLayout->addWidget(progressBar);

    mainLayout->addWidget(twitchGroup);
}

void MainWindow::setupYoutubeSection(QVBoxLayout* mainLayout)
{
    QGroupBox* ytGroup = new QGroupBox("YouTube Upload", this);
    QVBoxLayout* ytLayout = new QVBoxLayout(ytGroup);
    ytLayout->setSpacing(10);
    ytLayout->setContentsMargins(10, 20, 10, 10);

    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->addWidget(new QLabel("Title:", this));
    videoTitleInput = new QLineEdit(this);
    titleLayout->addWidget(videoTitleInput);
    ytLayout->addLayout(titleLayout);

    ytLayout->addWidget(new QLabel("Description:", this));
    videoDescInput = new QTextEdit(this);
    videoDescInput->setMinimumHeight(100);
    ytLayout->addWidget(videoDescInput);

    publicCheckbox = new QCheckBox("Make video public", this);
    publicCheckbox->setContentsMargins(0, 10, 0, 10);
    ytLayout->addWidget(publicCheckbox);
    ytLayout->addSpacing(5);

    uploadBtn = new QPushButton("Upload to YouTube", this);
    uploadBtn->setEnabled(false);
    connect(uploadBtn, &QPushButton::clicked, this, &MainWindow::onYoutubeUpload);
    ytLayout->addWidget(uploadBtn);

    mainLayout->addWidget(ytGroup);
}

void MainWindow::setupStyle()
{
    setWindowTitle("Ez_voder");
    
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    setWindowFlags(Qt::Window);
}

void MainWindow::onTwitchDownload()
{
    if (twitchUrlInput->text().isEmpty()) {
        statusLabel->setText("Please enter a URL");
        return;
    }

    downloadBtn->setEnabled(false);
    uploadBtn->setEnabled(false);
    progressBar->setValue(0);
    progressBar->show();
    statusLabel->setText("Starting download...");

    QString twitchDlPath = QDir(QDir::currentPath()).absoluteFilePath("twitch-dl.pyz");
    
    if (process) {
        delete process;
    }
    process = new QProcess(this);
    process->setWorkingDirectory(QDir::currentPath());
    process->setProcessChannelMode(QProcess::MergedChannels);
    
    QStringList arguments;
    arguments << twitchDlPath
              << "download"
              << twitchUrlInput->text()
              << "--quality"
              << qualityChoice->currentText();

    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::onProcessOutput);
    connect(process, &QProcess::errorOccurred, this, &MainWindow::onProcessError);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onProcessFinished);

    process->start("python3", arguments);

    if (!process->waitForStarted(3000)) {
        statusLabel->setText("Failed to start the download process");
        downloadBtn->setEnabled(true);
        progressBar->hide();
        return;
    }

    statusLabel->setText("Download started...");
}

void MainWindow::onYoutubeUpload()
{
    if (videoTitleInput->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a video title");
        return;
    }

    if (!setupYoutubeAuth()) {
        return;
    }

    uploadBtn->setEnabled(false);
    progressBar->setValue(0);
    progressBar->show();
    statusLabel->setText("Starting upload...");

    if (process) {
        delete process;
    }
    process = new QProcess(this);
    process->setWorkingDirectory(QDir::currentPath());
    process->setProcessChannelMode(QProcess::MergedChannels);

    QString pythonScript = QDir::currentPath() + "/upload_youtube.py";
    QStringList arguments;
    arguments << pythonScript
              << lastDownloadedFile
              << videoTitleInput->text()
              << videoDescInput->toPlainText()
              << (publicCheckbox->isChecked() ? "public" : "private");

    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::onProcessOutput);
    connect(process, &QProcess::errorOccurred, this, &MainWindow::onProcessError);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onProcessFinished);

    process->start("python3", arguments);

    if (!process->waitForStarted(3000)) {
        statusLabel->setText("Failed to start the upload process");
        uploadBtn->setEnabled(true);
        progressBar->hide();
        return;
    }

    statusLabel->setText("Upload started...");
}

void MainWindow::onProcessOutput()
{
    if (!process) return;
    
    QString output = QString::fromUtf8(process->readAllStandardOutput());
    
    if (!output.trimmed().isEmpty()) {
        statusLabel->setText(output.trimmed());
    }
    
    if (output.contains("%")) {
        QString percentStr = output.section("%", 0, 0).trimmed();
        percentStr = percentStr.section(" ", -1, -1);
        bool ok;
        int percent = percentStr.toInt(&ok);
        if (ok && percent >= 0 && percent <= 100) {
            progressBar->setValue(percent);
        }
    }
    
    if (output.contains("error", Qt::CaseInsensitive)) {
        statusLabel->setText("Error: " + output.trimmed());
    }
}

void MainWindow::onProcessError(QProcess::ProcessError error)
{
    QString errorMessage = "Error occurred: ";
    switch (error) {
        case QProcess::FailedToStart:
            errorMessage += "Failed to start process";
            break;
        case QProcess::Crashed:
            errorMessage += "Process crashed";
            break;
        default:
            errorMessage += "Unknown error occurred";
    }
    statusLabel->setText(errorMessage);
    progressBar->hide();
    downloadBtn->setEnabled(true);
}

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        statusLabel->setText("Operation completed successfully!");
        uploadBtn->setEnabled(true);
        
        QDir dir(QDir::currentPath());
        QStringList filters;
        filters << "*.mp4";
        QStringList files = dir.entryList(filters, QDir::Files, QDir::Time);
        if (!files.isEmpty()) {
            lastDownloadedFile = dir.absoluteFilePath(files.first());
        }
    } else {
        statusLabel->setText("Operation failed with error code: " + QString::number(exitCode));
    }

    downloadBtn->setEnabled(true);
    progressBar->hide();
}

bool MainWindow::setupYoutubeAuth()
{
    if (!QFile::exists("client_secrets.json")) {
        QMessageBox::warning(this, "Error",
                           "client_secrets.json not found!\n"
                           "Please obtain it from Google Cloud Console and place it in:\n" +
                           QDir::currentPath());
        return false;
    }

    statusLabel->setText("Starting YouTube authentication...");

    QProcess authProcess;
    authProcess.setWorkingDirectory(QDir::currentPath());
    authProcess.setProcessChannelMode(QProcess::MergedChannels);

    QString pythonScript = QDir::currentPath() + "/setup_youtube.py";
    QStringList arguments;
    arguments << pythonScript;

    authProcess.start("python3", arguments);

    if (!authProcess.waitForStarted(3000)) {
        statusLabel->setText("Failed to start YouTube authentication");
        QMessageBox::warning(this, "Error", "Failed to start YouTube authentication process");
        return false;
    }

    if (!authProcess.waitForFinished(30000)) {
        statusLabel->setText("YouTube authentication timed out");
        QMessageBox::warning(this, "Error", "YouTube authentication process timed out");
        return false;
    }

    QString output = QString::fromUtf8(authProcess.readAllStandardOutput());
    QString error = QString::fromUtf8(authProcess.readAllStandardError());

    if (authProcess.exitCode() != 0) {
        statusLabel->setText("YouTube authentication failed");
        QMessageBox::warning(this, "Error",
                           "Failed to authenticate with YouTube.\n"
                           "Error: " + error + "\n" +
                           "Output: " + output);
        return false;
    }

    statusLabel->setText("YouTube authentication successful");
    return true;
}

QString MainWindow::getCleanVideoId(const QString& url)
{
    QStringList parts = url.split('/');
    if (!parts.isEmpty()) {
        QString lastPart = parts.last();
        if (lastPart.contains('?')) {
            lastPart = lastPart.split('?').first();
        }
        return lastPart;
    }
    return QString();
} 