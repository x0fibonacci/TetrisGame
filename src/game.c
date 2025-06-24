// Подключение заголовочных файлов
#include <include/game.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Определение фигурок Тетриса
static const int tetrominoes[7][4][4] = {
    {{1,1,1,1}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}, // I
    {{1,1,0,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}}, // O
    {{1,1,1,0}, {0,1,0,0}, {0,0,0,0}, {0,0,0,0}}, // T
    {{1,1,1,0}, {1,0,0,0}, {0,0,0,0}, {0,0,0,0}}, // L
    {{1,1,1,0}, {0,0,1,0}, {0,0,0,0}, {0,0,0,0}}, // J
    {{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}, // S
    {{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}}  // Z
};

// Проверка столкновения фигурки с полем
static int checkCollision(GameState* game, int newX, int newY, int piece[4][4]) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (piece[y][x]) {
                int boardX = newX + x;
                int boardY = newY + y;
                if (boardX < 0 || boardX >= BOARD_WIDTH || boardY >= BOARD_HEIGHT || (boardY >= 0 && game->board[boardY][boardX])) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

// Вращение фигурки
static void rotatePiece(int piece[4][4], int rotated[4][4]) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            rotated[x][3-y] = piece[y][x];
        }
    }
}

// Инициализация игры
void initGame(GameState* game) {
    // Очистка игрового поля
    memset(game->board, 0, sizeof(game->board));
    // Сброс параметров
    game->score = 0;
    game->gameOver = 0;
    game->lastFallTime = glfwGetTime();
    strcpy(game->playerName, "Player");
    // Создание новой фигурки
    int pieceIndex = rand() % 7;
    memcpy(game->currentPiece, tetrominoes[pieceIndex], sizeof(game->currentPiece));
    game->pieceX = BOARD_WIDTH / 2 - 2;
    game->pieceY = -1;
}

// Обновление состояния игры
void updateGame(GameState* game) {
    if (game->gameOver) return;

    // Падение фигурки каждые 0.5 секунды
    double currentTime = glfwGetTime();
    if (currentTime - game->lastFallTime > 0.5) {
        if (!checkCollision(game, game->pieceX, game->pieceY + 1, game->currentPiece)) {
            game->pieceY++;
        } else {
            // Фиксация фигурки на поле
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    if (game->currentPiece[y][x] && game->pieceY + y >= 0) {
                        game->board[game->pieceY + y][game->pieceX + x] = 1;
                    }
                }
            }
            // Проверка заполненных линий
            int linesCleared = 0;
            for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
                int full = 1;
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    if (!game->board[y][x]) {
                        full = 0;
                        break;
                    }
                }
                if (full) {
                    linesCleared++;
                    for (int yy = y; yy > 0; yy--) {
                        memcpy(game->board[yy], game->board[yy-1], sizeof(game->board[yy]));
                    }
                    memset(game->board[0], 0, sizeof(game->board[0]));
                    y++;
                }
            }
            game->score += linesCleared * 100;
            // Создание новой фигурки
            int pieceIndex = rand() % 7;
            memcpy(game->currentPiece, tetrominoes[pieceIndex], sizeof(game->currentPiece));
            game->pieceX = BOARD_WIDTH / 2 - 2;
            game->pieceY = -1;
            // Проверка окончания игры
            if (checkCollision(game, game->pieceX, game->pieceY, game->currentPiece)) {
                game->gameOver = 1;
            }
            game->lastFallTime = currentTime;
        }
    }
}

// Обработка ввода для игры
void handleGameInput(GLFWwindow* window, GameState* game) {
    if (game->gameOver) {
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            initGame(game);
        }
        return;
    }

    // Движение влево
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        if (!checkCollision(game, game->pieceX - 1, game->pieceY, game->currentPiece)) {
            game->pieceX--;
        }
    }
    // Движение вправо
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (!checkCollision(game, game->pieceX + 1, game->pieceY, game->currentPiece)) {
            game->pieceX++;
        }
    }
    // Ускоренное падение
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (!checkCollision(game, game->pieceX, game->pieceY + 1, game->currentPiece)) {
            game->pieceY++;
            game->lastFallTime = glfwGetTime();
        }
    }
    // Вращение
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        int rotated[4][4];
        rotatePiece(game->currentPiece, rotated);
        if (!checkCollision(game, game->pieceX, game->pieceY, rotated)) {
            memcpy(game->currentPiece, rotated, sizeof(game->currentPiece));
        }
    }
}