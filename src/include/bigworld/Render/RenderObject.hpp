#pragma once

namespace bigworld
{

class RenderObject
{

public:
    RenderObject(unsigned int shaderProgram);
    ~RenderObject();

    void Draw();
    void DrawStep();

private:
    unsigned int m_shaderProgram;

}; // namespace bigworld

}