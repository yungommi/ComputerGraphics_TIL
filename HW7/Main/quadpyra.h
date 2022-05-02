
// Quadrangular Pyramid (사각뿔)
//
// 5 faces (front, right, back, left, and bottom)
// 4 faces are triangles (3 vertices): front, right, back, left
// bottom face is a rectangle (4 vertices)
//
// Vertex shader: the location (0: position attrib (vec3),
//                              1: color attrib (vec4))
//

#ifndef QUADPYRA_H
#define QUADPYRA_H

#include "shader.h"

class QuadPyra {
public:
    
    // vertex position array
    GLfloat quadVertices[48]  = { // (4 faces x 3 vertices x 3) + (4 vertices x 3) = 36 + 12 = 48
        .0f, .5f, .0f,   -.5f, -.5f,  .5f,    .5f, -.5f,  .5f,   // front
        .0f, .5f, .0f,    .5f, -.5f,  .5f,    .5f, -.5f, -.5f,   // right
        .0f, .5f, .0f,    .5f, -.5f, -.5f,   -.5f, -.5f, -.5f,   // back
        .0f, .5f, .0f,   -.5f, -.5f, -.5f,   -.5f, -.5f,  .5f,   // left
       -.5f,-.5f, .5f,   -.5f, -.5f, -.5f,    .5f, -.5f, -.5f,  .5f, -.5f, .5f // bottom
    };
    
    // color array
    GLfloat quadColors[64] = { // (4 faces x 3 vertices x 4) + (4 vertices x 4) = 48 + 16 = 64
        1, 0, 0, 1,  1, 0, 0, 1,  1, 0, 0, 1,    // front: red
        0, 1, 0, 1,  0, 1, 0, 1,  0, 1, 0, 1,    // right: green
        0, 0, 1, 1,  0, 0, 1, 1,  0, 0, 1, 1,    // back: blue
        1, 1, 0, 1,  1, 1, 0, 1,  1, 1, 0, 1,    // left: yellow
        1, 0, 1, 1,  1, 0, 1, 1,  1, 0, 1, 1,  1, 0, 1, 1  // bottom: magenta
    };
    
    
    // texture coord array
    GLfloat quadTexCoords[32] = { // 32 elements 2x3x4 +2x4
        0.5, 0.2,   0.4, 0.4,   0.6, 0.4,                // v0,v1,v2 (front)
        0.8, 0.5,   0.6, 0.4,   0.6, 0.6,                // v0,v3,v4 (right)
        0.5, 0.8,   0.6, 0.6,   0.4, 0.6,                 // v1,v6,v7 (back)
        0.2, 0.5,   0.4, 0.6,   0.4, 0.4,                // v4,v7,v6 (left)
        0.4, 0.4,   0.4, 0.6,   0.6, 0.6,   0.6, 0.4                // v7,v4,v3 (bottom)
    };
    
    // index array for glDrawElements()
    GLuint quadIndices[18] = { // (6 triangles x 3 indices)
       0, 1, 2,     // front
       3, 4, 5,     // right
       6, 7, 8,     // back
       9, 10, 11,   // left
       12, 13, 15,  // bottom 1
       15, 14, 13   // bottom 2
    };
    
    GLfloat quadNormals[48] = {  // 72 elements
        0, 0, 1,   0, 0, 1,   0, 0, 1,    // v0,v1,v2,v3 (front)
        1, 0, 0,   1, 0, 0,   1, 0, 0,  // v0,v3,v4,v5 (right)
        0, 0,-1,   0, 0,-1,   0, 0,-1,    // v4,v7,v6,v5 (back)
        -1, 0, 0,  -1, 0, 0,  -1, 0, 0,    // v1,v6,v7,v2 (left)
        0, -1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0 // v7,v4,v3,v2 (bottom)

    };

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    
    unsigned int vSize = sizeof(quadVertices);
    unsigned int nSize = sizeof(quadNormals);
    unsigned int cSize = sizeof(quadColors);
    unsigned int tSize = sizeof(quadTexCoords);
    
    
    QuadPyra() {
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        
        // copy vertex attrib data to VBO
        //glBindBuffer(GL_ARRAY_BUFFER, VBO);
        //glBufferData(GL_ARRAY_BUFFER, vSize+cSize, 0, GL_STATIC_DRAW); // reserve space
        //glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, quadVertices);         // copy verts at offset 0
        //glBufferSubData(GL_ARRAY_BUFFER, vSize, cSize, quadColors);       // copy cols after positions
        
        
        // copy vertex attrib data to VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vSize+nSize+cSize+tSize, 0, GL_STATIC_DRAW); // reserve space
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, quadVertices);                  // copy verts at offset 0
        glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, quadNormals);               // copy norms after verts
        glBufferSubData(GL_ARRAY_BUFFER, vSize+nSize, cSize, quadColors);          // copy cols after norms
        glBufferSubData(GL_ARRAY_BUFFER, vSize+nSize+cSize, tSize, quadTexCoords); // copy texs after cols
        
        // copy index data to EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
        
        // attribute position initialization
    
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  // position attrib
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)vSize); // normal attrib
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(vSize+nSize)); //color attrib
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)(vSize+nSize+cSize)); // tex coord
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };
    
    void draw(Shader *shader) {
        shader->use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    };
};


#endif
