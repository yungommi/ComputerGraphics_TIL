// 11_Polyline
//    - Mouse: left button: add a vertex, right button: stop input
//    - Keyboard 'r' to reset all inputs, ESC to quit

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader.h>

using namespace std;

// callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

// other functions
void normalize_cursor_position(double x, double y, float &nx, float &ny);
void update_vb_vertex(int vIndex, float x, float y);
void render(GLFWwindow *window);

// global settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
const int MAX_VERTEX = 200; //입력받을수있는 버텍스 크기
const int MAX_VERTEX_SIZE = MAX_VERTEX * 3; //3D
const int MAX_COLOR_SIZE = MAX_VERTEX * 4; //버텍스 하나당 색깔 RGBA
int nPoints = 0;
bool stopInput = false; //right mouse => true => stop input
bool nextPoint = false; //mouse cursor moving
Shader ourShader;
unsigned int VBO[2], VAO; //vbo [position, color] => vao
float vertices[MAX_VERTEX_SIZE]; //현재까지입력된 vetex data
float colors[MAX_COLOR_SIZE];
float colorOriginal[] = {
    1.0, 0.0, 0.0, 1.0, // red
    1.0, 1.0, 0.0, 1.0, // yellow
    0.0, 1.0, 0.0, 1.0, // green
    0.0, 1.0, 1.0, 1.0, // cyan
    0.0, 0.0, 1.0, 1.0, // blue
    1.0, 0.0, 1.0, 1.0  // magenta
}; //color 돌아가면서 사용

int main()
{
    if (!glfwInit())
    {
        printf("GLFW initialisation failed!");
        glfwTerminate();
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Polyline", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    glfwSetKeyCallback(window, key_callback); //r => num points = 0 , esc
    glfwSetCursorPosCallback(window, cursor_position_callback); //mouse pointer 움직여서 좌표받음
    glfwSetMouseButtonCallback(window, mouse_button_callback); //버튼 눌릴때 left point => add a position

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initialisation failed!");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    // shader loading and compile (by calling the constructor)
    ourShader.initShader("3.1.shader.vs", "3.1.shader.fs");
    
    // initialize color buffer data
    for (int i = 0, j = 0; i < MAX_VERTEX; i++, j = (j+1)%6) {
        colors[i*4] = colorOriginal[j*4];
        colors[i*4+1] = colorOriginal[j*4+1];
        colors[i*4+2] = colorOriginal[j*4+2];
        colors[i*4+3] = 1.0f;
    }

    glGenVertexArrays(1, &VAO);  // vertex array object
    glGenBuffers(2, VBO);        // vertex and color buffers
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //position

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //color

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        render(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(2, VBO);
    glfwTerminate();
    
    return 0;
}

void render(GLFWwindow *window) {
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    ourShader.use();
    
    if (nPoints > 0) {
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_STRIP, 0, nPoints);
    }
    
    glfwSwapBuffers(window);

}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        nPoints = 0;
        stopInput = false; //input 받을 수 있음
        nextPoint = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT= height;
    glViewport(0, 0, width, height);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (!stopInput) {
        float cx, cy;
        if (!nextPoint) {
            nPoints++;
            nextPoint = true;
        }
        normalize_cursor_position(xpos, ypos, cx, cy);
        update_vb_vertex(nPoints - 1, cx, cy);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    double xpos, ypos;
    float nx, ny;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (!stopInput) {
            nextPoint = false;
            if (nPoints == 0) nPoints = 1;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (!stopInput) { // final point
            glfwGetCursorPos(window, &xpos, &ypos);
            normalize_cursor_position(xpos, ypos, nx, ny);
            update_vb_vertex(nPoints, nx, ny);
            stopInput = true;
        }
    }
}

void normalize_cursor_position(double x, double y, float &nx, float &ny)
{
    nx = ((float)x / (float)SCR_WIDTH) * 2.0f - 1.0f;
    ny = -1.0f * (((float)y / (float)SCR_HEIGHT) * 2.0f - 1.0f);
}

void update_vb_vertex(int vIndex, float x, float y)
{
    float n[3];
    n[0] = x;
    n[1] = y;
    n[2] = 0.0f;
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vIndex * sizeof(float) * 3 , sizeof(float) * 3, n);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

