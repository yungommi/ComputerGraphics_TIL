// 02_HelloTriangle.cpp
//  Created by 정문경 on 2022/03/29.
// - draw a (orange) 2D triangle: using 1 VBO and 1 VAO
// - ESC to quit

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
//function의 이름과 parameter만 보여줌
//window size가 바뀌면 GLFWwindow function 불러옴

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

// 보통은 shader file을 따로 두는데 여기서는 간단하게 single string으로 써서 표현함
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
//aPos : "in" attribute variable passed from application program, location is specified to 0
//gl_Position : predefined shader variable
/*~in shader file~
 version 330 core
 
 layout (location = 0) in vec3 aPos;
 
 void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
 }
 */

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.0f, 0.8f, 1.0f);\n"
    "}\n\0";

/* ~in shader file~
 version 330 core
 
 out vec4 FragColor;
 
 void main() {
    FragColor = vec4(1.0f, 0.5f, 0.0f, 1.0f);
 }
 */

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    if (!glfwInit()) //에러나면 terminate
    {
        printf("GLFW initialisation failed!");
        glfwTerminate();
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //we'll use OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //3.3이상 버전 사용
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //3.3이상 사용해도 허용

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HelloTriangle", NULL, NULL);
    //window 이름 HelloTriangle
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    //하나의 프로그램에서 여러개의 윈도우를 만들었을 경우 위 func를 사용하여 윈도우 정함
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //framebuffer resize 될때 불러올 함수 지정

    // Allow modern extension features
    glewExperimental = GL_TRUE;
    //we'll use modern OpenGL extensions
    
    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initialisation failed!");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    
    // build and compile our shader program
    // ------------------------------------
    
    
    // vertex shader
    //creating vertex shader object (id return)
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //creating vertex shader obj (id return) attach source string to the shader obj
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // check for shader compile errors
    glCompileShader(vertexShader);
    //check compile error and handle it
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    //GL_COMPILE_STATUS 라는 쿼리를 날려서 success라는 값 받아옴
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    
    // link shaders
    //각각의 shader은 위에서 compile했고, 여기서는 linking
    //creating shader program obj
    int shaderProgram = glCreateProgram();
    //attach vertexShader, fragmentShader obj to the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //linke the program
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    //after linking, shader obj can be deleted
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
         0.5f, -0.5f, 0.0f, // right
         0.0f,  0.5f, 0.0f  // top
    };

    unsigned int VBO, VAO;
    //creating VAO id
    glGenVertexArrays(1, &VAO);
    
    //creating buffer obj, returns the id of buffer obj
    //void glGenBuffers(int n, unsigned int *ids)
    //unsigned int vboCube[2]; \n glGenBuffers(2, vboCuve);
    glGenBuffers(1, &VBO);
    
    // bind VAO
    //(bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).)
    glBindVertexArray(VAO);

    //---------------------VBO included in the currently bound VAO------------------------------------------
    //hook the buffer obj with the corresponding id before isong the buffer obj
    //void glBindBuffer(GLenum target, GLuint id)  / target:GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER
    //GL_ARRAY_BUFFER : vertex attributes(coords, texture coords, normals, and color)
    //GL_ELEMENT_ARRAY_BUFFER:index
    //id: VBO id
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    //void glBufferData(GLenum target, GLsizei size, const void* data, GLenum usage)
    //copy the data into VBO
    //size = vertices, type = GL_STATIC_DRAW, data: pointer to the array of source data, usage: performance hint for VBO
    //usage=> static :변하지않음 dynamic:움직임 stream:동영상같은거
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //void glBufferSubData(GLenum target, GLint offset, GLsizei size, void* date)
    //copy data into some portion of VBO, only replace a range data into the existing buffer
    //starting from given offset
    //gl

    //glVertexAttribPointer(location, #component, type, normalized?, stride, offset)
    //offset -> 저점에서 시작해서 (void*)0 ??이해X
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    //shader에서 준 layout(location=0)이라는 값 활성화
    glEnableVertexAttribArray(0);
    //=================================VBO include in bound VAO complete!=================================
    //---------------------------------------unbind VBO & VAO ---------------------------------------------
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    //====================================unbind VBO & VAO complete! ====================================

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //---------------------------------rendering loop ----------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // input-------
        processInput(window); //process input if exists

        // render-------
        glClearColor(0.8f, 0.3f, 0.3f, 0.5f);
        //background color 설정 R.G.B.투명도
        glClear(GL_COLOR_BUFFER_BIT);
        //color buffer = main buffer for drawing

        //-------------------------select shader program and bind VAO -----------------------------------
        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //============================select shader program and bind VAO complete!==========================
        
        //draw VAO, primitive=GL_TRIANGLES, starting index = 0; count(#vertives = 3)
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //primitives 종류: GL_POINTS, LINES, LINE_STRIP, LINE_LOOP, TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN
        // glBindVertexArray(0); // no need to unbind it every time
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        //swap front and back buffers
        glfwPollEvents();
        //loop body의 마지막. check any next event occurred
    }

    //optional: de-allocate all resources once they've outlived their purpose-------------------------------
    glDeleteVertexArrays(1, &VAO);
    
    //void glDeleteBuffer(GLsizei n, const GLuint* ids); delete a single/multi VBO
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.-----------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    //if ESC key pressed, quit the program
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) //뒤의 인풋들은 변경 후의 사이즈
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    //global 에 값 저장
}

