#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include<assimp/Importer.hpp>
#include"model.h"
#include"mesh.h"
#include<QTimer>
#include<QImage>


class OpenglWidget: public QOpenGLWidget, public QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    OpenglWidget(QWidget *parent = 0);
    ~OpenglWidget();
//    void paintEvent(QPaintEvent *event);
    void updatePoints(const QVector<QVector3D> &points);
    void loadCsvFile(const QString &path);

    bool loadModel(QString fileName, QVector<float> &vPoints);

    bool loadModelWithFacesArrary(QString fileName, QVector<float> &vPoints, QVector<int> &facesArray);

    QImage img;
signals:
    void signal_push_image(QImage image);
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event)override;

    virtual unsigned int drawMeshline(float size, int count);
    virtual void drawCooraxis(float length);
    virtual unsigned int drawPointdata(QVector<float> &pointVertexs);

    void loadModelWithAssimp();

private:
    QOpenGLFunctions_3_3_Core *core = nullptr;
    QOpenGLShaderProgram m_shaderProgramMesh;
    QOpenGLShaderProgram m_shaderProgramAxis;
    QOpenGLShaderProgram m_shaderProgramPoint;
    QOpenGLShaderProgram m_shaderProgramOtherTruck;
    QOpenGLShaderProgram m_shaderProgramGaoda;

    unsigned int m_VBO_MeshLine;
    unsigned int m_VAO_MeshLine;

    unsigned int m_VBO_Axis;
    unsigned int m_VAO_Axis;

    unsigned int m_VBO_Point;
    unsigned int m_VAO_Point;
    unsigned int m_EBO_Point;

    unsigned int m_VBO_OtherTruck;
    unsigned int m_VAO_OtherTruck;
    QVector<float> m_otherTruckCoor;

    QVector<float> m_pointData;
    QVector<int> m_pointIndexs;
    unsigned int m_pointCount;

    unsigned int m_vertexCount;

    float m_xRotate;
    float m_zRotate;
    float m_xTrans;
    float m_yTrans;
    float m_zoom;

    QVector3D cameraPos;
    QVector3D cameraTarget;
    QVector3D cameraDirection;
    QVector3D up;
    QVector3D cameraRight;
    QVector3D cameraUp;
    QVector3D cameraFront;
    QPoint   lastPos;

    Model *m_model;
    QTimer *timer;
    QVector<float> m_position;
public slots:
    void update_data();

};


#endif // OPENGLWIDGET_H
