#include "panel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Panel panel;
    panel.show();
    panel.setFocus();
    return a.exec();
}
