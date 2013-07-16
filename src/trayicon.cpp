#include "trayicon.h"

#include <KLocale>
#include <KMenu>
#include <QEvent>
#include <QDebug>
#include <QAction>
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

    KMenu *menu = contextMenu();
    menu->addAction(KStandardAction::open(kapp, SLOT(slotOpenImageFileBrowser()), this));
    menu->addSeparator();
    menu->addAction(KStandardAction::preferences(kapp, SLOT(slotConfigKaption()), this));
    menu->addAction(KStandardAction::keyBindings(kapp, SLOT(slotConfigShortcuts()), this));
    menu->addSeparator();
    KHelpMenu *helpMenu = new KHelpMenu(menu, KGlobal::mainComponent().aboutData(), false);
    menu->addMenu(helpMenu->menu());
    menu->addSeparator();
    menu->addAction(KStandardAction::quit(this, SLOT(maybeQuit()), this));
}

TrayIcon::~TrayIcon()
{
}
