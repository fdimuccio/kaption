#include "uploadprogressdialog.h"

UploadProgressDialog::UploadProgressDialog(QWidget *parent, const QString &caption,
                                           const QString &text) :
    QProgressDialog(caption, text, 0, 100, parent)
{
    setModal(true);
    setAutoClose(false);
    setAutoReset(false);
}

void UploadProgressDialog::clearLogInfo()
{
    setLabelText(QString());
}

void UploadProgressDialog::setLogInfo(const QIcon &icon, const QString &text)
{
    setLabelText(text);
}
