#include "generalsettingspage.h"
#include "ui_generalsettingspage.h"

#include <KFileDialog>

GeneralSettingsPage::GeneralSettingsPage(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::GeneralSettingsPage)
{
    ui->setupUi(this);

    ui->selectDefaultSaveLocationButton->setIcon(QIcon::fromTheme("folder-open"));

    connect(ui->selectDefaultSaveLocationButton, SIGNAL(clicked()),
            this, SLOT(selectDefaultSaveLocationUrl()));
}

GeneralSettingsPage::~GeneralSettingsPage()
{
    delete ui;
}

void GeneralSettingsPage::selectDefaultSaveLocationUrl()
{
    QString defaultSaveLocationUrl = ui->kcfg_DefaultSaveLocationUrl->text();
    if (defaultSaveLocationUrl.isEmpty()) {
        defaultSaveLocationUrl = QDir::homePath();
    }
    QUrl url = KFileDialog::getExistingDirectoryUrl(defaultSaveLocationUrl, this);
    if (!url.isEmpty()) {
        ui->kcfg_DefaultSaveLocationUrl->setText(url.toString());
    }
}
