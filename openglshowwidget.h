#ifndef OPENGLSHOWWIDGET_H
#define OPENGLSHOWWIDGET_H

#include <QWidget>

namespace Ui {
class OpenGLShowWidget;
}

class OpenGLShowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OpenGLShowWidget(QWidget *parent = nullptr);
    ~OpenGLShowWidget();

private:
    Ui::OpenGLShowWidget *ui;
};

#endif // OPENGLSHOWWIDGET_H
