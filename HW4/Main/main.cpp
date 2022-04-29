// Homework 04: Quadratic Curve - Line Intersection
//   - Input line using left mouse button dragging

#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <shader.h>

using namespace std;

unsigned int VAOQuad;    // VAO for quadratic curve
unsigned int VBOQuad;    // VBO for quadratic curve

unsigned int VAOLine;    // VAO for line segment
unsigned int VBOLine;    // VBO for line segment

unsigned int VAOInter;   // VAO for intersection points
unsigned int VBOInter;   // VBO for intersection points

// constants and types
const int NSUBDIV = 64;  // number of subdivision for curve drawing
const float quadColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
const float lineColor[] = {0.0f, 1.0f, 0.0f, 1.0f};
const float interColor[] = {1.0f, 1.0f, 0.0f, 1.0f};

enum State {START, LINE_INPUT, INPUT_DONE};  // state of mouse button

// global variables
GLFWwindow *window = NULL;
Shader *globalShader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
State state = START;
bool dragging = false;  // true during the mouse dragging

float quadV[(NSUBDIV + 1) * 2]; // coords of points on quadratic curve
float lineV[4];        // coords of two end points of input line segment
int nInter = 0;        // # of intersection points (0, 1, or 2)
float interV[4];       // coords of intersection points

// function prototypes
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void normalize_cursor_position(double x, double y, float &nx, float &ny);
void update_vb_vertex(float x, float y);
void compute_intersections();
int quadratic_solver(float a, float b, float c, float &root1, float &root2);
void render();


int main()
{
    window = glAllInit();

    // shader loading and compile (by calling the constructor)
    globalShader = new Shader("hw04.vs", "hw04.fs");
    
    // creating VAOs and VBOs
    glGenVertexArrays(1, &VAOQuad);
    glGenVertexArrays(1, &VAOLine);
    glGenVertexArrays(1, &VAOInter);
    glGenBuffers(1, &VBOQuad);
    glGenBuffers(1, &VBOLine);
    glGenBuffers(1, &VBOInter);
    
    // calculating points quadV on the quadratic curve: y = 2x^2 - (4/5)x - 0.42
    float xStep = 2.0f / NSUBDIV;
    for (int i = 0; i < NSUBDIV + 1; i++) {
        float x;
        if (i == 0) x = -1.0;
        else if (i == NSUBDIV) x = 1.0;
        else {
            x = -1.0 + xStep * i;
        }
        quadV[i * 2] = x;
        quadV[i * 2 + 1] = 2.0f * x * x - (4 / 5.0f) * x - 0.42;
    }

    // setup VAOQuad and VBOQuad
    glBindVertexArray(VAOQuad);
    glBindBuffer(GL_ARRAY_BUFFER, VBOQuad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadV), quadV, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // render loop
    while (!glfwWindowShouldClose(window)) {
        render();
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

GLFWwindow *glAllInit()
{
    GLFWwindow *window;
    
    // glfw: initialize and configure
    if (!glfwInit()) {
        printf("GLFW initialisation failed!");
        glfwTerminate();
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // glfw window creation
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Homework04", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    // Allow modern extension features
    glewExperimental = GL_TRUE;
    
    // set some OpenGL states (for transparency and background color)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // background color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f) ;
    
    if (glewInit() != GLEW_OK) {
        cout << "GLEW initialisation failed!" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(-1);
    }
    
    return window;
    
}

void render() {
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    globalShader->use();
    
    // draw qudratic curve
    glBindVertexArray(VAOQuad);
    globalShader->setVec4("inColor", quadColor[0], quadColor[1],
                            quadColor[2], quadColor[3]);
    glDrawArrays(GL_LINE_STRIP, 0, NSUBDIV + 1);
    glBindVertexArray(0);
    
    // draw line segment
    if (state >= LINE_INPUT) {
        glBindVertexArray(VAOLine);
        globalShader->setVec4("inColor", lineColor[0], lineColor[1],
                              lineColor[2], lineColor[3]);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
    }
    
    // draw intersection points
    if (state >= INPUT_DONE) {
        glBindVertexArray(VAOInter);
        globalShader->setVec4("inColor", interColor[0], interColor[1],
                              interColor[2], interColor[3]);
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, nInter);
        glBindVertexArray(0);
    }
    
    glfwSwapBuffers(window);
    
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
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
    float nx, ny;
    if (dragging) {
        normalize_cursor_position(xpos, ypos, nx, ny);
        update_vb_vertex(nx, ny);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    double xpos, ypos;
    float nx, ny;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            glfwGetCursorPos(window, &xpos, &ypos);
            normalize_cursor_position(xpos, ypos, nx, ny);
            
            if (state == START) {
                state = LINE_INPUT;
                lineV[0] = lineV[2] = nx;
                lineV[1] = lineV[3] = ny;
                glBindVertexArray(VAOLine);
                glBindBuffer(GL_ARRAY_BUFFER, VBOLine);
                glBufferData(GL_ARRAY_BUFFER, sizeof(lineV), lineV, GL_STATIC_DRAW);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
                dragging = true;
            }
            
        }
        else if (action == GLFW_RELEASE) {
            dragging = false;
            glfwGetCursorPos(window, &xpos, &ypos);
            normalize_cursor_position(xpos, ypos, nx, ny);
            update_vb_vertex(nx, ny);
            if (state == LINE_INPUT) {
                compute_intersections();
                state = INPUT_DONE;
            }
        }
    }
}

void normalize_cursor_position(double x, double y, float &nx, float &ny)
{
    nx = ((float)x / (float)SCR_WIDTH) * 2.0f - 1.0f;
    ny = -1.0f * (((float)y / (float)SCR_HEIGHT) * 2.0f - 1.0f);
}

void update_vb_vertex(float x, float y)
{
    if (state == LINE_INPUT) {
        lineV[2] = x;
        lineV[3] = y;
        glBindBuffer(GL_ARRAY_BUFFER, VAOLine);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lineV), lineV);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

// compute_intersections: compute the intersection between the circle and the line segment
void compute_intersections() {
    
    // parametric line: L(t) = (x(t), y(t)) = (at + b, ct + d), 0 <= t <= 1
    float x1 = lineV[0], y1 = lineV[1], x2 = lineV[2], y2 = lineV[3];
    float a = x2 - x1, b = x1, c = y2 - y1, d = y1;

    // quadratic curve y = 2x^2 - (4/5)x - 0.42
    
    // quadratic equation of t by plugging (at + b, ct + d) into the x and y of the quadratic curve
    // quadratic equation At^2 + Bt + C = 0
    
    float fbf = 4.0f / 5;
    
    float A = 2.0f * a * a,
        B = (4.0f * a * b) - (fbf * a) - c,
        C = (2.0f * b * b) - (fbf * b + 0.42f) - d;
    float t1, t2;  // roots computed
    int nRoot = quadratic_solver(A, B, C, t1, t2);  // # of roots
    
    if (nRoot == 2) { // 2 zeros
        if (0.0f <= t1 && t1 <= 1.0f) {
            interV[nInter * 2] = a * t1 + b;
            interV[nInter * 2 + 1] = c * t1 + d;
            nInter++;
        }
        if (0.0f <= t2 && t2 <= 1.0f) {
            interV[nInter * 2] = a * t2 + b;
            interV[nInter * 2 + 1] = c * t2 + d;
            nInter++;
        }
    }
    else if (nRoot == 1) { // 1 zero
        if (0.0f <= t1 && t1 <= 1.0f) {
            interV[nInter * 2] = a * t1 + b;
            interV[nInter * 2 + 1] = c * t1 + d;
            nInter++;
        }
    }

    glBindVertexArray(VAOInter);
    glBindBuffer(GL_ARRAY_BUFFER, VBOInter);
    glBufferData(GL_ARRAY_BUFFER, sizeof(interV), interV, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
}

// quadratic_solver: solving quadratic equation ax^2 + bx + c = 0
//   return value: the number of roots
//   root1, root2: the root values
int quadratic_solver(float a, float b, float c, float &root1, float &root2) {
    
    float discriminant = b*b-4*a*c;
    
    if (discriminant > 0) {
        // sqrt() function returns square root
        root1 = (-b+sqrt(discriminant))/(2*a);
        root2 = (-b-sqrt(discriminant))/(2*a);
        return 2;
    }
    else if (fabs(discriminant) < 0.000001) {
        root1 = root2 = -b/(2*a);
        return 1;
    }
    return 0;
}

