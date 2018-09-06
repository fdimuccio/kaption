#include "fontpropertytooleditor.h"

#include <QPushButton>
#include <QFontDialog>
#include <QFont>

FontPropertyToolEditor::FontPropertyToolEditor(QPushButton *bttn,
                                               QObject *parent)
    : PropertyToolEditor(bttn, parent)
{
    connect(bttn, SIGNAL(clicked()),
            this, SLOT(changeFont()));

    QFont font;
    font.setBold(true);
    font.setPointSize(10);
    setValue(QVariant::fromValue(font));
}

void FontPropertyToolEditor::changeFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok);
    if (ok) {
        changeValue(QVariant::fromValue(font));
    }
}
