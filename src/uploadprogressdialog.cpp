#include "uploadprogressdialog.h"

#include <QPushButton>

#include <KLocalizedString>
#include <KStandardGuiItem>

UploadProgressDialog::UploadProgressDialog(QWidget *parent, const QString &caption,
                                           const QString &text) :
    QProgressDialog(caption, text, 0, 100, parent),
    m_button(new QPushButton)
{
    setModal(true);
    setAutoClose(false);
    setAutoReset(false);
    setCancelButton(m_button);
}

void UploadProgressDialog::setButton(KStandardGuiItem::StandardItem item)
{
    KStandardGuiItem::assign(m_button, item);
}
