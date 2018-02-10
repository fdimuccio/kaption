#ifndef KaptionApplication_H
#define KaptionApplication_H

#include <QApplication>
#include <QPointer>
#include <QDebug>

class TrayIcon;
class Grabber;
class SnapshotPreview;
class KActionCollection;

class KaptionApplication : public QApplication
{
    Q_OBJECT

public:
    KaptionApplication(int& argc, char**&argv);
    virtual ~KaptionApplication();

    virtual int newInstance();

public Q_SLOTS:
    void slotConfigKaption(const QString &page = QString());
    void slotConfigShortcuts();
    void slotOpenImageFileBrowser();

private Q_SLOTS:
    void slotRegionGrabbed(const QPixmap &pixmap, const QPoint &topLeft);
    void slotQuitGrabber();
    void initObject();
    void captureScreen();

private:
    void setupActions();
    void initGUI();
    void setupTrayIcon();
    bool openEditor(const QPixmap &pixmap, const QPoint &topLeft = QPoint());

    TrayIcon *m_trayIcon;
    QPointer<SnapshotPreview> m_preview;
    KActionCollection *m_actionCollection;
};

#endif // KaptionApplication_H
