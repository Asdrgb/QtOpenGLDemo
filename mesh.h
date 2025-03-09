#ifndef MESH_H
#define MESH_H

#include <QString>
#include <string>
#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    QVector3D Position;
    // normal
    QVector3D Normal;
    // texCoords
    QVector2D TexCoords;

    // tangent
    QVector3D Tangent;
    // bitangent
    QVector3D Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    // mesh Data
    QVector<Vertex>       vertices;
    QVector<unsigned int> indices;
    QVector<Texture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(Mesh&&) = default;
    Mesh(const QVector<Vertex> &vertices, const QVector<unsigned int> &indices,QOpenGLFunctions_3_3_Core* openglContext);
    Mesh(const QVector<Vertex> &vertices, const QVector<unsigned int> &indices, const QVector<Texture> &textures,QOpenGLFunctions_3_3_Core* openglContext);
    void Draw(QOpenGLShaderProgram &shader);
private:
    // render data
    QOpenGLFunctions_3_3_Core* m_openglCore;
    unsigned int VBO, EBO;
    void setupMesh();
};
#endif
