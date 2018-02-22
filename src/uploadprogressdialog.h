#ifndef UPLOADPROGRESSDIALOG_H
#define UPLOADPROGRESSDIALOG_H

#include <QProgressDialog>

#include <KStandardGuiItem>

class UploadProgressDialog : public QProgressDialog
{
public:
    UploadProgressDialog(QWidget *parent = 0,
                         const QString &caption = QString(),
                         const QString &text = QString());

    void setButton(KStandardGuiItem::StandardItem item);
    void clearLogInfo();

    void setLogInfo(const QIcon &icon, const QString &text);

private:
    QPushButton *m_button;
};

#endif // UPLOADPROGRESSDIALOG_H
