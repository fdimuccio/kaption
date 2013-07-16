#include "ftpuploadsettingspage.h"
#include "ui_ftpuploadsettingspage.h"

#include "settings.h"

FtpUploadSettingsPage::FtpUploadSettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FtpUploadSettingsPage)
{
    ui->setupUi(this);

    connect(ui->kcfg_Type, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateDefaultPort()));
}

FtpUploadSettingsPage::~FtpUploadSettingsPage()
{
    delete ui;
}

void FtpUploadSettingsPage::updateDefaultPort()
{
    if (ui->kcfg_Type->currentIndex() == Settings::EnumType::FTP) {
        ui->kcfg_Port->setValue(21);
    } else if (ui->kcfg_Type->currentIndex() == Settings::EnumType::SFTP) {
        ui->kcfg_Port->setValue(22);
    }
}

void FtpUploadSettingsPage::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
