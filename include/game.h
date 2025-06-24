#ifndef GAME_H
#define GAME_H

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define MAX_LEADERBOARD 5

// Структура состояния игры
typedef struct {
    int board[BOARD_HEIGHT][BOARD_WIDTH]; // Игровое поле
    int currentPiece[4][4]; // Текущая фигурка
    int pieceX, pieceY; // Позиция фигурки
    int score; // Очки
    int gameOver; // Флаг окончания игры
    double lastFallTime; // Время последнего падения
    char playerName[20]; // Имя игрока
} GameState;

// Инициализация игры
void initGame(GameState* game);
// Обновление состояния игры
void updateGame(GameState* game);
// Обработка ввода для игры
void handleGameInput(GLFWwindow* window, GameState* game);

#endif