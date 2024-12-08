#ifndef ZF3_COMMON_H
#define ZF3_COMMON_H

#include <math.h>

//
// Math
//
#define ZF3_PI 3.14159265358979323846f

typedef struct {
    float x;
    float y;
} ZF3Vec2D;

typedef struct {
    int x;
    int y;
} ZF3Vec2DInt;

typedef struct {
    float elems[4][4];
} ZF3Matrix4x4;

void zf3_init_identity_matrix_4x4(ZF3Matrix4x4* const mat);
void zf3_init_ortho_matrix_4x4(ZF3Matrix4x4* const mat, const float left, const float right, const float bottom, const float top, const float near, const float far);

#endif
