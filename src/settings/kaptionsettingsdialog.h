#ifndef KAPTIONSETTINGSDIALOG_H
#define KAPTIONSETTINGSDIALOG_H

#include <KConfigDialog>

class KConfigSkeleton;

class KaptionSettingsDialog : public KConfigDialog
{
    Q_OBJECT

public:
    explicit KaptionSettingsDialog(QWidget *parent,
                                   const char *name,
                                   KConfigSkeleton *config);
    ~KaptionSettingsDialog();

public Q_SLOTS:
    void show(const QString &page);

private:
    KPageWidgetItem *addPage(QWidget *page,
                             const QString &itemName,
                             const QString &pixmapName = QString(),
                             const QString &header = QString(),
                             bool manage = true);

    KPageWidgetItem *addPage(QWidget *page,
                             KConfigSkeleton *config,
                             const QString &itemName,
                             const QString &pixmapName = QString(),
                             const QString &header = QString());

    QMap<QString, KPageWidgetItem*> m_pagesMap;
};

#endif // KAPTIONSETTINGSDIALOG_H
