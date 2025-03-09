#include "showview.h"
#include<QPainter>
#include<iostream>
ShowView::ShowView(QWidget *parent)
    : QWidget{parent}
{
    setAttribute(Qt::WA_TranslucentBackground);
}

void ShowView::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
//    QImage alphaMask(img.size(), QImage::Format_Alpha8);
//    alphaMask.fill(0);
//    for(int i=0;i<img.width();i++)
//    {
//        for(int j=0;j<img.height();j++)
//        {
//            if(img.pixelColor(i,j).redF()>0 || img.pixelColor(i,j).green()>0 || img.pixelColor(i,j).blue()>0)
//                alphaMask.setPixelColor(i,j,1);
//        }
//    }
//    img.setAlphaChannel(alphaMask);
    painter.drawImage(0,0,img);

}

void ShowView::slot_recImage(QImage image)
{
    img = image;
    update();
}
