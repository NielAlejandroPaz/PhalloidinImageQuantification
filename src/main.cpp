#include "phalloidinimagequantification.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PhalloidinImageQuantification w;
    w.show();
    return a.exec();
}
