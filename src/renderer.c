#include "renderer.h"
#include <cglm.h>
#include <math.h>

Mat4 Mat4Create() {
  return (Mat4){.a = {
                    1.f,
                    0.f,
                    0.f,
                    0.f,
                    0.f,
                    1.f,
                    0.f,
                    0.f,
                    0.f,
                    0.f,
                    1.f,
                    0.f,
                    0.f,
                    0.f,
                    0.f,
                    1.f,
                }};
}

u16 Mat4Get(u16 x, u16 y) {
  return y * 4 + x;
}

Mat4 Mat4Mul(Mat4 a, Mat4 b) {
  Mat4 result = {0};
  for (u16 j = 0; j < 4; j++) {
    for (u16 i = 0; i < 4; i++) {
      result.a[Mat4Get(i, j)] = a.a[Mat4Get(i, 0)] * b.a[Mat4Get(0, j)] + a.a[Mat4Get(i, 1)] * b.a[Mat4Get(1, j)] + a.a[Mat4Get(i, 2)] * b.a[Mat4Get(2, j)] + a.a[Mat4Get(i, 3)] * b.a[Mat4Get(3, j)];
    }
  }
  return result;
}

void Mat4Set(Mat4 *mat, Mat4 o) {
  for (u16 j = 0; j < 4; j++) {
    for (u16 i = 0; i < 4; i++) {
      mat->a[Mat4Get(i, j)] = o.a[Mat4Get(i, j)];
    }
  }
}

Mat4 Mat4Translate(VecF3 v) {
  return (Mat4){.a = {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, v.x, v.y, v.z, 1.f}};
}

Mat4 Mat4Scale(VecF3 v) {
  return (Mat4){.a = {
                    v.x,
                    0.f,
                    0.f,
                    0.f,
                    0.f,
                    v.y,
                    0.f,
                    0.f,
                    0.f,
                    0.f,
                    v.z,
                    0.f,
                    0.f,
                    0.f,
                    0.f,
                    1.f,
                }};
}

// Mat4 mat4_rotX(f32 deg) {
//   f64 rad = deg * DEG_TO_RAD;
//   return (Mat4){.a = {
//                     1.f,
//                     0.f,
//                     0.f,
//                     0.f,
//                     0.f,
//                     cosf(rad),
//                     -sinf(rad),
//                     0.f,
//                     0.f,
//                     sinf(rad),
//                     cosf(rad),
//                     0.f,
//                     0.f,
//                     0.f,
//                     0.f,
//                     1.f,
//                 }};
// }

// Mat4 mat4_rotY(f32 deg) {
//   f64 rad = deg * DEG_TO_RAD;
//   return (Mat4){.a = {
//                     cosf(rad),
//                     0.f,
//                     -sinf(rad),
//                     0.f,
//                     0.f,
//                     1.f,
//                     0.f,
//                     0.f,
//                     sinf(rad),
//                     0.f,
//                     cosf(rad),
//                     0.f,
//                     0.f,
//                     0.f,
//                     0.f,
//                     1.f,
//                 }};
// }

// Mat4 mat4_rotZ(f32 deg) {
//   f64 rad = deg * DEG_TO_RAD;
//   return (Mat4){.a = {
//                     cosf(rad),
//                     -sinf(rad),
//                     0.f,
//                     0.f,
//                     sinf(rad),
//                     cosf(rad),
//                     0.f,
//                     0.f,
//                     0.f,
//                     0.f,
//                     1.f,
//                     0.f,
//                     0.f,
//                     0.f,
//                     0.f,
//                     1.f,
//                 }};
// }

Mat4 Mat4Transpose(Mat4 a) {
  Mat4 ret = {0};
  for (u16 j = 0; j < 4; j++) {
    for (u16 i = 0; i < 4; i++) {
      ret.a[Mat4Get(i, j)] = a.a[Mat4Get(j, i)];
    }
  }
  return ret;
}

// Mat4 Mat4Perspective(f32 fov, f32 aspect_ratio, f32 near, f32 far) {
//   f32 top = tanf(fov * DEG_TO_RAD / 2) * near;
//   f32 right = top * aspect_ratio;
//   f32 depth = far - near;
//   return (Mat4){.a = {
//                     1.f / right,
//                     0.f,
//                     0.f,
//                     0.f,
//                     0.f,
//                     1.f / top,
//                     0.f,
//                     0.f,
//                     0.f,
//                     0.f,
//                     -2.f / depth,
//                     0.f,
//                     0.f,
//                     0.f,
//                     -(far + near) / depth,
//                     1.f,
//                 }};
// }

VecF3 VecF3Add(VecF3 a, VecF3 b) {
  return (VecF3){.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}

VecF3 VecF3Sub(VecF3 a, VecF3 b) {
  return (VecF3){.x = a.x - b.x, .y = a.y - b.y, .z = a.z - b.z};
}

VecF3 VecF3Scale(VecF3 a, f32 s) {
  return (VecF3){.x = a.x * s, .y = a.y * s, .z = a.z * s};
}

VecF3 VecF3Cross(VecF3 a, VecF3 b) {
  return (VecF3){.x = a.y * b.z - b.y * a.z, .y = a.x * b.z - b.x * a.z, .z = a.x * b.y - a.y * b.x};
}

f64 VecF3Dot(VecF3 a, VecF3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

VecF3 VecF3Mul(VecF3 a, VecF3 b) {
  return (VecF3){.x = a.x * b.x, .y = a.y * b.y, .z = a.z * b.z};
}

VecF3 VecF3Normalize(VecF3 v) {
  f64 length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

  if (length < 1e-10) {
    return (VecF3){0, 0, 0};
  }

  return (VecF3){v.x / length, v.y / length, v.z / length};
}

f64 VecF3Norm(VecF3 v) {
  return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

VecF3 M2V(Mat4 m) {
  return (VecF3){m.a[Mat4Get(0, 0)] / m.a[Mat4Get(3, 0)], m.a[Mat4Get(1, 0)] / m.a[Mat4Get(3, 0)], m.a[Mat4Get(2, 0)] / m.a[Mat4Get(3, 0)]};
}

Mat4 V2M(VecF3 v) {
  Mat4 m = Mat4Create();
  m.a[Mat4Get(0, 0)] = v.x;
  m.a[Mat4Get(1, 0)] = v.y;
  m.a[Mat4Get(2, 0)] = v.z;
  m.a[Mat4Get(3, 0)] = 1.f;
  return m;
}
