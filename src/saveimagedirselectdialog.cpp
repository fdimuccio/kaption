#include "saveimagedirselectdialog.h"

#include <KIO/NetAccess>
#include <QLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QString>
#include <KLocale>

#include "settings.h"

SaveImageDirSelectDialog::SaveImageDirSelectDialog(const QUrl &startDir,
                                                   bool localOnly,
                                                   QWidget *parent)
    : KDirSelectDialog(startDir, localOnly, parent),
      m_dontAskMeAgainCheckBox(0L)
{
    // PORTME
    /*
    QLayout *hlay = mainWidget()->layout();

    if (hlay && hlay->count() > 1) {
        QVBoxLayout *vlay = qobject_cast<QVBoxLayout*>(hlay->itemAt(1)->layout());
        if (vlay) {
            m_dontAskMeAgainCheckBox = new QCheckBox(i18n("Don't ask me again"),
                                          mainWidget());
            QString tooltipText =
                    i18n("This option will set the choosen directoy as default,"
                         " next time you will not be prompted where to save.\n"
                         "This behavior can be changed from the settings panel");
            m_dontAskMeAgainCheckBox->setToolTip(tooltipText);
            vlay->addWidget(m_dontAskMeAgainCheckBox);
        }
    }
    */
}

QUrl SaveImageDirSelectDialog::selectDirectory(const QUrl &startDir,
                                               bool localOnly,
                                               QWidget *parent,
                                               const QString &caption)
{
    SaveImageDirSelectDialog dlg(startDir, localOnly, parent);

    if (!caption.isNull()) dlg.setWindowTitle(caption);

    if (dlg.exec() == QDialog::Accepted) {
        if (dlg.dontAskMeAgain()) {
            Settings::setDefaultSaveLocationUrl(dlg.url().url());
            Settings::setDontAskSaveLocation(true);
            Settings::self()->writeConfig();

        }
        return KIO::NetAccess::mostLocalUrl(dlg.url(), parent);
    } else {
        return QUrl();
    }
}

bool SaveImageDirSelectDialog::dontAskMeAgain() const
{
    if (!m_dontAskMeAgainCheckBox) return false;
    return m_dontAskMeAgainCheckBox->isChecked();
}
