#include "trayicon.h"

#include <KAboutData>
#include <KLocale>
#include <QEvent>
#include <QDebug>
#include <QAction>
#include <QMenu>
#include <QTimer>
#include <KActionCollection>
#include <KStandardAction>
#include <QDebug>
#include <KApplication>
#include <KHelpMenu>
#include <KGlobal>

TrayIcon::TrayIcon(QObject *parent)
    : KStatusNotifierItem(parent)
{
    setIconByName("ksnapshot");
    setCategory(KStatusNotifierItem::ApplicationStatus);
    setStatus(KStatusNotifierItem::Active);
    setToolTip("ksnapshot", "Kaption", i18n("Left click to start grabbing the screen"));
    setStandardActionsEnabled(false);

    // Delay init so that KAboutData has been initialized
    QTimer::singleShot(0, this, &TrayIcon::fillContextMenu);
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::fillContextMenu()
{
    QMenu *menu = contextMenu();

    menu->addAction(KStandardAction::open(qApp, SLOT(slotOpenImageFileBrowser()), this));
    menu->addSeparator();
    menu->addAction(KStandardAction::preferences(qApp, SLOT(slotConfigKaption()), this));
    menu->addAction(KStandardAction::keyBindings(qApp, SLOT(slotConfigShortcuts()), this));
    menu->addSeparator();

    KHelpMenu *helpMenu = new KHelpMenu(menu, KAboutData::applicationData(), false);
    menu->addMenu(helpMenu->menu());
    menu->addSeparator();

    menu->addAction(KStandardAction::quit(this, SLOT(maybeQuit()), this));
}
