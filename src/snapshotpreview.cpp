#include "snapshotpreview.h"
#include "ui_snapshotpreview.h"

// Qt headers
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QCloseEvent>
#include <QPointer>
#include <QFileDialog>
#include <KMessageBox>
#include <QByteArray>
#include <QFile>
#include <QIODevice>
#include <QImageWriter>
#include <QTemporaryFile>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPainter>
#include <QSignalMapper>
#include <KMessageBox>
#include <QClipboard>
#include <QStyle>
#include <QSizePolicy>
#include <KNotification>
#include <KLocalizedString>
#include <KIO/CopyJob>
#include <KIO/DeleteJob>
#include <KIO/StatJob>
#include <KJobWidgets>
#include <QRegExp>
#include "settings.h"
#include "scale.h"
#include "kaptionapplication.h"
#include "graphic/snapshotcanvas.h"
#include "graphic/items/boxtextgraphicsitem.h"
#include "kaptiongraphicsitem.h"
#include "uploadprogressdialog.h"
#include "kaptiongraphicstoolkit.h"

#include "colorpropertytooleditor.h"
#include "scalepropertytooleditor.h"
#include "fontpropertytooleditor.h"
#include "numberpropertytooleditor.h"

#include "items/arrowgraphicsitem.h"
#include "items/boxgraphicsitem.h"
#include "items/ellipsegraphicsitem.h"
#include "items/numbereditem.h"
#include "items/textgraphicsitem.h"

SnapshotPreview::SnapshotPreview(QWidget *parent)
    : QFrame(parent)
{
    init();
}

SnapshotPreview::SnapshotPreview(const QPixmap &pixmap, QWidget *parent)
    : QFrame(parent)
{
    init();
    setPixmap(pixmap);
}

SnapshotPreview::~SnapshotPreview()
{
    delete ui;
    if (m_tmpFile) {
        m_tmpFile->deleteLater();
    }
}

void SnapshotPreview::init()
{
    m_tmpFile = 0;
    m_progressDialog = 0;
    m_screenSaved = false;
    m_pixmapSet = false;

    ui = new Ui::SnapshotPreview;
    ui->setupUi(this);

    ui->arrowBtn->setIcon(QIcon::fromTheme("toolbox_arrow"));
    ui->boxBtn->setIcon(QIcon::fromTheme("toolbox_box"));
    ui->ellipseBtn->setIcon(QIcon::fromTheme("toolbox_ellipse"));
    ui->textBtn->setIcon(QIcon::fromTheme("toolbox_text"));
    ui->numberedBtn->setIcon(QIcon::fromTheme("toolbox_number"));

    ui->formatTextBtn->setIcon(QIcon::fromTheme("draw-text"));

    ui->cancelBtn->setIcon(QIcon::fromTheme("dialog-cancel"));
    ui->saveBtn->setIcon(QIcon::fromTheme("document-save-as"));
    ui->uploadBtn->setIcon(QIcon::fromTheme("upload"));

    m_toolkit = new KaptionGraphicsToolkit(ui->propertyToolbar, this);

    m_toolkit->bindButtonToGraphicsItem<ArrowGraphicsItem>(ui->arrowBtn, true);
    m_toolkit->bindButtonToGraphicsItem<BoxGraphicsItem>(ui->boxBtn);
    m_toolkit->bindButtonToGraphicsItem<EllipseGraphicsItem>(ui->ellipseBtn);
    m_toolkit->bindButtonToGraphicsItem<NumberedItem>(ui->numberedBtn);
    m_toolkit->bindButtonToGraphicsItem<BoxTextGraphicsItem>(ui->textBtn);

    m_toolkit->bindPropertyTool(new ColorPropertyToolEditor(ui->colorBtn, this), "color");
    m_toolkit->bindPropertyTool(new ScalePropertyToolEditor(ui->widthSlider, this), "size");
    m_toolkit->bindPropertyTool(new FontPropertyToolEditor(ui->formatTextBtn, this), "font");
    m_toolkit->bindPropertyTool(new NumberPropertyToolEditor(this), "number");

    m_toolkit->updateUi();

    ui->snapshotCanvas->setToolkit(m_toolkit);

    connect(ui->cancelBtn, SIGNAL(clicked()),
            this, SLOT(close()));
    connect(ui->saveBtn, SIGNAL(clicked()),
            this, SLOT(slotSaveAs()));
    connect(ui->uploadBtn, SIGNAL(clicked()),
            this, SLOT(slotUpload()));
}

void SnapshotPreview::setPixmap(const QPixmap &pixmap)
{
    saveAndContinueAction(PixmapChanging);
    ui->snapshotCanvas->setPixmap(pixmap);
    setToolbarsMaxSize();
    ui->filenameLineEdit->setText(generateScreenFilename());
    m_screenSaved = false;
    m_pixmapSet = true;
}

// Simplified copy of NetAccess::exists from KDELibs4Support
static bool destinationExists(const QUrl &url, QWidget *window)
{
    if (url.isLocalFile()) {
        return QFile::exists(url.toLocalFile());
    }

    KIO::JobFlags flags = url.isLocalFile() ? KIO::HideProgressInfo : KIO::DefaultFlags;
    KIO::StatJob *job = KIO::stat(url, flags);
    KJobWidgets::setWindow(job, window);
    job->setSide(KIO::StatJob::DestinationSide);
    return job->exec();
}

//TODO: Needs refactoring!
void SnapshotPreview::slotSaveAs()
{
    QUrl locationUrl;
    if (!Settings::dontAskSaveLocation()) {
        QString startingUrl =  Settings::lastSaveLocationUrl();
        if (startingUrl.isEmpty()) startingUrl = QDir::homePath();
        locationUrl = QFileDialog::getExistingDirectoryUrl(this, QString(), startingUrl);

        // Action aborted by user
        if (locationUrl.isEmpty()) return;
    } else {
        locationUrl.setUrl(Settings::defaultSaveLocationUrl());

        if (!locationUrl.isValid()) {
            const QString caption = i18n("Invalid location");
            QString text;
            if (locationUrl.isEmpty()) {
                text = i18n("No location has been choosen, check your settings.");
            } else {
                text = i18n("Choosen location is invalid: <br>%1<br><i>Reason: <b>%2</b>.</i>",
                            locationUrl.toString(), locationUrl.errorString());
            }
            KMessageBox::error(this, text, caption);
            return;
        }
    }

    QUrl url = locationUrl;
    url.setPath(url.path() + '/' + filenameFromLineEdit());

    if (destinationExists(url, this)) {
        const QString title = i18n("File Exists");
        const QString text = i18n("<qt>Do you really want to overwrite <b>%1</b>?</qt>", url.toString());
        if (KMessageBox::Continue != KMessageBox::warningContinueCancel(this, text, title, KGuiItem(i18n("Overwrite")))) {
            //delete dlg;
            return;
        }
    }

    QByteArray format = imageFormatFromComboBox();

    bool ok = false;

    if (url.isLocalFile()) {
        QFile output(url.toLocalFile());
        if (output.open(QFile::WriteOnly)) {
            ok = saveImage(&output, format);
        } else {
            m_lastError = output.errorString();
        }
    } else {
        QTemporaryFile tmpFile;
        if (tmpFile.open()) {
            if (saveImage(&tmpFile, format)) {
                KJob *job = KIO::copy(QUrl::fromLocalFile(tmpFile.fileName()), url);
                if (!job->exec()) m_lastError = job->errorString();
            }
        } else {
            m_lastError = tmpFile.errorString();
        }
    }

    QApplication::restoreOverrideCursor();
    if (!ok) {
        const QString caption = i18n("Unable to Save Image");
        const QString text = i18n("Kaption was unable to save the image to<br>%1<br><i>Reason: <b>%2</b>.</i>", url.toString(), m_lastError);
        KMessageBox::error(this, text, caption);
    } else {
        m_screenSaved = true;
        Settings::setLastSaveLocationUrl(locationUrl.url());
        Settings::self()->save();

        KNotification *notification = new KNotification("imagesaved", this);
        notification->setTitle("Kaption");
        notification->setPixmap(m_lastGeneratedPixmap);
        notification->setText(i18n("Image saved successfully into %1",
                                   locationUrl.toString()));
        notification->sendEvent();
    }

    //delete dlg;
}

void SnapshotPreview::slotUpload()
{
    Settings *settings = Settings::self();

    QString scheme    = settings->type() == Settings::EnumType::FTP ? "ftp" : "sftp";
    QString server    = settings->server();
    uint    port      = settings->port();
    QString directory = settings->directory();
    QString username  = settings->username();
    QString password  = settings->password();

    if (server.isEmpty() || port < 1) {
        QString txt = i18n("To upload the screenshot you must configure a connection.\n"
                           "Do it now?");
        QString title = i18n("Missing connection configuration");
        if (KMessageBox::warningYesNo(this, txt, title) == KMessageBox::Yes) {
            (static_cast<KaptionApplication*>(qApp))->slotConfigKaption("FtpConfig");
        }
    } else {
        // TODO: All this code should be placed in something like
        //       an upload service
        QString filename = filenameFromLineEdit();
        m_tmpFile =  new QTemporaryFile;
        if (m_tmpFile->open()) {
            if (saveImage(m_tmpFile, qPrintable(QFileInfo(filename).suffix()))) {
                QUrl url;
                url.setScheme(scheme);
                url.setHost(server);
                url.setPort(port);
                url.setPath(directory + "/" + filename);
                if (!username.isNull() && !password.isNull()) {
                    url.setUserName(username);
                    url.setPassword(password);
                }
                // Before uploading I must set permission to 0666
                m_tmpFile->setPermissions(QFile::ReadOwner | QFile::WriteOwner |
                                          QFile::ReadGroup | QFile::WriteGroup |
                                          QFile::ReadOther | QFile::WriteOther);
                m_currentJob = KIO::copy(QUrl::fromLocalFile(m_tmpFile->fileName()), url, KIO::HideProgressInfo);
                m_currentJob->setUiDelegate(0);
                connect(m_currentJob, SIGNAL(infoMessage(KJob*,QString)),
                        this, SLOT(slotPrintUploadInfo(KJob*,QString)));
                connect(m_currentJob, SIGNAL(description(KJob*,QString)),
                        this, SLOT(slotPrintUploadInfo(KJob*,QString)));
                connect(m_currentJob, SIGNAL(warning(KJob*,QString)),
                        this, SLOT(slotPrintUploadInfo(KJob*,QString)));
                connect(m_currentJob, SIGNAL(percent(KJob*,unsigned long)),
                        this, SLOT(slotUploading(KJob*,unsigned long)));
                connect(m_currentJob, SIGNAL(result(KJob*)),
                        this, SLOT(slotUploadResult(KJob*)));

                if (m_progressDialog == 0) {
                    m_progressDialog = new UploadProgressDialog(this, i18n("Upload progress"));
                    connect(m_progressDialog, SIGNAL(canceled()),
                            this, SLOT(slotCancelUpload()));
                }
                m_progressDialog->setLabelText(i18n("Uploading to %1", url.toString()));
                m_progressDialog->setValue(0);
                m_progressDialog->setButton(KStandardGuiItem::Cancel);
                m_progressDialog->show();
            } else {
                const QString caption = i18n("Unable to Save Image");
                const QString text = i18n("Kaption was unable to upload the image, reason: <b>%1</b>", m_lastError);
                KMessageBox::error(this, text, caption);
            }
        }
    }
}

void SnapshotPreview::slotPrintUploadInfo(KJob *job, const QString &plain)
{
    Q_UNUSED(job)
    m_progressDialog->setLabelText(plain);
}

void SnapshotPreview::slotUploading(KJob *job, unsigned long percent)
{
    Q_UNUSED(job)
    m_progressDialog->setValue(percent);
}

// TODO: Needs refactoring...it is horrible!
void SnapshotPreview::slotUploadResult(KJob *job)
{
    KIO::CopyJob *copyJob = static_cast<KIO::CopyJob*>(job);
    QString n_eventid;
    QString n_txt;
    QPixmap n_pixmap;
    //QStringList n_actions;

    if (job->error()) {
        QString text;
        if (!job->errorString().isEmpty()) {
            text = job->errorString();
        } else {
            text = i18n("Upload canceled");
        }

        QIcon icon = QIcon::fromTheme("dialog-close");

        // Setup "uploaderror" notification
        n_eventid = "uploaderror";
        n_txt = text;
        n_pixmap = icon.pixmap(64, 64);

        // Show error into the progress dialog
        m_progressDialog->setLabelText(text);
        m_progressDialog->setButton(KStandardGuiItem::Close);

        // Delete remote partial file
        QUrl partial = copyJob->destUrl();
        partial.setPath(partial.path()+".part");
        KIO::del(partial, KIO::HideProgressInfo);
    } else {
        if (m_progressDialog->isVisible()) {
            // Set 100% progress, so it looks pretty
            m_progressDialog->setValue(100);
            m_progressDialog->hide();
        }

        QString txt = i18n("Image uploaded");
        if (Settings::useClipboard() && !Settings::clipboardContents().isEmpty()) {
            QString clipboard = Settings::clipboardContents();
            clipboard.replace("%filename", copyJob->destUrl().fileName());
            QGuiApplication::clipboard()->setText(clipboard);
            if (clipboard.startsWith("http://", Qt::CaseInsensitive)) {
                clipboard = QString("<a href=\"%1\">%1</a>").arg(clipboard);
            } else {
                clipboard = QString("<em>%1</em>").arg(clipboard);
            }
            txt += i18n("<br><br>Clipboard content:<br>%1<br>", clipboard);
        }

        // Setup "uploadok" notification
        n_eventid = "uploadok";
        n_txt = txt;
        n_pixmap = m_lastGeneratedPixmap;
    }

    // Launch notification
    KNotification *notification = new KNotification(n_eventid, this);
    notification->setTitle("Kaption");
    notification->setPixmap(n_pixmap);
    notification->setText(n_txt);
    notification->sendEvent();

    m_screenSaved = true;

    // House cleaning
    m_currentJob->deleteLater();
    m_tmpFile->deleteLater();
}

void SnapshotPreview::slotCancelUpload()
{
    if (m_currentJob != 0 && m_currentJob->percent() < 100) {
        m_currentJob->kill(KJob::EmitResult);
    }
}

bool SnapshotPreview::saveImage(QIODevice *device, const QByteArray &format)
{
    QImageWriter imgWriter(device, format);

    if (!imgWriter.canWrite()) {
        m_lastError = imgWriter.errorString();
        return false;
    }

    if (0 == qstricmp(format.constData(), "jpeg") || 0 == qstricmp(format.constData(), "jpg")) {
        imgWriter.setQuality(100);
    }

    QPixmap p = m_lastGeneratedPixmap = getFinalPixmap();
    bool ok = imgWriter.write(p.toImage());
    if (!ok) {
        m_lastError = imgWriter.errorString();
    }
    return ok;
}

QPixmap SnapshotPreview::getFinalPixmap() const
{
    ui->snapshotCanvas->deselectItems();
    QGraphicsScene *scene = ui->snapshotCanvas->scene();
    QPixmap p(scene->width(), scene->height());
    QPainter painter(&p);
    painter.setRenderHint(QPainter::Antialiasing);
    scene->render(&painter);
    return p;
}

void SnapshotPreview::showEvent(QShowEvent *e)
{
    QFrame::showEvent(e);
}

void SnapshotPreview::resizeEvent(QResizeEvent *e)
{
    setToolbarsMaxSize();
    QFrame::resizeEvent(e);
}

void SnapshotPreview::closeEvent(QCloseEvent *e)
{
    if (saveAndContinueAction(EditorClosing)) {
        e->accept();
    } else {
        e->ignore();
    }
}

void SnapshotPreview::setToolbarsMaxSize()
{
    int maxHeight = ui->leftToolbar->layout()->minimumSize().height();
    if (ui->rightToolbar->layout()->minimumSize().height() > maxHeight) {
        maxHeight = ui->rightToolbar->layout()->minimumSize().height();
    }
    if (ui->centralLayout->maximumSize().height() > maxHeight) {
        maxHeight = ui->centralLayout->maximumSize().height();
    }
    ui->leftToolbar->setMaximumHeight(maxHeight);
    ui->rightToolbar->setMaximumHeight(maxHeight);

    int maxWidth = ui->bottomToolbar->layout()->minimumSize().width();
    if (ui->snapshotCanvas->maximumWidth() > maxWidth) {
        maxWidth = ui->snapshotCanvas->maximumWidth();
    }
    ui->bottomToolbar->setMaximumWidth(maxWidth);

    layout()->invalidate();
}

QString SnapshotPreview::filenameFromLineEdit()
{
    QString filename = ui->filenameLineEdit->text();
    if (filename.isEmpty()) {
        filename = generateScreenFilename();
        ui->filenameLineEdit->setText(filename);
    }
    //Search for / \ ? % * : | " < > .
    QRegExp regexp("/|\\\\|\\?|%|\\*|:|\\||\"|<|>|\\.");
    if (filename.contains(regexp)) {
        filename.replace(regexp, "_");
        ui->filenameLineEdit->setText(filename);
    }
    return filename + "." + ui->formatCmbBox->currentText();
}

QByteArray SnapshotPreview::imageFormatFromComboBox() const
{
    return ui->formatCmbBox->currentText().toUpper().toUtf8();
}

QString SnapshotPreview::generateScreenFilename() const
{
    return "screen_" + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
}

bool SnapshotPreview::saveAndContinueAction(SaveAndContinueReason reason)
{
    bool continueAction = true;

    if (!m_screenSaved && m_pixmapSet) {
        const QString title = i18n("Save image");
        const QString text = i18n("The current image is unsaved,"
                                  " do you want to save or discard it?");

        int ret = KMessageBox::No;
        if (reason == EditorClosing) {
            ret = KMessageBox::warningYesNoCancel(this, text, title,
                                                       KStandardGuiItem::save(),
                                                       KStandardGuiItem::discard(),
                                                       KStandardGuiItem::cancel(),
                                                       "",
                                                       KMessageBox::Dangerous);
        } else if (reason == PixmapChanging) {
            ret = KMessageBox::warningYesNo(this, text, title,
                                                KStandardGuiItem::save(),
                                                KStandardGuiItem::discard(),
                                                "",
                                                KMessageBox::Dangerous);
        }
        if (ret == KMessageBox::Cancel) {
            continueAction = false;
        } else if (ret == KMessageBox::Yes) {
            slotSaveAs();
        }
    }

    return continueAction;
}
