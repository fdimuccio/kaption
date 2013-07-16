#include "uploadprogressdialog.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

UploadProgressDialog::UploadProgressDialog(QWidget *parent, const QString &caption,
                                           const QString &text, Qt::WFlags flags) :
    KProgressDialog(parent, caption, text, flags), m_logIconLabel(new QLabel(mainWidget())),
    m_logTextLabel(new QLabel(mainWidget()))
{
    setModal(true);
    setAutoClose(false);
    setAutoReset(false);
    progressBar()->setMinimum(0);
    progressBar()->setMaximum(100);

    QHBoxLayout *logLayout = new QHBoxLayout(mainWidget());
    logLayout->addWidget(m_logIconLabel);
    logLayout->addWidget(m_logTextLabel, 1);

    QVBoxLayout *layout = dynamic_cast<QVBoxLayout*>(mainWidget()->layout());
    if (layout) {
        layout->addLayout(logLayout);
    }
}

void UploadProgressDialog::clearLogInfo()
{
    m_logIconLabel->clear();
    m_logTextLabel->clear();
}

void UploadProgressDialog::setLogInfo(const QIcon &icon, const QString &text)
{
    m_logIconLabel->setPixmap(icon.pixmap(16, 16));
    m_logTextLabel->setText("<i>"+text+"</i>");
}
