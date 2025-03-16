#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QComboBox;
class QTextEdit;
class QPushButton;
class QCheckBox;
class QProgressBar;
class QLabel;
class QVBoxLayout;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTwitchDownload();
    void onYoutubeUpload();
    void onProcessOutput();
    void onProcessError(QProcess::ProcessError error);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void setupUi();
    void setupStyle();
    void setupHeader(QVBoxLayout* mainLayout);
    void setupTwitchSection(QVBoxLayout* mainLayout);
    void setupYoutubeSection(QVBoxLayout* mainLayout);
    bool setupYoutubeAuth();
    QString getCleanVideoId(const QString& url);

    QLineEdit* twitchUrlInput;
    QComboBox* qualityChoice;
    QPushButton* downloadBtn;
    QPushButton* uploadBtn;
    QLineEdit* videoTitleInput;
    QTextEdit* videoDescInput;
    QCheckBox* publicCheckbox;
    QProgressBar* progressBar;
    QLabel* statusLabel;
    
    QProcess* process;
    QString lastDownloadedFile;
};

#endif // MAINWINDOW_H
