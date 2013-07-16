#ifndef COLORPROPERTYTOOLEDITOR_H
#define COLORPROPERTYTOOLEDITOR_H

#include "propertytooleditor.h"
#include <KColorButton>

class KColorButton;

class ColorPropertyToolEditor : public PropertyToolEditor
{
    Q_OBJECT

public:
    ColorPropertyToolEditor(KColorButton *btn, QObject *parent = 0);
    virtual ~ColorPropertyToolEditor();

private Q_SLOTS:
    void changeColor(const QColor &color);
    void setColor(const QVariant &var);
};

#endif // COLORPROPERTYTOOLEDITOR_H
