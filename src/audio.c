// Подключение заголовочных файлов
#include <include/audio.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

// Глобальные указатели на аудио
static Mix_Music* backgroundMusic = NULL;
static Mix_Chunk* clearSound = NULL;

// Инициализация аудио
int initAudio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Ошибка инициализации SDL_mixer: %s\n", Mix_GetError());
        return 0;
    }
    backgroundMusic = Mix_LoadMUS("../audio/background.wav");
    if (!backgroundMusic) {
        fprintf(stderr, "Ошибка загрузки фоновой музыки: %s\n", Mix_GetError());
        return 0;
    }
    clearSound = Mix_LoadWAV("../audio/clear.wav");
    if (!clearSound) {
        fprintf(stderr, "Ошибка загрузки звука очистки: %s\n", Mix_GetError());
        return 0;
    }
    Mix_PlayMusic(backgroundMusic, -1);
    return 1;
}

// Очистка аудио ресурсов
void cleanupAudio() {
    Mix_FreeMusic(backgroundMusic);
    Mix_FreeChunk(clearSound);
    Mix_CloseAudio();
}