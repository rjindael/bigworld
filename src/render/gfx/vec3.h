#ifndef VEC3_H
#define VEC3_H

typedef struct Vec3 {
    float x;
    float y;
    float z;
} Vec3;

////////////////// OPERATIONS //////////////////

Vec3 vec3_add(const Vec3* a, const Vec3* b);
Vec3 vec3_subtract(const Vec3* a, const Vec3* b);
Vec3 vec3_scale(const Vec3* v, float scalar);
float vec3_dot(const Vec3* a, const Vec3* b);
Vec3 vec3_cross(const Vec3* a, const Vec3* b);
float vec3_length(const Vec3* v);
Vec3 vec3_normalize(const Vec3* v);

#endif // VEC3_H