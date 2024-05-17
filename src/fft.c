#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <assert.h>

float pi;

void dft(float in[], float complex out[], size_t n) {

    for (size_t f = 0; f < n; ++f) {
        out[f] = 0;
        for (size_t i = 0; i < n; ++i) {
            float t = (float)i/n;
            out[f] += in[i] * cexp(2*I*pi*f*t);
        }
    }
}

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


int main(void) {
    
    pi = atan2f(1, 1) * 4; // fortran trickery
    
    size_t n = 8;
    float in[n];
    float complex out[n];

    // mix freqs
    for (size_t i = 0; i < n; ++i) {
         float t = (float)i/n;
         in[i] = cosf(2*pi*t) + sinf(2*pi*t*2);
    }

    fft(in, 1, out, n);

    for (size_t f = 0; f < n; ++f) {
        printf("%02zu: %.2f, %.2f\n", f, creal(out[f]), cimag(out[f]));
    }

#if 0
    for (size_t f = 0; f < n/2; ++f) {
        out[f] = 0;
        out[f + n/2] = 0;

        // even
        for (size_t i = 0; i < n; i += 2) {
            float t = (float)i/n;
            float complex v = in[i]*cexp(2*I*pi*t*f);
            out[f] += v;
            out[f + n/2] +=v;
        }

        //odd
        for (size_t i = 1; i < n; i += 2) {
            float t = (float)i/n;
            float complex v = in[i]*cexp(2*I*pi*t*f);
            out[f] += v;
            out[f + n/2] -=v;
        }

    }
#endif

}
