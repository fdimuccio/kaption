// Here we avoid loading the header multiple times
#ifndef SNAPSHOTPREVIEW_H
#define SNAPSHOTPREVIEW_H

#include <QPixmap>
#include <QFrame>
#include <QPointer>
#include <QClipboard>

namespace Ui {
    class SnapshotPreview;
}

namespace KIO {
    class Job;
    class CopyJob;
}

class QIODevice;
class QByteArray;
class QGraphicsView;
class QGraphicsScene;
class KJob;
class QTemporaryFile;
class UploadProgressDialog;
class QLabel;
class ShapeWidth;
class QPushButton;
class QSignalMapper;
class KaptionGraphicsItem;
class KaptionGraphicsToolkit;

class SnapshotPreview: public QFrame
{
    Q_OBJECT

public:
    explicit SnapshotPreview(QWidget *parent = 0);
    SnapshotPreview(const QPixmap &pixmap, QWidget *parent = 0);
    ~SnapshotPreview();

    void setPixmap(const QPixmap &pixmap);

protected:
    void showEvent(QShowEvent *e);
    void resizeEvent(QResizeEvent *e);
    void closeEvent(QCloseEvent *e);

private Q_SLOTS:
    void slotUpload();
    void slotUploading(KJob *job, unsigned long percent);
    void slotUploadResult(KJob *job);
    void slotPrintUploadInfo(KJob *job, const QString &plain);
    void slotCancelUpload();
    void slotCopy();
    void slotSaveAs();

private:
    enum SaveAndContinueReason {
        EditorClosing,
        PixmapChanging
    };

    void init();
    QPixmap getFinalPixmap() const;
    bool saveImage(QIODevice *device, const QByteArray &format);
    void setToolbarsMaxSize();
    QString filenameFromLineEdit();
    QByteArray imageFormatFromComboBox() const;
    QString generateScreenFilename() const;
    bool saveAndContinueAction(SaveAndContinueReason reason);

    KIO::CopyJob *m_currentJob;
    UploadProgressDialog *m_progressDialog;
    QPointer<QTemporaryFile> m_tmpFile;
    Ui::SnapshotPreview *ui;
    KaptionGraphicsToolkit *m_toolkit;
    QString m_lastError;
    QPixmap m_lastGeneratedPixmap;
    bool m_screenSaved;
    bool m_pixmapSet;
    QClipboard *m_clipboard;
};

#endif  //  SNAPSHOTPREVIEW_H

