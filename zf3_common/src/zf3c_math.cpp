#include <zf3c_math.h>

namespace zf3 {

Matrix4x4 create_identity_matrix_4x4() {
    Matrix4x4 mat = {};
    mat[0][0] = 1.0f;
    mat[1][1] = 1.0f;
    mat[2][2] = 1.0f;
    mat[3][3] = 1.0f;
    return mat;
}

Matrix4x4 create_ortho_matrix_4x4(const float left, const float right, const float bottom, const float top, const float near, const float far) {
    Matrix4x4 mat = {};
    mat[0][0] = 2.0f / (right - left);
    mat[1][1] = 2.0f / (top - bottom);
    mat[2][2] = -2.0f / (far - near);
    mat[3][0] = -(right + left) / (right - left);
    mat[3][1] = -(top + bottom) / (top - bottom);
    mat[3][2] = -(far + near) / (far - near);
    mat[3][3] = 1.0f;
    return mat;
}

}
