#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <complex.h>
#include <math.h>
#include <raylib.h>

#define ARRAY_LEN(xs) sizeof(xs)/sizeof(xs[0])

#define N (1<<13)

float in[N];
float complex out[N];

typedef struct {
    float left;
    float right;
} Frame;

typedef struct {
    Music music;
} Sample;

void fft(float in[], size_t stride, float complex out[], size_t n) {
    assert(n > 0);

    if (n == 1) {
        out[0] = in[0];
        return;
    }

    fft(in, stride*2, out, n/2);
    fft(in + stride, stride*2,  out + n/2, n/2);

    for (size_t k = 0; k < n/2; ++k) {
        float t = (float)k/n;
        float complex v = cexp(-2*I*PI*t)*out[k + n/2];
        float complex e = out[k];
        out[k]       = e + v;
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
    Frame *fs = bufferData;

    for (size_t i = 0; i < frames; ++i) {
        memmove(in, in + 1, (N - 1)*sizeof(in[0]));
        in[N-1] = fs[i].left;
    }
}

void init(Sample *sample, const char *file_path) {
    sample->music = LoadMusicStream(file_path);
    printf("music.frameCount = %u\n", sample->music.frameCount);
    printf("music.stream.sampleRate = %u\n", sample->music.stream.sampleRate);
    printf("music.stream.sampleSize = %u\n", sample->music.stream.sampleSize);
    printf("music.stream.channels = %u\n", sample->music.stream.channels);
    assert(sample->music.stream.sampleSize == 16);
    assert(sample->music.stream.channels == 2);

    SetMusicVolume(sample->music, 0.5f);
    PlayMusicStream(sample->music);
    AttachAudioStreamProcessor(sample->music.stream, callback);
}

void update(Sample *sample) {
    UpdateMusicStream(sample->music);

    if (IsKeyPressed(KEY_SPACE)) {
        if (IsMusicStreamPlaying(sample->music)) {
            PauseMusicStream(sample->music);
        } else {
            ResumeMusicStream(sample->music);
        }
    }

    if (IsKeyPressed(KEY_Q)) {
        StopMusicStream(sample->music);
        PlayMusicStream(sample->music);
    }

    int w = GetRenderWidth();
    int h = GetRenderHeight();

    BeginDrawing();
    ClearBackground(CLITERAL(Color) {
        0x48, 0x48, 0x48, 0xFF
    });

    fft(in, 1, out, N);

    float max = 0.0f;
    for (size_t i = 0; i < N; ++i) {
        float a = amp(out[i]);
        if (max < a) max = a;
    }

    float step = 1.06;
    size_t m = 0;
    for (float f = 20.0f; (size_t) f < N; f *= step) {
        m += 1;
    }

    float cell_width = (float)w/m;
    m = 0;
    for (float f = 20.0f; (size_t) f < N; f *= step) {
        float f1 = f*step;
        float a = 0.0f;
        for (size_t q = (size_t) f; q < N && q < (size_t) f1; ++q) {
            a += amp(out[q]);
        }
        a /= (size_t) f1 - (size_t) f + 1;
        float t = a/max;
        int c = (int)(t * 1000) % 255;
        DrawRectangle(m*cell_width, h/2 - h/2*t, cell_width/2, h/2*t, CLITERAL(Color){ c, 109, 194, 255 });
        m += 1;
    }
    EndDrawing();
}

char *shift_args(int *argc, char ***argv) {
    assert(*argc > 0);
    char *result = (**argv);
    (*argv) += 1;
    (*argc) -= 1;
    return result;
}

//    #TODO: Handle samples > 16

int main(int argc, char **argv) {
    Sample sample = {0};
    const char *program = shift_args(&argc, &argv);

    if (argc == 0) {
        fprintf(stderr, "Usage: %s <input>\n", program);
        fprintf(stderr, "ERROR: no input file is provided\n");
        return 1;
    }
    const char *file_path = shift_args(&argc, &argv);

    InitWindow(800, 600, "musac");
    SetTargetFPS(60);
    InitAudioDevice();

    init(&sample, file_path);
    while (!WindowShouldClose()) {
        update(&sample);
    }

    return 0;
}
