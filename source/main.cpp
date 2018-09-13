#include <stdio.h>

#include <iostream>
#include <chrono>
#include <ctime>


#include "SDL.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
};

float vertices[] = {
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // Top-left
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
        1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // Bottom-right
        -1.0f, -1.0f, 1.0f, 1.0f, 1.0f  // Bottom-left
};

auto t_start = std::chrono::high_resolution_clock::now();


static GLuint createAndCompileShader(GLenum type, const char *source) {
    GLint success;
    GLchar msg[512];

    GLuint handle = glCreateShader(type);
    if (!handle) {
        return 0;
    }
    glShaderSource(handle, 1, &source, NULL);
    glCompileShader(handle);
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(handle, sizeof(msg), NULL, msg);
        glDeleteShader(handle);
        return 0;
    }

    return handle;
}


// Shader sources
const GLchar *vertexSource = R"glsl(
    #version 330 core

    in vec2 position;
    in vec3 color;

    out vec3 Color;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;
    uniform mat4 scale;
    void main()
    {
        Color = color;
        gl_Position = proj * view * scale * model * vec4(position, 0.0, 1.0);
    }
)glsl";

const GLchar *fragmentSource = R"glsl(
    #version 330 core

    in vec3 Color;
    out vec4 outColor;
    void main()
    {
        outColor = vec4(Color, 1.0);
    }
)glsl";


SDL_Window *init_window() {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    return SDL_CreateWindow("OpenGL", 100, 100, 1280, 720, SDL_WINDOW_OPENGL);
}


GLuint bootstrap_vao() {
    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    return vao;
}

GLuint bootstrap_vbo(float vertices[], int size) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    return vbo;
}


GLuint bootstrap_ebo(GLuint elements[], int size) {
    GLuint ebo;
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 size, elements, GL_STATIC_DRAW);
    return ebo;
}


int main(int argc, char *argv[]) {

    auto t_start = std::chrono::high_resolution_clock::now();

    SDL_Window *window = init_window();
    SDL_GLContext context = SDL_GL_CreateContext(window);

    gladLoadGL();

    GLuint vao = bootstrap_vao();
    GLuint vbo = bootstrap_vbo(vertices, sizeof(vertices));
    GLuint ebo = bootstrap_ebo(elements, sizeof(elements));

    GLuint vertexShader = createAndCompileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = createAndCompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) (2 * sizeof(GLfloat)));


    GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    // Set up projection
    glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 2.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
    );
    GLint uniView = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 scale = glm::mat4(1.0f);
    scale = glm::scale(
            scale,
            glm::vec3(0.05f, 0.05f, 1.0f)
    );

    GLint uniScale = glGetUniformLocation(shaderProgram, "scale");
    glUniformMatrix4fv(uniScale, 1, GL_FALSE, glm::value_ptr(scale));


    glm::mat4 proj =
            glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.0f, 10.0f);
    GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));


    SDL_Event windowEvent;
    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT) break;
        }

        if (windowEvent.type == SDL_KEYUP &&
            windowEvent.key.keysym.sym == SDLK_ESCAPE)
            break;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Calculate transformation
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();


        for (int i = -10; i < 10; ++i) {
            for (int j = -5; j < 5; ++j) {
                glm::mat4 model = glm::translate(
                        glm::mat4(1.0f),
                        glm::vec3((i * 3.0f) + 1.0f, (j * 3.0f) + 2.0f,
                                  sin(time + (i * 1.0f + j * 1.0f) / 2) * 0.1f)
                );
                glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            }
        }

        SDL_GL_SwapWindow(window);
    }

    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}
