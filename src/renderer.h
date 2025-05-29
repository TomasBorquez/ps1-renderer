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

// TODO: Change to u8
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

Mat3 Mat3Create();
u16 Mat3Get(u16 x, u16 y);
void Mat3Set(Mat3 *mat, Mat3 o);
Mat3 Mat3Mul(Mat3 a, Mat3 b);
Mat3 Mat3Translate(VecF2 v);

Mat4 Mat4Create();
u16 Mat4Get(u16 x, u16 y);
Mat4 Mat4Mul(Mat4 a, Mat4 b);
Mat4 Mat4Translate(VecF3 v);
Mat4 Mat4Scale(VecF3 v);
Mat4 Mat4Transpose(Mat4 a);

VecF3 Vec3Add(VecF3 a, VecF3 b);
VecF3 Vec3Sub(VecF3 a, VecF3 b);
VecF3 Vec3Mul(VecF3 a, VecF3 b);
VecF3 Vec3Scale(VecF3 a, f32 s);
VecF3 Vec3Cross(VecF3 a, VecF3 b);
f64 Vec3Dot(VecF3 a, VecF3 b);
VecF3 Vec3Normalize(VecF3 v);
f64 Vec3Norm(VecF3 v);

VecF3 M2V(Mat4 m);
Mat4 V2M(VecF3 v);
