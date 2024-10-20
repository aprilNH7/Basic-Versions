#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shader sources
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 fragColor;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        fragColor = aColor;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 fragColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(fragColor, 1.0);
    }
)";

// Function to compile shader program
GLuint compileShaderProgram() {
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Link shaders into shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Delete shaders as they're linked into our program and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Function to create a perspective projection matrix
glm::mat4 createProjectionMatrix(float fov, float aspectRatio, float near, float far) {
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Error initializing GLFW" << std::endl;
        return -1;
    }

    // Create a window
    GLFWwindow* window = glfwCreateWindow(1200, 900, "3D Pyramid", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error creating GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error initializing GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Compile shader program
    GLuint shaderProgram = compileShaderProgram();
    if (shaderProgram == -1) {
        glfwTerminate();
        return -1;
    }

    // Define pyramid vertices and colors
    float vertices[] = {
        // Base vertices (counterclockwise order)
        -0.5f, -0.5f, -0.5f, // Vertex 0
         0.5f, -0.5f, -0.5f, // Vertex 1
         0.5f, -0.5f,  0.5f, // Vertex 2
        -0.5f, -0.5f,  0.5f, // Vertex 3
        // Apex vertex (top)
         0.0f,  0.5f,  0.0f  // Vertex 4
    };

    float colors[] = {
        0.0f, 1.0f, 0.0f,   // Green (Base)
        1.0f, 0.0f, 1.0f,  // Purple (Base)
        1.0f, 1.0f, 0.0f,  // Yellow (Base)
        0.0f, 0.0f, 1.0f,  // Blue (Base)
        1.0f, 1.0f, 1.0f,  // White (Top)

        
        
    };

    unsigned int indices[] = {
        0, 1, 4, // Base triangle 1
        1, 2, 4, // Base triangle 2
        2, 3, 4, // Base triangle 3
        3, 0, 4  // Base triangle 4
    };

    // Create vertex buffer object (VBO), color buffer object (CBO), and element buffer object (EBO)
    GLuint VBO, CBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &CBO);
    glGenBuffers(1, &EBO);

    // Bind VBO, CBO, and EBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attribute pointers for position and color
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the color and depth buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader program
        glUseProgram(shaderProgram);

        // Create perspective projection matrix
        glm::mat4 projection = createProjectionMatrix(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Set up view matrix (camera position and orientation)
        glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Set up model matrix (position and orientation of the pyramid)
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the Y-axis
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // Draw the pyramid
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &CBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
