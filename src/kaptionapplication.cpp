#include "kaptionapplication.h"

#include <QShortcut>
#include <QSizePolicy>
#include <QDebug>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QImageReader>
#include <KActionCollection>
#include <KHelpMenu>
#include <QTimer>
#include <QLayout>
#include <KGlobalAccel>
#include <KShortcutsDialog>
#include <KMessageBox>
#include <KLocalizedString>
#include "settings.h"
#include "trayicon.h"
#include "grabber.h"
#include "snapshotpreview.h"
#include "settings/kaptionsettingsdialog.h"

KaptionApplication::KaptionApplication(int& argc, char**&argv)
    : QApplication(argc, argv)
{
    initGUI();
    setupActions();
    QTimer::singleShot(0, this, SLOT(initObject()));
}

KaptionApplication::~KaptionApplication()
{
    if (m_preview) {
        m_preview->deleteLater();
    }
    delete m_trayIcon;
}

void KaptionApplication::setupActions()
{
    m_actionCollection = new KActionCollection(this);
    m_actionCollection->setObjectName("Kaption-KActionCollection");

    QAction *capture = new QAction(i18n("Capture desktop"), m_actionCollection);
    capture->setObjectName("Kaption-captureDesktop");
    m_actionCollection->addAction("capture", capture);
    KGlobalAccel::self()->setGlobalShortcut(capture, Qt::META + Qt::Key_Print);
    connect(capture, SIGNAL(triggered(bool)),
            this, SLOT(captureScreen()));
}

void KaptionApplication::initGUI()
{
    setQuitOnLastWindowClosed(false);
    m_trayIcon = new TrayIcon(this);
    setupTrayIcon();
}

void KaptionApplication::initObject()
{
    m_preview = 0;
}

void KaptionApplication::captureScreen()
{
    if (m_trayIcon != 0) {
        m_trayIcon->activate();
    }
}

void KaptionApplication::slotRegionGrabbed(const QPixmap &pixmap, const QPoint &topLeft)
{
    slotQuitGrabber();
    openEditor(pixmap, topLeft);
}

void KaptionApplication::slotQuitGrabber()
{
    /**
        @todo: This is ugly. It's useless to delete and create
               each time a new Grabber widget. But in some cases it would not
               work, See grabber.cpp...it should be fixed there.
    */
    Grabber *g = dynamic_cast<Grabber*>(m_trayIcon->associatedWidget());
    if (g != 0) {
        g->close();
    }
    setupTrayIcon();
}

void KaptionApplication::slotConfigKaption(const QString &page)
{
    KaptionSettingsDialog *dialog =
            dynamic_cast<KaptionSettingsDialog*>(KConfigDialog::exists("settings"));
    if (dialog) {
        dialog->close();
        dialog->deleteLater();
    }
    dialog = new KaptionSettingsDialog(0, "settings", Settings::self());
    dialog->show(page);
}

void KaptionApplication::slotConfigShortcuts()
{
    KShortcutsDialog::configure(m_actionCollection);
}

void KaptionApplication::slotOpenImageFileBrowser()
{
    QStringList mimetypes;
    for (const QByteArray& mimetype : QImageReader::supportedMimeTypes()) {
        mimetypes << QString::fromUtf8(mimetype);
    }
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setMimeTypeFilters(mimetypes);
    fileDialog.setDirectory(QDir::homePath());
    if (!fileDialog.exec()) {
        return;
    }
    QString filename = fileDialog.selectedFiles().first();

    QFile file(filename);
    QString errorString;
    if (file.open(QIODevice::ReadOnly)) {
        QPixmap pixmap(filename);
        if (!openEditor(pixmap)) {
            errorString = i18n("<b>%1</b> is not a valid image file", filename);
        }
    } else {
        errorString = i18n("Could not read <b>%1</b>: %2", filename, file.errorString());
    }

    if (!errorString.isEmpty()) {
        KMessageBox::error(0, errorString);
    }
}

void KaptionApplication::setupTrayIcon()
{
    Grabber *grabber = new Grabber();
    grabber->setAttribute(Qt::WA_DeleteOnClose);
    m_trayIcon->setAssociatedWidget(grabber);

    connect(grabber, SIGNAL(regionGrabbed(QPixmap, QPoint)),
            this, SLOT(slotRegionGrabbed(QPixmap, QPoint)));
    connect(grabber, SIGNAL(quitRequested()),
            this, SLOT(slotQuitGrabber()));
}

bool KaptionApplication::openEditor(const QPixmap &pixmap, const QPoint &tl)
{
    if (pixmap.isNull()) return false;

    bool instantiated = false;
    if (!m_preview) {
        m_preview = new SnapshotPreview();
        m_preview->setAttribute(Qt::WA_DeleteOnClose);
        instantiated = true;
    }
    m_preview->setPixmap(pixmap);
    m_preview->adjustSize();
    m_preview->show();
    m_preview->raise();
    m_preview->activateWindow();

    if (instantiated && !tl.isNull()) {
        m_preview->move(tl);
    }

    return true;
}
