#include "colorpropertytooleditor.h"

#include <KColorButton>
#include <QVariant>

ColorPropertyToolEditor::ColorPropertyToolEditor(KColorButton *btn, QObject *parent)
    : PropertyToolEditor(btn, parent)
{
    connect(btn, SIGNAL(changed(QColor)),
            this, SLOT(changeColor(QColor)));
    connect(this, SIGNAL(valueSet(QVariant)),
            this, SLOT(setColor(QVariant)));

    setValue(QVariant::fromValue(btn->color()));
}

ColorPropertyToolEditor::~ColorPropertyToolEditor()
{}

void ColorPropertyToolEditor::changeColor(const QColor &color)
{
    changeValue(QVariant(color));
}

void ColorPropertyToolEditor::setColor(const QVariant &var)
{
    KColorButton *btn = static_cast<KColorButton*>(widget());
    btn->blockSignals(true);
    btn->setColor(var.value<QColor>());
    btn->blockSignals(false);
}
