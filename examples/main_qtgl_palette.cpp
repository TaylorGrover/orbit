#include <paletteGL.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication paletteGLApp(argc, argv);

    PaletteGL glWindow;
    glWindow.show();

    return paletteGLApp.exec();
}
