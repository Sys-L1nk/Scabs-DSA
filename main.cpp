#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("Fusion"); // clean cross-platform look

    MainWindow w;
    w.show();

    return app.exec();
}
