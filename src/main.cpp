#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <learnopengl/shader_m.h>
#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD

// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually.
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 1280;




glm::vec3 translations[] = {
        glm::vec3(0.0f, -2.0f * sqrt(3.0f) / 6.0f, 0.0f),
        glm::vec3(-0.5f, sqrt(3.0f) / 6.0f, 0.0f),
        glm::vec3(0.5f, sqrt(3.0f) / 6.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, sqrt(6.0f) / 3.0f),
};

void generateSierpinskiTriangle(Shader newShader, glm::mat4 model, int maxRec, int curRec) {
    if (curRec >= maxRec) {
        int modelLoc = glGetUniformLocation(newShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 12);
        return;
    }

    if (curRec == 0) {
        int modelLoc = glGetUniformLocation(newShader.ID, "model");
        glm::mat4 largestTriangleModel = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(largestTriangleModel));
        glDrawArrays(GL_TRIANGLES, 12, 6);
    }

    for (size_t i = 0; i < 4; i++) {
        glm::mat4 newModel = glm::scale(model, glm::vec3(0.5f));
        newModel = glm::translate(newModel, translations[i]);
        generateSierpinskiTriangle(newShader, newModel, maxRec, curRec + 1);
    }
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "242574 SIERPINSKI TRIANGLE", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
#endif
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    ImGui::StyleColorsDark();


    Shader ourShader("res/shaders/texture.vert", "res/shaders/texture.frag");

    float a = 1.0f;

    float vertices[] = {
            // Pierwszy trójkąt
            a / 2, float(a * sqrt(3)) / 6, 0.0f,               1.0f, 1.0f, // C
            -a / 2, float(a * sqrt(3)) / 6, 0.0f,              0.0f, 0.0f, // D
            0.0f, float(-a * 2 * sqrt(3)) / 6, 0.0f,        1.0f, 0.0f, // B

            // Drugi trójkąt
            a / 2, float(a * sqrt(3)) / 6, 0.0f,               1.0f, 1.0f, // C
            -a / 2, float(a * sqrt(3)) / 6, 0.0f,              0.0f, 0.0f, // D
            0.0f, 0.0f, float(a * sqrt(6)) / 3,           1.0f, 0.0f, // A

            // Trzeci trójkąt
            0.0f, float(-a * 2 * sqrt(3)) / 6, 0.0f,        1.0f, 1.0f, // B
            0.0f, 0.0f, float(a * sqrt(6)) / 3,           0.0f, 0.0f, // A
            -a / 2, float(a * sqrt(3)) / 6, 0.0f,              1.0f, 0.0f, // D

            // Czwarty trójkąt
            0.0f, float(-a * 2 * sqrt(3)) / 6, 0.0f,        1.0f, 1.0f, // B
            a / 2, float(a * sqrt(3)) / 6, 0.0f,               1.0f, 0.0f, // C
            0.0f, 0.0f, float(a * sqrt(6)) / 3,           0.0f, 0.0f, // A
    };



    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);




    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);



    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("res/textures/stone.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    ourShader.use();
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static int recursion = 0, xAxis = 0, yAxis = 0;
        static ImVec4 fractalColor = ImVec4(2.0f, 0.1f, 1.4f, 1.5f);
        int colorLoc = glGetUniformLocation(ourShader.ID, "fractalColor");
        glUniform4f(colorLoc, fractalColor.x, fractalColor.y, fractalColor.z, fractalColor.w);

        ImGui::SliderInt("X AXIS", &xAxis, 1, 360);
        ImGui::SliderInt("Y AXIS", &yAxis, 1, 360);
        ImGui::SliderInt("RECURSION", &recursion, 0, 10);
        ImGui::ColorEdit3("FRACTAL COLOR", (float*)&fractalColor);

        glm::mat4 model         = glm::mat4(1.0f);
        glm::mat4 view          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);


        model = glm::rotate(model, glm::radians((float)xAxis), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians((float)yAxis), glm::vec3(1.0f, 0.0f, 0.0f));

        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);




        ImGui::Render();
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // bind Texture
        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);


        ourShader.use();

        glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());




        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

        ourShader.setMat4("projection", projection);

        glBindVertexArray(VAO);

        generateSierpinskiTriangle(ourShader, model, recursion, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    glfwDestroyWindow(window);
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);


}

