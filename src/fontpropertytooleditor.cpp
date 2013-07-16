#include "fontpropertytooleditor.h"

#include <QPushButton>
#include <KFontDialog>
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
    QFont font = value().value<QFont>();

    int result = KFontDialog::getFont(font);
    if (result == KFontDialog::Accepted) {
        changeValue(QVariant::fromValue(font));
    }
}
