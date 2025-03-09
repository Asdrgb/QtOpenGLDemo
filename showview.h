#ifndef SHOWVIEW_H
#define SHOWVIEW_H

#include <QWidget>
#include<QImage>

class ShowView : public QWidget
{
    Q_OBJECT
public:
    explicit ShowView(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);
    QImage img;

public slots:
    void slot_recImage(QImage image);
signals:

};

#endif // SHOWVIEW_H
