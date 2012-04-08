#ifndef _ARDUINO3D_UTILITY3D_HPP_
#define _ARDUINO3D_UTILITY3D_HPP_

#include "MatrixMxM.hpp"
namespace A3D
{

void Frustum(Array::Matrix4f& matrix, const float left, const float right, const float bottom, const float top, const float near, const float far);

void Diagonal(Array::Matrix4f& matrix, const float scale);

inline void Identity(Array::Matrix4f& matrix)
{
    Diagonal(matrix, 1.0f);
}

void RotateXrad(Array::Matrix4f& matrix, const float radians);
void RotateYrad(Array::Matrix4f& matrix, const float radians);
void RotateZrad(Array::Matrix4f& matrix, const float radians);

inline void RotateXdeg(Array::Matrix4f& matrix, const float degrees)
{
    RotateXrad(matrix, degrees * (M_PI/180.0f));
}

inline void RotateYdeg(Array::Matrix4f& matrix, const float degrees)
{
    RotateYrad(matrix, degrees * (M_PI/180.0f));
}

inline void RotateZdeg(Array::Matrix4f& matrix, const float degrees)
{
    RotateZrad(matrix, degrees * (M_PI/180.0f));
}

void Scale(Array::Matrix4f& matrix, const float scale_x, const float scale_y, const float scale_z);

inline void Scale(Array::Matrix4f& matrix, const float scale)
{
    Scale(matrix, scale, scale, scale);
}

void Translate(Array::Matrix4f& matrix, const float dx, const float dy, const float dz);

}
#endif
