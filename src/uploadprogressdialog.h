#ifndef UPLOADPROGRESSDIALOG_H
#define UPLOADPROGRESSDIALOG_H

#include <QProgressDialog>

class UploadProgressDialog : public QProgressDialog
{
public:
    UploadProgressDialog(QWidget *parent = 0,
                         const QString &caption = QString(),
                         const QString &text = QString());

    void clearLogInfo();

    void setLogInfo(const QIcon &icon, const QString &text);
};

#endif // UPLOADPROGRESSDIALOG_H
