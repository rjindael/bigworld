#include "Renderer.hpp"
#include "Application.hpp"

#include <glad/glad.h>

namespace bigworld
{

Renderer::Renderer(bigworld::Application* app) : app(app)
{
    this->m_running = true;
    this->glContext = SDL_GL_CreateContext(app->window);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        app->shutdown();
    }

    glViewport(0, 0, app->height(), app->width());
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    SDL_GL_SetSwapInterval(1);
}

Renderer::~Renderer()
{
    this->shutdown();
}

void Renderer::render()
{
    if (!this->m_running) return;
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(app->window);
}

void Renderer::shutdown()
{
    this->m_running = false;
    SDL_GL_DeleteContext(glContext);
}

} // namespace bigworld