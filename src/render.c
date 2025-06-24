// Подключение необходимых библиотек
#include <include/render.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <include/stb_image.h>

// Глобальные переменные для рендеринга
static unsigned int VAO, VBO, textureBlock, textureBackground;

// Загрузка текстуры
static unsigned int loadTexture(const char* path) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        fprintf(stderr, "Ошибка загрузки текстуры: %s\n", path);
    }
    stbi_image_free(data);
    return texture;
}

// Загрузка шейдера
static unsigned int loadShader(const char* path, GLenum type) {
    FILE* file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Ошибка открытия файла шейдера: %s\n", path);
        return 0;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* source = (char*)malloc(length + 1);
    fread(source, 1, length, file);
    source[length] = '\0';
    fclose(file);

    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Ошибка компиляции шейдера: %s\n", infoLog);
        free(source);
        return 0;
    }
    free(source);
    return shader;
}

// Инициализация рендеринга
unsigned int initRendering() {
    // Загрузка шейдеров
    unsigned int vertexShader = loadShader("../shaders/vertex.glsl", GL_VERTEX_SHADER);
    unsigned int fragmentShader = loadShader("../shaders/fragment.glsl", GL_FRAGMENT_SHADER);
    if (!vertexShader || !fragmentShader) {
        return 0;
    }
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "Ошибка линковки программы шейдеров: %s\n", infoLog);
        return 0;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Настройка вершинного буфера
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };
    unsigned int indices[] = {0, 1, 2, 2, 3, 0};
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Загрузка текстур
    textureBlock = loadTexture("../textures/block.png");
    textureBackground = loadTexture("../textures/background.png");

    return shaderProgram;
}

// Отрисовка игры
void renderGame(unsigned int shaderProgram, GameState* game) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glm::mat4 projection = glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Отрисовка фона
    glBindTexture(GL_TEXTURE_2D, textureBackground);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(800.0f, 600.0f, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Отрисовка игрового поля
    glBindTexture(GL_TEXTURE_2D, textureBlock);
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->board[y][x]) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3((x - BOARD_WIDTH/2) * 30.0f, (BOARD_HEIGHT/2 - y) * 30.0f, 0.0f));
                model = glm::scale(model, glm::vec3(30.0f, 30.0f, 1.0f));
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
    }

    // Отрисовка текущей фигурки
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (game->currentPiece[y][x] && game->pieceY + y >= 0) {
                model = glm::translate(glm::mat4(1.0f), glm::vec3((game->pieceX + x - BOARD_WIDTH/2) * 30.0f, (BOARD_HEIGHT/2 - (game->pieceY + y)) * 30.0f, 0.0f));
                model = glm::scale(model, glm::vec3(30.0f, 30.0f, 1.0f));
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
    }
}

// Отрисовка меню
void renderMenu(unsigned int shaderProgram, MenuState* menu) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glm::mat4 projection = glm::ortho(-400.0f, 400.0f, -300.0f, 300.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Отрисовка фона
    glBindTexture(GL_TEXTURE_2D, textureBackground);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(800.0f, 600.0f, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Отрисовка пунктов меню (упрощенно, как квадраты)
    glBindTexture(GL_TEXTURE_2D, textureBlock);
    for (int i = 0; i < menu->itemCount; i++) {
        float yPos = 50.0f - i * 50.0f;
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, yPos, 0.0f));
        model = glm::scale(model, glm::vec3(i == menu->selected ? 200.0f : 180.0f, 40.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

// Очистка ресурсов рендеринга
void cleanupRendering() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &textureBlock);
    glDeleteTextures(1, &textureBackground);
}