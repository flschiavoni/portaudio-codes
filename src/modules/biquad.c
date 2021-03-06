#include <math.h>
#include <stdlib.h>
#include "include/biquad.h"

biquad_t *create_biquad(int type, int order, int framesPerBuffer) {
  biquad_t *filter = malloc(sizeof(biquad_t));

  filter->type = type;
  filter->order = order;
  filter->framesPerBuffer = framesPerBuffer;
  filter->output = malloc(framesPerBuffer * sizeof(float));
  filter->xn1 = 0;
  filter->xn2 = 0;
  filter->yn1 = 0;
  filter->yn2 = 0;

  switch (type) {
    case 0:
      filter->process = allpass_process;
      break;
    case 1:
      filter->process = lowpass_process;
      break;
    case 2:
      filter->process = highpass_process;
      break;
    case 3:
      filter->process = bandpass_process;
      break;
    case 4:
      filter->process = bandreject_process;
      break;
  }

  return filter;
}

void allpass_process(biquad_t *filter) {
  if (filter->order == 1) { /* First-order allpass */
    float K = (float)tan(M_PI * (filter->cutOff) / (filter->sampleRate));
    float b0 = (K - 1) / (K + 1);
    float b1 = 1;
    float a1 = (K - 1) / (K + 1);

    int i = 0;
    for (i = 0; i < filter->framesPerBuffer; i++) {
      filter->output[i] =
          b0 * filter->input[i] + b1 * filter->xn1 - a1 * filter->yn1;
      filter->xn1 = filter->input[i];
      filter->yn1 = filter->output[i];
    }
  } else { /* Second-order allpass */
    float K = (float)tan(M_PI * (filter->cutOff) / (filter->sampleRate));
    float Q = filter->slope;
    float b0 = (K * K * Q - K + Q) / (K * K * Q + K + Q);
    float b1 = (2 * Q * (K * K - 1)) / (K * K * Q + K + Q);
    float b2 = 1;
    float a1 = (2 * Q * (K * K - 1)) / (K * K * Q + K + Q);
    float a2 = (K * K * Q - K + Q) / (K * K * Q + K + Q);

    int i = 0;
    for (i = 0; i < filter->framesPerBuffer; i++) {
      filter->output[i] = b0 * filter->input[i] + b1 * (filter->xn1) +
                          b2 * (filter->xn2) - a1 * (filter->yn1) -
                          a2 * (filter->yn2);
      filter->xn2 = filter->xn1;
      filter->yn2 = filter->yn1;
      filter->xn1 = filter->input[i];
      filter->yn1 = filter->output[i];
    }
  }
}

void lowpass_process(biquad_t *filter) {
  if (filter->order == 1) { /* First-order lowpass */

    float K = (float)tan(M_PI * filter->cutOff / filter->sampleRate);
    float b0 = 1 / (K + 1);
    float b1 = -1 / (K + 1);
    float a1 = (K - 1) / (K + 1);

    int i = 0;
    for (i = 0; i < filter->framesPerBuffer; i++) {
      filter->output[i] =
          b0 * filter->input[i] + b1 * filter->xn1 - a1 * filter->yn1;
      filter->xn1 = filter->input[i];
      filter->yn1 = filter->output[i];
    }
  } else { /* Second-order lowpass */

    float K = (float)tan(M_PI * filter->cutOff / filter->sampleRate);
    float Q = filter->slope;
    float b0 = Q / (K * K * Q + K + Q);
    float b1 = (-2 * Q) / (K * K * Q + K + Q);
    float b2 = Q;
    float a1 = (2 * Q * (K * K - 1)) / (K * K * Q + K + Q);
    float a2 = (K * K * Q - K + Q) / (K * K * Q + K + Q);

    int i = 0;
    for (i = 0; i < filter->framesPerBuffer; i++) {
      filter->output[i] = b0 * filter->input[i] + b1 * filter->xn1 +
                          b2 * filter->xn2 - a1 * filter->yn1 -
                          a2 * filter->yn2;
      filter->xn2 = filter->xn1;
      filter->yn2 = filter->yn1;
      filter->xn1 = filter->input[i];
      filter->yn1 = filter->output[i];
    }
  }
}

void highpass_process(biquad_t *filter) {
  if (filter->order == 1) { /* First-order highpass */
    float K = (float)tan(M_PI * filter->cutOff / filter->sampleRate);
    float b0 = 1 / (K + 1);
    float b1 = -1 / (K + 1);
    float a1 = (K - 1) / (K + 1);

    int i = 0;
    for (i = 0; i < filter->framesPerBuffer; i++) {
      filter->output[i] =
          b0 * filter->input[i] + b1 * filter->xn1 - a1 * filter->yn1;
      filter->xn1 = filter->input[i];
      filter->yn1 = filter->output[i];
    }
  } else { /* Second-order highpass */
    float K = (float)tan(M_PI * filter->cutOff / filter->sampleRate);
    float Q = filter->slope;
    float b0 = Q / (K * K * Q + K + Q);
    float b1 = (-2 * Q) / (K * K * Q + K + Q);
    float b2 = Q;
    float a1 = (2 * Q * (K * K - 1)) / (K * K * Q + K + Q);
    float a2 = (K * K * Q - K + Q) / (K * K * Q + K + Q);

    int i = 0;
    for (i = 0; i < filter->framesPerBuffer; i++) {
      filter->output[i] = b0 * filter->input[i] + b1 * filter->xn1 +
                          b2 * filter->xn2 - a1 * filter->yn1 -
                          a2 * filter->yn2;
      filter->xn2 = filter->xn1;
      filter->yn2 = filter->yn1;
      filter->xn1 = filter->input[i];
      filter->yn1 = filter->output[i];
    }
  }
}

void bandpass_process(biquad_t *filter) {
  /* Second-order bandpass */
  float K = (float)tan(M_PI * filter->cutOff / filter->sampleRate);
  float Q = filter->slope;
  float b0 = (K) / (K * K * Q + K + Q);
  float b1 = 0;
  float b2 = (K) / (K * K * Q + K + Q);
  float a1 = (2 * Q * (K * K - 1)) / (K * K * Q + K + Q);
  float a2 = (K * K * Q - K + Q) / (K * K * Q + K + Q);

  int i = 0;
  for (i = 0; i < filter->framesPerBuffer; i++) {
    filter->output[i] = b0 * filter->input[i] + b1 * filter->xn1 +
                        b2 * filter->xn2 - a1 * filter->yn1 - a2 * filter->yn2;
    filter->xn2 = filter->xn1;
    filter->yn2 = filter->yn1;
    filter->xn1 = filter->input[i];
    filter->yn1 = filter->output[i];
  }
}

void bandreject_process(biquad_t *filter) {
  /* Second-order bandreject */

  float K = (float)tan(M_PI * filter->sampleRate / filter->sampleRate);
  float Q = filter->slope;
  float b0 = (Q * (1 + K * K)) / (K * K * Q + K + Q);
  float b1 = (2 * Q * (K * K - 1)) / (K * K * Q + K + Q);
  float b2 = (Q * (1 + K * K)) / (K * K * Q + K + Q);
  float a1 = (2 * Q * (K * K - 1)) / (K * K * Q + K + Q);
  float a2 = (K * K * Q - K + Q) / (K * K * Q + K + Q);

  int i = 0;
  for (i = 0; i < filter->framesPerBuffer; i++) {
    filter->output[i] = b0 * filter->input[i] + b1 * filter->xn1 +
                        b2 * filter->xn2 - a1 * filter->yn1 - a2 * filter->yn2;
    filter->xn2 = filter->xn1;
    filter->yn2 = filter->yn1;
    filter->xn1 = filter->input[i];
    filter->yn1 = filter->output[i];
  }
}
