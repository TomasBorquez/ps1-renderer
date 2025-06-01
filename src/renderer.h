#pragma once

#include "base.h"

typedef struct {
  f64 x;
  f64 y;
} VecF2;

typedef struct {
  i32 x;
  i32 y;
} VecI2;

typedef struct {
  f64 x;
  f64 y;
  f64 z;
} VecF3;

typedef struct {
  i32 x;
  i32 y;
  i32 z;
} VecI3;

typedef struct {
  f64 x;
  f64 y;
  f64 z;
  f64 w;
} VecF4;

typedef struct {
  i32 x;
  i32 y;
  i32 z;
  i32 w;
} VecI4;

typedef struct {
  f32 a[3 * 3];
} Mat3;

typedef struct {
  f32 a[4 * 4];
} Mat4;

VEC_TYPE(VectorVecF2, VecF2);
VEC_TYPE(VectorVecF3, VecF3);

VEC_TYPE(VectorVecI2, VecI2);
VEC_TYPE(VectorVecI3, VecI3);

VEC_TYPE(VectorI8, i8);
VEC_TYPE(VectorI32, i32);
VEC_TYPE(VectorF32, f32);

typedef struct {
  u8 r;
  u8 g;
  u8 b;
  u8 a;
} Color;

#define BLACK (Color){10, 10, 10, 255}
#define WHITE (Color){240, 240, 240, 255}
#define GREEN (Color){0, 255, 0, 255}
#define RED (Color){255, 0, 0, 255}
#define BLUE (Color){64, 128, 255, 255}
#define YELLOW (Color){255, 200, 0, 255}
