#version 330 core
layout (location = 0) in vec3 aPos;

uniform float uRotationY;

void main()
{
    float angle = radians(uRotationY);
    float cosA = cos(angle);
    float sinA = sin(angle);

    // Apply rotation about the Y axis
    vec3 rotated;
    rotated.x = aPos.x * cosA + aPos.z * sinA;
    rotated.y = aPos.y;
    rotated.z = -aPos.x * sinA + aPos.z * cosA;

    gl_Position = vec4(rotated, 1.0);
}