#include "trayicon.h"

#include <KAboutData>
#include <KLocale>
#include <KMenu>
#include <QEvent>
#include <QDebug>
#include <QAction>
#include <QTimer>
#include <KAction>
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

    menu->addAction(KStandardAction::open(kapp, SLOT(slotOpenImageFileBrowser()), this));
    menu->addSeparator();
    menu->addAction(KStandardAction::preferences(kapp, SLOT(slotConfigKaption()), this));
    menu->addAction(KStandardAction::keyBindings(kapp, SLOT(slotConfigShortcuts()), this));
    menu->addSeparator();

    KHelpMenu *helpMenu = new KHelpMenu(menu, KAboutData::applicationData(), false);
    menu->addMenu(helpMenu->menu());
    menu->addSeparator();

    menu->addAction(KStandardAction::quit(this, SLOT(maybeQuit()), this));
}
