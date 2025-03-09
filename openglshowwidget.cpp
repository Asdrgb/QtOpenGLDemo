#include "openglshowwidget.h"
#include "ui_openglshowwidget.h"

OpenGLShowWidget::OpenGLShowWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OpenGLShowWidget)
{
    setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);
    connect(ui->openGLWidget,&OpenglWidget::signal_push_image,ui->widget,&ShowView::slot_recImage);
}

OpenGLShowWidget::~OpenGLShowWidget()
{
    delete ui;
}
