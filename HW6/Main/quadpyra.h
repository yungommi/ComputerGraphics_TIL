
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
    GLfloat vertices[48]  = { // (4 faces x 3 vertices x 3) + (4 vertices x 3) = 36 + 12 = 48
        .0f, .5f, .0f,   -.5f, -.5f,  .5f,    .5f, -.5f,  .5f,   // front
        .0f, .5f, .0f,    .5f, -.5f,  .5f,    .5f, -.5f, -.5f,   // right
        .0f, .5f, .0f,    .5f, -.5f, -.5f,   -.5f, -.5f, -.5f,   // back
        .0f, .5f, .0f,   -.5f, -.5f, -.5f,   -.5f, -.5f,  .5f,   // left
       -.5f,-.5f, .5f,   -.5f, -.5f, -.5f,    .5f, -.5f, -.5f,  .5f, -.5f, .5f // bottom
    };
    
    // color array
    GLfloat colors[64] = { // (4 faces x 3 vertices x 4) + (4 vertices x 4) = 48 + 16 = 64
        1, 0, 0, 1,  1, 0, 0, 1,  1, 0, 0, 1,    // front: red
        0, 1, 0, 1,  0, 1, 0, 1,  0, 1, 0, 1,    // right: green
        0, 0, 1, 1,  0, 0, 1, 1,  0, 0, 1, 1,    // back: blue
        1, 1, 0, 1,  1, 1, 0, 1,  1, 1, 0, 1,    // left: yellow
        1, 0, 1, 1,  1, 0, 1, 1,  1, 0, 1, 1,  1, 0, 1, 1  // bottom: magenta
    };
    
    // index array for glDrawElements()

    GLuint cubeIndices[18] = { // (6 triangles x 3 indices)
       0, 1, 2,     // front
       3, 4, 5,     // right
       6, 7, 8,     // back
       9, 10, 11,   // left
       12, 13, 15,  // bottom 1
       15, 14, 13   // bottom 2
    };

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    
    unsigned int vSize = sizeof(vertices);
    unsigned int cSize = sizeof(colors);
    
    QuadPyra() {
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        
        // copy vertex attrib data to VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vSize+cSize, 0, GL_STATIC_DRAW); // reserve space
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, vertices);         // copy verts at offset 0
        glBufferSubData(GL_ARRAY_BUFFER, vSize, cSize, colors);       // copy cols after positions
        
        // copy index data to EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
        
        // attribute position initialization
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  // position attrib
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)vSize); //color attrib

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        
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
