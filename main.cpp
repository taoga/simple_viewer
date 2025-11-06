#include "dlgview.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Temporarily: Open an existing file
    DlgView w("../../dem_10_30_44.bin");
    //DlgView w;

    w.show();
    return a.exec();
}
