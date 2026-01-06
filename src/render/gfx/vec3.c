#include "vec3.h"

#include <math.h>

// add two vec3s together
Vec3 vec3_add(const Vec3* a, const Vec3* b)
{
    Vec3 result = { a->x + b->x, a->y + b->y, a->z + b->z };

    return result;
}

// subtract vec3 b from vec3 a
Vec3 vec3_subtract(const Vec3* a, const Vec3* b)
{
    Vec3 result = { a->x - b->x, a->y - b->y, a->z - b->z };

    return result;
}

// scale a vec3 by a scalar value
Vec3 vec3_scale(const Vec3* v, float scalar)
{
    Vec3 result = { v->x * scalar, v->y * scalar, v->z * scalar };

    return result;
}

// compute the dot product of two vec3s
float vec3_dot(const Vec3* a, const Vec3* b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

// compute the cross product of two vec3s
Vec3 vec3_cross(const Vec3* a, const Vec3* b)
{
    Vec3 result = { (a->y * b->z) - (a->z * b->y), (a->z * b->x) - (a->x * b->z), (a->x * b->y) - (a->y * b->x) };

    return result;
}

// compute the length (magnitude) of a vec3
// i.e. compute it as a scalar value
float vec3_length(const Vec3* v)
{
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

// normalize a vec3 to have a length of 1
Vec3 vec3_normalize(const Vec3* v)
{
    float length = vec3_length(v);
    if (length == 0.0f) {
        return (Vec3){ 0.0f, 0.0f, 0.0f }; // return zero vector if length is zero
    }

    Vec3 result = { v->x / length, v->y / length, v->z / length };

    return result;
}