// KDE
#include <KCmdLineArgs>
#include <KUniqueApplication>
#include <KAboutData>
#include <KLocale>
#include <KStatusNotifierItem>
#include <QDebug>

#include "kaptionapplication.h"

int main(int argc, char *argv[])
{
    KAboutData aboutData("kaption", // Internal use. DO NOT CHANGE!
                         0,
                         ki18n("Kaption"), // Displayable one (ie: program name)
                         "0.1 beta",
                         ki18n("Screenshot grabber and editor for KDE"),
                         KAboutData::License_GPL_V2,
                         ki18n("Copyright (C) 2011 Francesco Di Muccio"),
                         ki18n("Kaption is a screenshot utility similar to Skitch or Jing"),
                         "",
                         "francesco.dimuccio@gmail.com");

    aboutData.addAuthor(ki18n("Francesco Di Muccio"),
                        ki18n("Author"),
                        "francesco.dimuccio@gmail.com");

    // Must be changed with a proper icon
    aboutData.setProgramIconName("ksnapshot");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("capture", ki18n("Captures the desktop"));
    KCmdLineArgs::addCmdLineOptions(options);

    //KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    QApplication::setGraphicsSystem("native");

    KaptionApplication::addCmdLineOptions();
    KaptionApplication app;

    return app.exec();
}

