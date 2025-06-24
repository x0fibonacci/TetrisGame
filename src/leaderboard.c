// Подключение заголовочных файлов
#include <include/leaderboard.h>
#include <stdio.h>
#include <string.h>

// Структура записи в таблице лидеров
typedef struct {
    char name[20];
    int score;
} LeaderboardEntry;

// Отображение таблицы лидеров (вывод в консоль)
void displayLeaderboard() {
    FILE* file = fopen("leaderboard.txt", "r");
    if (!file) {
        printf("Таблица лидеров пуста\n");
        return;
    }
    LeaderboardEntry entries[MAX_LEADERBOARD];
    int count = 0;
    while (count < MAX_LEADERBOARD && fscanf(file, "%s %d", entries[count].name, &entries[count].score) == 2) {
        count++;
    }
    fclose(file);
    printf("Таблица лидеров:\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s - %d\n", i + 1, entries[i].name, entries[i].score);
    }
}

// Сохранение результата в таблицу лидеров
void saveLeaderboard(GameState* game) {
    LeaderboardEntry entries[MAX_LEADERBOARD + 1];
    int count = 0;
    FILE* file = fopen("leaderboard.txt", "r");
    if (file) {
        while (count < MAX_LEADERBOARD && fscanf(file, "%s %d", entries[count].name, &entries[count].score) == 2) {
            count++;
        }
        fclose(file);
    }
    entries[count].score = game->score;
    strcpy(entries[count].name, game->playerName);
    count++;
    // Сортировка по убыванию очков
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (entries[j].score < entries[j + 1].score) {
                LeaderboardEntry temp = entries[j];
                entries[j] = entries[j + 1];
                entries[j + 1] = temp;
            }
        }
    }
    // Сохранение топ-5
    file = fopen("leaderboard.txt", "w");
    for (int i = 0; i < count && i < MAX_LEADERBOARD; i++) {
        fprintf(file, "%s %d\n", entries[i].name, entries[i].score);
    }
    fclose(file);
}