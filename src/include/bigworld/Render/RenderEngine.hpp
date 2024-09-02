#pragma once

#include <vector>
#include <memory>

#include <SDL2/SDL.h>

#include "RenderObject.hpp"

namespace bigworld
{

class Application;

class Renderer
{

public:
    Renderer(std::shared_ptr<bigworld::Application> app);
    ~Renderer();

    void Step();
    void Shutdown();

    void AddObject(std::shared_ptr<RenderObject>* object);
    void RemoveObject(RenderObject* object);
    void ClearObjects();

    unsigned int CreateShaderProgram();

private:
    std::weak_ptr<bigworld::Application> m_app;
    SDL_GLContext m_gl;
    std::vector<RenderObject*> m_objects;
    bool m_running;

};

struct Color
{
    char r, g, b, a;
};

} // namespace bigworld