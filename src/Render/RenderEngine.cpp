#include <iostream>

#include "bigworld/Render/RenderEngine.hpp"
#include "bigworld/Application.hpp"

#include <glad/glad.h>

namespace bigworld
{

Renderer::Renderer(std::shared_ptr<bigworld::Application> app)
    : m_app(app)
    , m_running(false)
{
    m_gl = SDL_GL_CreateContext(app->window);

    // Use glad to load our OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        app->shutdown();
    }

    // Initialize viewport
    glViewport(0, 0, app->height(), app->width());
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // bkg. color

    // Enable VSync (cap at 60hz)
    SDL_GL_SetSwapInterval(1);

    m_running = true;
}

Renderer::~Renderer()
{
    Shutdown();
}

void Renderer::Step()
{
    if (!this->m_running)
        return;

    glClear(GL_COLOR_BUFFER_BIT);

    //====== Main render code ======
    for (auto object : m_objects)
    {
        object->DrawStep();
    }
    //==============================

    SDL_GL_SwapWindow(m_app.lock()->window);
}

void Renderer::Shutdown()
{
    this->m_running = false;
    SDL_GL_DeleteContext(m_gl);
}

void Renderer::AddObject(std::shared_ptr<RenderObject>* object)
{
    m_objects.push_back(object->get());
    object->get()->Draw();
}

void Renderer::RemoveObject(RenderObject* object)
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        if (*it == object)
        {
            m_objects.erase(it);
            return;
        }
    }
}

void Renderer::ClearObjects()
{
    m_objects.clear();
}

unsigned int Renderer::CreateShaderProgram()
{
    unsigned int shaderProgram;

    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

} // namespace bigworld