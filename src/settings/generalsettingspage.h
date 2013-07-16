#ifndef GENERALSETTINGSPAGE_H
#define GENERALSETTINGSPAGE_H

#include <QWidget>

namespace Ui {
    class GeneralSettingsPage;
}

class GeneralSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralSettingsPage(QWidget *parent = 0);
    ~GeneralSettingsPage();

private Q_SLOTS:
    void selectDefaultSaveLocationUrl();

private:
    Ui::GeneralSettingsPage *ui;
};

#endif // GENERALSETTINGSPAGE_H
