#include "kaptionsettingsdialog.h"

#include <KConfigSkeleton>
#include <KLocalizedString>

#include <QDebug>

#include "settings.h"

#include "ftpuploadsettingspage.h"
#include "generalsettingspage.h"

KaptionSettingsDialog::KaptionSettingsDialog(QWidget *parent,
                                             const char *name,
                                             KConfigSkeleton *config)
    : KConfigDialog(parent, name, config)
{
    addPage(new GeneralSettingsPage(this),
            i18n("General"),
            "system-run",
            i18n("Configure general settings"));

    addPage(new FtpUploadSettingsPage(this),
            i18n("FTP upload"),
            "upload",
            i18n("Configure FTP upload"));
}

KaptionSettingsDialog::~KaptionSettingsDialog()
{

}

void KaptionSettingsDialog::show(const QString &page)
{
    if (!page.isEmpty()) {
        if (m_pagesMap.contains(page)) {
            setCurrentPage(m_pagesMap.value(page));
        }
    }

    KConfigDialog::show();
    raise();
    activateWindow();
}

KPageWidgetItem *KaptionSettingsDialog::addPage(QWidget *page,
                                                const QString &itemName,
                                                const QString &pixmapName,
                                                const QString &header,
                                                bool manage)
{
    KPageWidgetItem *item = KConfigDialog::addPage(page, itemName,
                                                   pixmapName, header, manage);
    m_pagesMap.insert(page->metaObject()->className(), item);
    return item;
}

KPageWidgetItem *KaptionSettingsDialog::addPage(QWidget *page,
                                                KConfigSkeleton *config,
                                                const QString &itemName,
                                                const QString &pixmapName,
                                                const QString &header)
{
    KPageWidgetItem *item = KConfigDialog::addPage(page, config, itemName,
                                                   pixmapName, header);
    m_pagesMap.insert(page->metaObject()->className(), item);
    return item;
}
