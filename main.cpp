
#include "openglwidget.h"
#include <QApplication>
#include<QSurfaceFormat>
#include "openglshowwidget.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    OpenglWidget window;
    OpenGLShowWidget view;
//    QSurfaceFormat fmt;
//    fmt.setVersion( 3, 3 );
//    fmt.setProfile( QSurfaceFormat::CoreProfile );
//    window.setFormat( fmt );
//    QSurfaceFormat::setDefaultFormat( fmt );
//    window.show();
    view.show();
    return a.exec();
}
