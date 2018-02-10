// KDE
#include <KAboutData>
#include <KLocale>
#include <KStatusNotifierItem>

#include <QCommandLineParser>
#include <QDebug>

#include "kaptionapplication.h"

int main(int argc, char *argv[])
{
    KaptionApplication app(argc, argv);

    // PORTME
    /*
    KAboutData aboutData("kaption", // Internal use. DO NOT CHANGE!
                         0,
                         ki18n("Kaption"), // Displayable one (ie: program name)
                         "0.1 beta",
                         ki18n("Screenshot grabber and editor for KDE"),
                         KAboutLicense::GPL_V2,
                         ki18n("Copyright (C) 2011 Francesco Di Muccio"),
                         ki18n("Kaption is a screenshot utility similar to Skitch or Jing"),
                         "",
                         "francesco.dimuccio@gmail.com");

    aboutData.addAuthor(ki18n("Francesco Di Muccio"),
                        ki18n("Author"),
                        "francesco.dimuccio@gmail.com");

    // Must be changed with a proper icon
    aboutData.setProgramIconName("ksnapshot");
    */

    QCommandLineParser parser;
    // PORTME
    //parser.addOption("capture", ki18n("Captures the desktop"));

    QApplication::setGraphicsSystem("native");

    return app.exec();
}

