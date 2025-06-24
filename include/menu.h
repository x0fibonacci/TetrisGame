#ifndef MENU_H
#define MENU_H

#include <include/game.h>

// Структура состояния меню
typedef struct {
    int active; // Активно ли меню
    int selected; // Выбранный пункт
    int itemCount; // Количество пунктов
    const char* items[3]; // Названия пунктов
} MenuState;

// Обработка ввода для меню
void handleMenuInput(GLFWwindow* window, MenuState* menu, GameState* game);

#endif