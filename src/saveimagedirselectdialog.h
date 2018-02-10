#ifndef SAVEIMAGEDIRSELECTDIALOG_H
#define SAVEIMAGEDIRSELECTDIALOG_H

#include <KDirSelectDialog>

class QCheckBox;

class SaveImageDirSelectDialog : public KDirSelectDialog
{
    Q_OBJECT

public:
    SaveImageDirSelectDialog(const QUrl &startDir = QUrl(),
                             bool localOnly = false,
                             QWidget *parent = 0L);

    static QUrl selectDirectory(const QUrl &startDir = QUrl(),
                                bool localOnly = false,
                                QWidget *parent = 0L,
                                const QString &caption = QString());

    bool dontAskMeAgain() const;

private:
    QCheckBox *m_dontAskMeAgainCheckBox;
};

#endif // SAVEIMAGEDIRSELECTDIALOG_H
