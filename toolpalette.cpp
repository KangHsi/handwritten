#include "toolpalette.h"
#include "globalDefs.h"

ToolPalette::ToolPalette(QWidget *parent) :
    QTreeWidget(parent)
{
    setAlternatingRowColors(true);
    setRootIsDecorated(false);
    setColumnCount(2);
    setRootIsDecorated(false);
}

ToolPalette::~ToolPalette()
{

}
