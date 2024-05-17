#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "raylib.h"
#include <complex.h>
#include <math.h>

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

typedef struct {
    float left;
    float right;
} Frame;

#define N 256 

float pi;
float in[N];
float complex out[N];
float max;

void fft(float in[], size_t stride, float complex out[], size_t n) {

    assert(n > 0);

    if (n == 1) {
        out[0] = in[0];
        return;
    }

    fft(in, stride*2, out, n/2); // even
    fft(in + stride, stride*2, out + n/2, n/2); // odd
    
    // out = [ e e e e | o o o o ]

    for (size_t k = 0; k < n/2; ++k) {
        float t = (float)k/n;
        float complex v = cexp(-2*I*pi*t)*out[k + n/2];
        float complex e = out[k];
        out[k] = e + v;
        out[k + n/2] = e - v;
    }
}

float amp(float complex z) {
    float a = fabsf(crealf(z));
    float b = fabsf(cimagf(z));
    if (a < b) return b;
    return a;
}

void callback(void *bufferData, unsigned int frames) {
    if (frames < N) return;

    Frame *fs= bufferData;

    for (size_t i = 0; i < frames; ++i) {
        in[i] = fs[i].left;
    }

    fft(in, 1, out, N);

    max = 0.0f;
    for (size_t i = 0; i < frames; ++i) {
        float a = amp(out[i]);
        if (max < a) max = a;
    }
}

int main(void) {
    
    pi = atan2f(1, 1)*4;

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
        float cell_width = (float)w/N;
        for (size_t i = 0; i < N; ++i) {
            float t = amp(out[i])/max;
            DrawRectangle(i*cell_width, h/2 - h/2*t, cell_width, h/2*t, BLUE);
        }
        EndDrawing();
    }
    return 0;
}
