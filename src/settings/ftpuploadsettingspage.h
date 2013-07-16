#ifndef FTPUPLOADSETTINGSPAGE_H
#define FTPUPLOADSETTINGSPAGE_H

#include <QWidget>

namespace Ui {
    class FtpUploadSettingsPage;
}

class FtpUploadSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit FtpUploadSettingsPage(QWidget *parent = 0);
    ~FtpUploadSettingsPage();

protected Q_SLOTS:
    void updateDefaultPort();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FtpUploadSettingsPage *ui;
};

#endif // FTPUPLOADSETTINGSPAGE_H
