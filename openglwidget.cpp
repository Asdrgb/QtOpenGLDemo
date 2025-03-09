#include "openglwidget.h"
#include <QDebug>
#include<qmath.h>

#define TIMEOUTMSEC 100

OpenglWidget::OpenglWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
//    setAutoFillBackground(true);
//    setAttribute(Qt::WA_TranslucentBackground);
//    setAttribute(Qt::WA_AlwaysStackOnTop);
    m_xRotate = -40.0;
    m_zRotate = 90.0;
    m_xTrans = 0.0;
    m_yTrans = 0.0;
    m_zoom = 40.0;

    // 相机位置
    cameraPos = QVector3D(0.0, 0.0, 50.0);
    // 相机方向
    cameraTarget = QVector3D(0.0, 0.0, 0.0);
    cameraDirection = QVector3D(cameraPos - cameraTarget);
    cameraDirection.normalize();

    // Right axis
    up = QVector3D(0.0, 1.0, 0.0);
    cameraRight = QVector3D::crossProduct(up, cameraDirection);
    cameraRight.normalize();
    // Up axis
    cameraUp = QVector3D::crossProduct(cameraDirection, cameraPos);

    cameraFront = QVector3D(0.0, 0.0, -1.0);

//    timer = new QTimer();
//    connect(timer,SIGNAL(timeout()),this,SLOT(update_data()));
//    timer->start(500);

}

OpenglWidget::~OpenglWidget()
{
    makeCurrent();
    glDeleteBuffers(1, &m_VBO_MeshLine);
    glDeleteVertexArrays(1, &m_VAO_MeshLine);

    glDeleteBuffers(1, &m_VBO_Axis);
    glDeleteVertexArrays(1, &m_VAO_Axis);

    glDeleteBuffers(1, &m_VBO_Point);
    glDeleteVertexArrays(1, &m_VAO_Point);

    m_shaderProgramMesh.release();
    m_shaderProgramAxis.release();
    m_shaderProgramPoint.release();

    if(m_model){delete m_model;m_model = nullptr;}

    doneCurrent();
    qDebug() << __FUNCTION__;
}

#if 0
void OpenglWidget::paintEvent(QPaintEvent *event)
{
    makeCurrent();
    paintGL();
    QImage image(width(), height(), QImage::Format_RGB32);
    QPainter painter(&image);
    painter.beginNativePainting();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
       为了将坐标从一个坐标系转换到另一个坐标系，需要用到几个转换矩阵，
       分别是模型(Model)、视图(View)、投影(Projection)三个矩阵。
    */
    QMatrix4x4 projection, view, model, obj;
    //透视矩阵变换
    projection.perspective(m_zoom, (float)width() / (float)height(), 1.0f, 200.0f);

    // eye：摄像机位置  center：摄像机看的点位 up：摄像机上方的朝向
//    view.lookAt(QVector3D(0.0, 0.0, 50.0), QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 1.0, 0.0));
    // 向前看
    view.lookAt(cameraPos, cameraPos + cameraFront, up);

//    model.translate(m_xTrans, m_yTrans, 0.0);
    model.rotate(m_xRotate, 1.0, 0.0, 0.0);
    model.rotate(m_zRotate, 0.0, 0.0, 1.0);

//    obj.translate(m_xTrans, m_yTrans, 0.0);
    obj.rotate(m_xRotate, 1.0, 0.0, 0.0);
    obj.rotate(m_zRotate, 0.0, 0.0, 1.0);
    obj.scale(0.011);



    QMatrix4x4 other_truck;
//    other_truck.translate(m_xTrans, m_yTrans, 0.0);
    other_truck.rotate(m_xRotate, 1.0, 0.0, 0.0);
    other_truck.rotate(m_zRotate, 0.0, 0.0, 1.0);
    other_truck.scale(0.011);



    m_shaderProgramMesh.bind();
    m_shaderProgramMesh.setUniformValue("projection", projection);
    m_shaderProgramMesh.setUniformValue("view", view);
    m_shaderProgramMesh.setUniformValue("model", model);

    m_shaderProgramAxis.bind();
    m_shaderProgramAxis.setUniformValue("projection", projection);
    m_shaderProgramAxis.setUniformValue("view", view);
    m_shaderProgramAxis.setUniformValue("model", model);

    //画网格
    m_shaderProgramMesh.bind();
    glBindVertexArray(m_VAO_MeshLine);
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, m_vertexCount);

    //画坐标轴
    m_shaderProgramAxis.bind();
    glBindVertexArray(m_VAO_Axis);
    glLineWidth(50.0f);
    glDrawArrays(GL_LINES, 0, 6);
    painter.endNativePainting();
    emit signal_push_image(image);
}
#endif
void OpenglWidget::updatePoints(const QVector<QVector3D> &points)
{
    m_pointData.clear();
    for(auto vector3D : points)
    {
        m_pointData.push_back(vector3D.x());
        m_pointData.push_back(vector3D.y());
        m_pointData.push_back(vector3D.z());
        m_pointData.push_back(1);
    }
}

void OpenglWidget::loadCsvFile(const QString &path)
{
    m_pointData.clear();
    QFile inFile(path);
    if (inFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream_text(&inFile);
        while (!stream_text.atEnd())
        {
            QString line = stream_text.readLine();
            QStringList strSplit = line.split(",");

            double x = strSplit.value(0).toDouble();
            double y = strSplit.value(1).toDouble();
            double z = strSplit.value(2).toDouble();
            m_pointData.push_back(x);
            m_pointData.push_back(y);
            m_pointData.push_back(z);
            m_pointData.push_back(1);
        }
        inFile.close();
    }
}

bool OpenglWidget::loadModel(QString fileName, QVector<float>& vPoints)
{
    if (fileName.mid(fileName.lastIndexOf('.')) != ".obj" && fileName.mid(fileName.lastIndexOf('.')) != ".OBJ")
    {
        qDebug() << "file is not a obj file.";
        return false;
    }

    QFile objFile(fileName);
    if (!objFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "open" << fileName << "failed";
        return false;
    }
    else
    {
        qDebug() << "open" << fileName << "success!";
    }

    QVector<float> vertextPoints, texturePoints, normalPoints;
    QVector<int> facesIndexs,textureIndexs,normalIndexs;
    while (!objFile.atEnd())
    {
        QByteArray lineData = objFile.readLine();

        QList<QByteArray> strValues = lineData.trimmed().split(' ');
        QString dataType = strValues.takeFirst();
        if (dataType == "v")
        {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(vertextPoints), [](QByteArray& str) {
                return str.toFloat();
            });
        }
        else if (dataType == "vt")
        {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(texturePoints), [](QByteArray& str) {
                return str.toFloat();
            });
        }
        else if (dataType == "vn")
        {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(normalPoints), [](QByteArray& str) {
                return str.toFloat();
            });
        }
        else if (dataType == "f")
        {
            auto v1 = strValues.at(0).trimmed().split('/');
            auto v2 = strValues.at(1).trimmed().split('/');
            auto v3 = strValues.at(2).trimmed().split('/');
            facesIndexs << v1.at(0).toInt()-1 << v2.at(0).toInt()-1 << v3.at(0).toInt()-1;
            textureIndexs << v1.at(1).toInt()-1 << v2.at(1).toInt()-1 << v3.at(1).toInt()-1;
            normalIndexs << v1.at(2).toInt()-1 << v2.at(2).toInt()-1 << v3.at(2).toInt()-1;
        }
    }
    objFile.close();

    for (int i=0;i<facesIndexs.size();i++)
    {
        int vIndex = facesIndexs.at(i);
        int nIndex = normalIndexs.at(i);
        int tIndex = textureIndexs.at(i);

        vPoints << vertextPoints.at(vIndex * 3);
        vPoints << vertextPoints.at(vIndex * 3 + 1);
        vPoints << vertextPoints.at(vIndex * 3 + 2);
        vPoints << normalPoints.at(nIndex * 3);
        vPoints << normalPoints.at(nIndex * 3 + 1);
        vPoints << normalPoints.at(nIndex * 3 + 2);
        vPoints << texturePoints.at(tIndex * 2);
        vPoints << texturePoints.at(tIndex * 2 + 1);
    }

    vertextPoints.clear();
    texturePoints.clear();
    normalPoints.clear();
    facesIndexs.clear();

    return true;
}

bool OpenglWidget::loadModelWithFacesArrary(QString fileName, QVector<float>& vPoints,QVector<int>& facesArray)
{
    if (fileName.mid(fileName.lastIndexOf('.')) != ".obj" && fileName.mid(fileName.lastIndexOf('.')) != ".OBJ")
    {
        qDebug() << "file is not a obj file.";
        return false;
    }

    QFile objFile(fileName);
    if (!objFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "open" << fileName << "failed";
        return false;
    }
    else
    {
        qDebug() << "open" << fileName << "success!";
    }

    QVector<float> vertextPoints, texturePoints, normalPoints;
    QVector<int> facesIndexs,textureIndexs,normalIndexs;
    while (!objFile.atEnd())
    {
        QByteArray lineData = objFile.readLine();

        QList<QByteArray> strValues = lineData.trimmed().split(' ');
        QString dataType = strValues.takeFirst();
        if (dataType == "v")
        {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(vertextPoints), [](QByteArray& str) {
                return str.toFloat();
            });
        }
        else if (dataType == "vt")
        {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(texturePoints), [](QByteArray& str) {
                return str.toFloat();
            });
        }
        else if (dataType == "vn")
        {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(normalPoints), [](QByteArray& str) {
                return str.toFloat();
            });
        }
        else if (dataType == "f")
        {
            auto v1 = strValues.at(0).trimmed().split('/');
            auto v2 = strValues.at(1).trimmed().split('/');
            auto v3 = strValues.at(2).trimmed().split('/');
            facesIndexs << v1.at(0).toInt()-1 << v2.at(0).toInt()-1 << v3.at(0).toInt()-1;
            textureIndexs << v1.at(1).toInt()-1 << v2.at(1).toInt()-1 << v3.at(1).toInt()-1;
            normalIndexs << v1.at(2).toInt()-1 << v2.at(2).toInt()-1 << v3.at(2).toInt()-1;
        }
    }
    objFile.close();

    QVector<float> points(vertextPoints.size()/3*8);
    QVector<bool> flag(vertextPoints.size()/3);
    for (int i=0;i<facesIndexs.size();i++)
    {
        int vIndex = facesIndexs.at(i);
        if(flag[vIndex] == true)continue;
        int nIndex = normalIndexs.at(i);
        int tIndex = textureIndexs.at(i);


        points[vIndex*8] = vertextPoints.at(vIndex * 3);
        points[vIndex*8+1] =vertextPoints.at(vIndex * 3 + 1);
        points[vIndex*8+2] = vertextPoints.at(vIndex * 3 + 2);
        points[vIndex*8+3] = normalPoints.at(nIndex * 3);
        points[vIndex*8+4] = normalPoints.at(nIndex * 3 + 1);
        points[vIndex*8+5] = normalPoints.at(nIndex * 3 + 2);
        points[vIndex*8+6] = texturePoints.at(tIndex * 2);
        points[vIndex*8+7] = texturePoints.at(tIndex * 2 + 1);

        flag[vIndex] = true;
    }
    vertextPoints.clear();
    texturePoints.clear();
    normalPoints.clear();
    facesArray = facesIndexs;
    vPoints = points;
    return true;
}

void OpenglWidget::initializeGL()
{
    initializeOpenGLFunctions();
    core = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

    // enable depth_test
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_ALPHA_TEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // link meshline shaders   vs文件为顶点着色器  fs为片段着色器
    m_shaderProgramMesh.addShaderFromSourceFile(QOpenGLShader::Vertex,
            ":/opengl/shader/shader_mesh.vs");
    m_shaderProgramMesh.addShaderFromSourceFile(QOpenGLShader::Fragment,
            ":/opengl/shader/shader_mesh.fs");
    m_shaderProgramMesh.link();

    // link coordinate axis shaders
    m_shaderProgramAxis.addShaderFromSourceFile(QOpenGLShader::Vertex,
            ":/opengl/shader/shader_axis.vs");
    m_shaderProgramAxis.addShaderFromSourceFile(QOpenGLShader::Fragment,
            ":/opengl/shader/shader_axis.fs");
    m_shaderProgramAxis.link();

    // link pointcloud shaders
    m_shaderProgramPoint.addShaderFromSourceFile(QOpenGLShader::Vertex,
            ":/opengl/shader/shader_obj.vs");
    m_shaderProgramPoint.addShaderFromSourceFile(QOpenGLShader::Fragment,
            ":/opengl/shader/shader_obj.fs");
    m_shaderProgramPoint.link();

    m_shaderProgramOtherTruck.addShaderFromSourceFile(QOpenGLShader::Vertex,
            ":/opengl/shader/shader_truck.vs");
    m_shaderProgramOtherTruck.addShaderFromSourceFile(QOpenGLShader::Fragment,
            ":/opengl/shader/shader_truck.fs");
    m_shaderProgramOtherTruck.link();

    m_shaderProgramGaoda.addShaderFromSourceFile(QOpenGLShader::Vertex,
            ":/opengl/shader/shader_truck.vs");
    m_shaderProgramGaoda.addShaderFromSourceFile(QOpenGLShader::Fragment,
            ":/opengl/shader/shader_truck.fs");
    m_shaderProgramGaoda.link();



    m_vertexCount = drawMeshline(2.0, 16);
    m_pointCount = drawPointdata(m_pointData);
    qDebug() << "point_count" << m_pointCount;
    drawCooraxis(4.0);
}

#if 1
void OpenglWidget::paintGL()
{
//    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
//    glClear(GL_COLOR_BUFFER_BIT);
//    glClear(GL_DEPTH_BUFFER_BIT);

    /*
       为了将坐标从一个坐标系转换到另一个坐标系，需要用到几个转换矩阵，
       分别是模型(Model)、视图(View)、投影(Projection)三个矩阵。
    */
    QMatrix4x4 projection, view, model, obj;
    //透视矩阵变换
    projection.perspective(m_zoom, (float)width() / (float)height(), 1.0f, 200.0f);

    // eye：摄像机位置  center：摄像机看的点位 up：摄像机上方的朝向
//    view.lookAt(QVector3D(0.0, 0.0, 50.0), QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 1.0, 0.0));
    // 向前看
    view.lookAt(cameraPos, cameraPos + cameraFront, up);


    model.rotate(m_xRotate, 1.0, 0.0, 0.0);
    model.rotate(m_zRotate, 0.0, 0.0, 1.0);
    model.translate(10, 0, 0.0);

//    obj.translate(m_xTrans, m_yTrans, 0.0);
    obj.rotate(m_xRotate, 1.0, 0.0, 0.0);
    obj.rotate(m_zRotate, 0.0, 0.0, 1.0);
    obj.scale(0.011);



    QMatrix4x4 other_truck;
//    other_truck.translate(m_xTrans, m_yTrans, 0.0);
    other_truck.rotate(m_xRotate, 1.0, 0.0, 0.0);
    other_truck.rotate(m_zRotate, 0.0, 0.0, 1.0);
    other_truck.scale(0.011);



    m_shaderProgramMesh.bind();
    m_shaderProgramMesh.setUniformValue("projection", projection);
    m_shaderProgramMesh.setUniformValue("view", view);
    m_shaderProgramMesh.setUniformValue("model", model);

    m_shaderProgramAxis.bind();
    m_shaderProgramAxis.setUniformValue("projection", projection);
    m_shaderProgramAxis.setUniformValue("view", view);
    m_shaderProgramAxis.setUniformValue("model", model);

#if 1
    m_shaderProgramPoint.bind();
    m_shaderProgramPoint.setUniformValue("projection", projection);
    m_shaderProgramPoint.setUniformValue("view", view);
    m_shaderProgramPoint.setUniformValue("model", obj);
    QVector3D objectColor(0.8f, 0.8f, 0.8f);
    QVector3D lightColor( 1.0f, 1.0f, 1.0f);
    QVector3D lightPos = model*QVector3D(0,0,6);
    m_shaderProgramPoint.setUniformValue("objectColor", objectColor);
    m_shaderProgramPoint.setUniformValue("lightColor", lightColor);
    m_shaderProgramPoint.setUniformValue("lightPos", lightPos);

    m_shaderProgramOtherTruck.bind();
    m_shaderProgramOtherTruck.setUniformValue("projection", projection);
    m_shaderProgramOtherTruck.setUniformValue("view", view);
    m_shaderProgramOtherTruck.setUniformValue("model", other_truck);
    QVector3D tobjectColor(1.0f, 1.0f, 1.0f);
    QVector3D tlightColor( 1.0f, 1.0f, 1.0f);
//    QVector3D tlightPos = model*QVector3D(4,0,10);
    m_shaderProgramOtherTruck.setUniformValue("objectColor", tobjectColor);
    m_shaderProgramOtherTruck.setUniformValue("lightColor", tlightColor);
    m_shaderProgramOtherTruck.setUniformValue("lightPos", lightPos);

    m_shaderProgramGaoda.bind();
    m_shaderProgramGaoda.setUniformValue("projection", projection);
    m_shaderProgramGaoda.setUniformValue("view", view);
    m_shaderProgramGaoda.setUniformValue("model", other_truck);
    m_shaderProgramGaoda.setUniformValue("objectColor", tobjectColor);
    m_shaderProgramGaoda.setUniformValue("lightColor", tlightColor);
    m_shaderProgramGaoda.setUniformValue("lightPos", lightPos);

    m_model->Draw(m_shaderProgramPoint);

#endif
    //画网格
    m_shaderProgramMesh.bind();
    glBindVertexArray(m_VAO_MeshLine);
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, m_vertexCount);

    //画坐标轴
//    m_shaderProgramAxis.bind();
//    glBindVertexArray(m_VAO_Axis);
//    glLineWidth(50.0f);
//    glDrawArrays(GL_LINES, 0, 6);


    //画点云
//    m_shaderProgramPoint.bind();
//    glBindVertexArray(m_VAO_Point);
////    glDrawArrays(GL_TRIANGLES, 0, m_pointCount);
//    glDrawElements(GL_TRIANGLES, m_pointIndexs.size(), GL_UNSIGNED_INT, 0);


//    m_shaderProgramOtherTruck.bind();
//    glBindVertexArray(m_VAO_OtherTruck);
//    glDrawArraysInstanced(GL_TRIANGLES, 0, m_pointCount, m_otherTruckCoor.size()/4);
//    glDrawElementsInstanced(GL_TRIANGLES, m_pointIndexs.size(), GL_UNSIGNED_INT, 0, m_otherTruckCoor.size()/4);

    QImage image(width(), height(), QImage::Format_RGB32);
    image.fill(Qt::transparent);
    glReadPixels(0, 0, width(), height(), GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    image = image.mirrored();
    image.save("render.png");
    emit signal_push_image(image);
}

#endif

void OpenglWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void OpenglWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void OpenglWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->pos().x() - lastPos.x();
    int dy = event->pos().y() - lastPos.y();
    if (event->buttons() & Qt::LeftButton)
    {
        m_xRotate = m_xRotate + 0.3 * dy;
        m_zRotate = m_zRotate + 0.3 * dx;

        if (m_xRotate > 30.0f)
        {
            m_xRotate = 30.0f;
        }
        if (m_xRotate < -120.0f)
        {
            m_xRotate = -120.0f;
        }
        update();
    }
    else if (event->buttons() & Qt::MidButton)
    {
        m_xTrans = m_xTrans + 0.1 * dx;
        m_yTrans = m_yTrans - 0.1 * dy;
        update();
    }
    lastPos = event->pos();
}

void OpenglWidget::wheelEvent(QWheelEvent *event)
{
    auto scroll_offest = event->angleDelta().y() / 120;
    m_zoom = m_zoom - (float)scroll_offest;

    if (m_zoom < 1.0f)    /* 放大限制 */
    {
        m_zoom = 1.0f;
    }

    if (m_zoom > 80.0f)
    {
        m_zoom = 80.0f;
    }

    update();
}

void OpenglWidget::keyPressEvent(QKeyEvent *event)
{

    float cameraSpeed = 2.5 * TIMEOUTMSEC/1000.0;
    switch(event->key()){
        case Qt::Key_W: cameraPos += cameraSpeed * cameraFront; break;
        case Qt::Key_S: cameraPos -= cameraSpeed * cameraFront; break;
        case Qt::Key_D: cameraPos += cameraSpeed * cameraRight; break;
        case Qt::Key_A: cameraPos -= cameraSpeed * cameraRight; break;
        case Qt::Key_Space:
            m_xRotate = -40.0;
            m_zRotate = 90.0;
            m_xTrans = 0.0;
            m_yTrans = 0.0;
            m_zoom = 40.0;
            cameraPos = QVector3D(0.0, 0.0, 50.0);
            break;
        default:
            break;
    }
    qDebug()<<cameraPos.x()<<" "<<cameraPos.y()<<" "<<cameraPos.z();

    // 调用绘制函数
    update();
}

unsigned int OpenglWidget::drawMeshline(float size, int count)
{
    std::vector<float> mesh_vertexs;
    unsigned int vertex_count = 0;

    float start = count * (size / 2);
    float posX = start, posZ = start;

    for (int i = 0; i <= count; ++i)
    {
        mesh_vertexs.push_back(posX);
        mesh_vertexs.push_back(start);
        mesh_vertexs.push_back(0);

        mesh_vertexs.push_back(posX);
        mesh_vertexs.push_back(-start);
        mesh_vertexs.push_back(0);

        mesh_vertexs.push_back(start);
        mesh_vertexs.push_back(posZ);
        mesh_vertexs.push_back(0);

        mesh_vertexs.push_back(-start);
        mesh_vertexs.push_back(posZ);
        mesh_vertexs.push_back(0);

        posX = posX - size;
        posZ = posZ - size;
    }

    glGenVertexArrays(1, &m_VAO_MeshLine);
    glGenBuffers(1, &m_VBO_MeshLine);

    glBindVertexArray(m_VAO_MeshLine);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_MeshLine);

    glBufferData(GL_ARRAY_BUFFER, mesh_vertexs.size() * sizeof(float), &mesh_vertexs[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    vertex_count = (int)mesh_vertexs.size() / 3;

    return vertex_count;
}

void OpenglWidget::drawCooraxis(float length)
{
    std::vector<float> axis_vertexs =
    {
        //x,y ,z ,r, g, b
        0.0, 0.0, 0.0, 1.0, 0.0, 0.0,     // red x
        length, 0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 1.0, 0.0,      // green y
        0.0, length, 0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 1.0,      // blue z
        0.0, 0.0, length, 0.0, 0.0, 1.0,
    };

    glGenVertexArrays(1, &m_VAO_Axis);
    glGenBuffers(1, &m_VBO_Axis);

    glBindVertexArray(m_VAO_Axis);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_Axis);
    glBufferData(GL_ARRAY_BUFFER, axis_vertexs.size() * sizeof(float), &axis_vertexs[0], GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}


unsigned int OpenglWidget::drawPointdata(QVector<float> &pointVertexs)
{
    unsigned int point_count = 0;

    glGenVertexArrays(1, &m_VAO_Point);
    glGenBuffers(1, &m_VBO_Point);
    glGenBuffers(1,&m_EBO_Point);

    glBindVertexArray(m_VAO_Point);

//    loadModel(":/opengl/shader/truck.obj",pointVertexs);
    loadModelWithFacesArrary("./truck.obj",pointVertexs,m_pointIndexs);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_Point);
    glBufferData(GL_ARRAY_BUFFER, pointVertexs.size() * sizeof(float), &pointVertexs[0], GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_Point);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_pointIndexs.size() * sizeof(int), &m_pointIndexs[0], GL_STATIC_DRAW);

    QVector3D p(1.0f,0,0);
    p.normalize();
    m_otherTruckCoor.push_back(1.0f);
    m_otherTruckCoor.push_back(0);
    m_otherTruckCoor.push_back(0);
    QVector3D p1(-1.0f,1.0f,0.0f);
    p1.normalize();
    auto cos1 = qAcos(QVector3D::dotProduct(p1,p)/p1.length());
    m_otherTruckCoor.push_back(p1.y() > 0 ? -1 * cos1:cos1);

    m_otherTruckCoor.push_back(0.0f);
    m_otherTruckCoor.push_back(1.0f);
    m_otherTruckCoor.push_back(0);
    QVector3D p2(1.0f,0.0f,0.0f);
    p2.normalize();
    auto cos2 = qAcos(QVector3D::dotProduct(p2,p)/p2.length());
    m_otherTruckCoor.push_back(p2.y() > 0 ? -1 * cos2:cos2);

    glGenVertexArrays(1, &m_VAO_OtherTruck);
    glGenBuffers(1, &m_VBO_OtherTruck);
    glBindVertexArray(m_VAO_OtherTruck);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_Point);
    glBufferData(GL_ARRAY_BUFFER, pointVertexs.size() * sizeof(float), &pointVertexs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_OtherTruck);
    glBufferData(GL_ARRAY_BUFFER, m_otherTruckCoor.size() * sizeof(float), &m_otherTruckCoor[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_Point);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_pointIndexs.size() * sizeof(int), &m_pointIndexs[0], GL_STATIC_DRAW);

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    point_count = (unsigned int)pointVertexs.size() / 3;

    loadModelWithAssimp();

    return point_count;
}

void OpenglWidget::loadModelWithAssimp()
{
    m_model = new Model(core,"truck.obj");
}

void OpenglWidget::update_data()
{

    static float count = 0;
    if(count >= 1000)count = 0;
    m_otherTruckCoor.clear();
    float y = qCos(count*0.1);
    float x = qSin(count*0.1);
    m_otherTruckCoor.push_back(x);
    m_otherTruckCoor.push_back(y);
    m_otherTruckCoor.push_back(0);
    m_otherTruckCoor.push_back(0);

    m_otherTruckCoor.push_back(-x);
    m_otherTruckCoor.push_back(-y);
    m_otherTruckCoor.push_back(0);
    m_otherTruckCoor.push_back(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO_OtherTruck);
    glBufferData(GL_ARRAY_BUFFER, m_otherTruckCoor.size() * sizeof(float), &m_otherTruckCoor[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(3 * sizeof(float)));
    count++;
    update();
}
