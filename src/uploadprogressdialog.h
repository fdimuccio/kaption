#ifndef UPLOADPROGRESSDIALOG_H
#define UPLOADPROGRESSDIALOG_H

#include <KProgressDialog>

class QLabel;

class UploadProgressDialog : public KProgressDialog
{
public:
    UploadProgressDialog(QWidget *parent = 0,
                         const QString &caption = QString(),
                         const QString &text = QString(),
                         Qt::WFlags flags = 0);

    void clearLogInfo();

    void setLogInfo(const QIcon &icon, const QString &text);

private:
    QLabel *m_logIconLabel;
    QLabel *m_logTextLabel;
};

#endif // UPLOADPROGRESSDIALOG_H
