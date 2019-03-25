// KDE
#include <KAboutData>
#include <KDBusService>
#include <KLocalizedString>
#include <KStatusNotifierItem>

#include <QCommandLineParser>
#include <QDebug>
#include <QIcon>

#include "kaptionapplication.h"

int main(int argc, char *argv[])
{
    KaptionApplication app(argc, argv);

    KAboutData aboutData("kaption", // Internal use. DO NOT CHANGE!
                         i18n("Kaption"), // Displayable one (ie: program name)
                         "0.1 beta");
    aboutData.setShortDescription(i18n("Screenshot grabber and editor for KDE"));
    aboutData.setLicense(KAboutLicense::GPL_V2);
    aboutData.setCopyrightStatement(i18n("Copyright (C) 2011 Francesco Di Muccio"));
    aboutData.setOtherText(i18n("Kaption is a screenshot utility similar to Skitch or Jing"));
    aboutData.setBugAddress("francesco.dimuccio@gmail.com");

    aboutData.addAuthor(i18n("Francesco Di Muccio"),
                        i18n("Author"),
                        "francesco.dimuccio@gmail.com");

    KAboutData::setApplicationData(aboutData);

    // Must be changed with a proper icon
    QApplication::setWindowIcon(QIcon::fromTheme("ksnapshot"));

    KDBusService service(KDBusService::Unique);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption({"capture", i18n("Captures the desktop")});
    parser.process(app);
    if (parser.isSet("capture")) {
        app.captureScreen();
    }

    return app.exec();
}

