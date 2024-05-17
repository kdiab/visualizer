#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "raylib.h"
#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

uint32_t global_frames[1024];
size_t global_frames_count = 0;

void callback(void *bufferData, unsigned int frames) {
    if (frames > ARRAY_LEN(global_frames)) {
        frames = ARRAY_LEN(global_frames);
    }
    memcpy(global_frames, bufferData, sizeof(uint32_t)*frames);
    global_frames_count = frames;
}

int main(void) {
    InitWindow(800, 600, "Music");
    SetTargetFPS(60);

    InitAudioDevice();
    Music music = LoadMusicStream("ogi-feel-the-beat-expanded-191267.mp3");
    PlayMusicStream(music);
    AttachAudioStreamProcessor(music.stream, callback);

    while (!WindowShouldClose()) {
        UpdateMusicStream(music);

        if (IsKeyPressed(KEY_SPACE)) {
            if (IsMusicStreamPlaying(music)) {
                PauseMusicStream(music);
            } else {
                ResumeMusicStream(music);
            }
        }
        
        int w = GetRenderWidth();
        int h = GetRenderHeight();

        BeginDrawing();
        ClearBackground(BLACK);
        float cell_width = (float)w/global_frames_count;
        for (size_t i = 0; i < global_frames_count; i++) {
            int16_t sample = *(int16_t*)&global_frames[i];
            if (sample > 0) {
                float t = (float)sample/INT16_MAX;
                DrawRectangle(i * cell_width, h/2 - h/2 * t, cell_width, h / 2 * t, RED);
            } else {
                float t = (float)sample/INT16_MIN;
                DrawRectangle(i * cell_width, h / 2, cell_width, h / 2 * t, RED);
            }
        }
        EndDrawing();
    }
    return 0;
}
