#pragma once

#include <SDL2/SDL.h>
#include <iostream>

namespace bigworld
{

class Application;

class Renderer
{
public:
    Renderer(bigworld::Application* app);
    ~Renderer();

    void render();
    void shutdown();

private:
    bigworld::Application* app;
    SDL_GLContext glContext;
    bool m_running;
    void compileShaders();
    void drawTriangle();
};

} // namespace bigworld