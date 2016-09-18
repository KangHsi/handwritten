#ifndef TOOLPALETTE_H
#define TOOLPALETTE_H

#include <QWidget>
#include <QColor>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class ToolPalette : public QTreeWidget
{
    Q_OBJECT
public:
    ToolPalette(QWidget *parent = 0);
    ~ToolPalette();

private:
    QColor mBgColor;
};

#endif // TOOLPALETTE_H
