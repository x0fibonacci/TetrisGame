// Подключение заголовочных файлов
#include <include/menu.h>
#include <GLFW/glfw3.h>
#include <include/leaderboard.h>

// Обработка ввода для меню
void handleMenuInput(GLFWwindow* window, MenuState* menu, GameState* game) {
    // Переключение пунктов меню
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        menu->selected = (menu->selected - 1 + menu->itemCount) % menu->itemCount;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        menu->selected = (menu->selected + 1) % menu->itemCount;
    }
    // Выбор пункта
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (menu->selected == 0) { // Начать
            menu->active = 0;
            initGame(game);
        } else if (menu->selected == 1) { // Таблица лидеров
            displayLeaderboard();
        } else if (menu->selected == 2) { // Выход
            glfwSetWindowShouldClose(window, 1);
        }
    }
}