#ifndef RENDER_H
#define RENDER_H

#include <GL/glew.h>
#include <include/game.h>
#include <include/menu.h>

// Инициализация рендеринга
unsigned int initRendering();
// Отрисовка игры
void renderGame(unsigned int shaderProgram, GameState* game);
// Отрисовка меню
void renderMenu(unsigned int shaderProgram, MenuState* menu);
// Очистка ресурсов рендеринга
void cleanupRendering();

#endif