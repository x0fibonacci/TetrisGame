// Подключение необходимых библиотек и заголовочных файлов
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <include/game.h>
#include <include/render.h>
#include <include/menu.h>
#include <include/leaderboard.h>
#include <include/audio.h>

// Callback для изменения размера окна
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main(void) {
    // Инициализация GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Ошибка инициализации GLFW\n");
        return -1;
    }

    // Настройка версии OpenGL и профиля
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(800, 600, "Тетрис", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Ошибка создания окна GLFW\n");
        glfwTerminate();
        return -1;
    }

    // Установка контекста и callback для изменения размера
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Инициализация GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Ошибка инициализации GLEW\n");
        glfwTerminate();
        return -1;
    }

    // Включение смешивания для прозрачности текстур
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Инициализация аудио
    if (!initAudio()) {
        fprintf(stderr, "Ошибка инициализации аудио\n");
        glfwTerminate();
        return -1;
    }

    // Инициализация рендеринга
    unsigned int shaderProgram = initRendering();
    if (!shaderProgram) {
        cleanupAudio();
        glfwTerminate();
        return -1;
    }

    // Инициализация игры
    GameState game;
    initGame(&game);

    // Инициализация меню
    MenuState menu = {0, 3, {"Начать", "Таблица лидеров", "Выход"}};

    // Главный цикл
    while (!glfwWindowShouldClose(window)) {
        if (menu.active) {
            // Отрисовка и обработка меню
            renderMenu(shaderProgram, &menu);
            handleMenuInput(window, &menu, &game);
        } else {
            // Обновление и отрисовка игры
            updateGame(&game);
            renderGame(shaderProgram, &game);
            handleGameInput(window, &game);
        }

        // Обмен буферов и обработка событий
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Очистка ресурсов
    cleanupRendering();
    cleanupAudio();
    saveLeaderboard(&game);
    glfwTerminate();
    return 0;
}