#include "mesh.h"

// constructor
Mesh::Mesh(const QVector<Vertex> &vertices,
           const QVector<unsigned int> &indices,
           QOpenGLFunctions_3_3_Core* openglContext)
{
    m_openglCore = openglContext;
    this->vertices = vertices;
    this->indices = indices;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}

// constructor
Mesh::Mesh(const QVector<Vertex> &vertices,
           const QVector<unsigned int> &indices,
           const QVector<Texture> &textures,
           QOpenGLFunctions_3_3_Core* openglContext)
{
    m_openglCore = openglContext;
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupMesh();
}


// render the mesh
void Mesh::Draw(QOpenGLShaderProgram &shader)
{
    // bind appropriate textures
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
#if 1
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        m_openglCore->glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if(name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
         else if(name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        m_openglCore->glUniform1i(m_openglCore->glGetUniformLocation(shader.programId(), (name + number).c_str()), i);
        // and finally bind the texture
        m_openglCore->glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
#endif
    // draw mesh
    shader.bind();
    m_openglCore->glBindVertexArray(VAO);
    m_openglCore->glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    m_openglCore->glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    m_openglCore->glActiveTexture(GL_TEXTURE0);
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh()
{
    // create buffers/arrays
    m_openglCore->glGenVertexArrays(1, &VAO);
    m_openglCore->glGenBuffers(1, &VBO);
    m_openglCore->glGenBuffers(1, &EBO);

    m_openglCore->glBindVertexArray(VAO);
    // load data into vertex buffers
    m_openglCore->glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a QQVector3D/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    m_openglCore->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    m_openglCore->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    m_openglCore->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    m_openglCore->glEnableVertexAttribArray(0);
    m_openglCore->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    m_openglCore->glEnableVertexAttribArray(1);
    m_openglCore->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    m_openglCore->glEnableVertexAttribArray(2);
    m_openglCore->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
#if 1
    // vertex tangent
    m_openglCore->glEnableVertexAttribArray(3);
    m_openglCore->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    m_openglCore->glEnableVertexAttribArray(4);
    m_openglCore->glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    m_openglCore->glEnableVertexAttribArray(5);
    m_openglCore->glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
    // weights
    m_openglCore->glEnableVertexAttribArray(6);
    m_openglCore->glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
#endif
    m_openglCore->glBindVertexArray(0);
}
