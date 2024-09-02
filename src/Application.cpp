#include "bigworld/Application.hpp"

#include <iostream>
#include <glad/glad.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace bigworld
{

Application::Application(cxxopts::ParseResult arguments)
    : window(nullptr)
    , renderer(nullptr)
    , m_running(false)
    , m_statusCode(0)
{
    this->m_width = arguments["width"].as<int>();
    this->m_height = arguments["height"].as<int>();
    this->m_quiet = arguments["quiet"].as<bool>();
}

Application::~Application()
{
    this->cleanup();
}

void Application::showMessage(std::string message, bool quiet, bool error)
{
    if (quiet && !error) return;

    // hack: ideally, hwnd should be this->m_window hwnd if it exists, but it's too complicated to get hwnd
    // from sdl2 window, so it doesn't really matter (also this is static func anyway so)
#ifdef _WIN32
    MessageBox(nullptr, message.c_str(), "bigworld", error ? MB_ICONERROR : MB_ICONINFORMATION);
#else
    std::cout << message << std::endl;
#endif
}

// Called from the renderer to force a shutdown (independent of cleaning up GL context, which renderer handles itself)
void Application::shutdown(int statusCode)
{
    this->m_running = false;
    this->m_statusCode = statusCode;

    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void Application::cleanup()
{
    this->m_running = false;

    if (this->renderer != nullptr)
    {
        this->renderer->shutdown();
    }

    if (this->inputHandler != nullptr)
    {
        this->inputHandler->shutdown();
    }

    if (this->window != nullptr)
    {
        this->shutdown();
    }
}

void Application::run()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        this->showError("Failed to initialize SDL2: " + std::string(SDL_GetError()));
        return;
    }

    this->window = SDL_CreateWindow("bigworld", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->m_width, this->m_height, SDL_WINDOW_OPENGL);
    SDL_SetWindowResizable(window, SDL_TRUE);

    if (this->window == nullptr)
    {
        this->showError("Failed to create SDL window: " + std::string(SDL_GetError()));
        this->cleanup();
        return;
    }

    this->inputHandler = new InputHandler(this);
    this->renderer = new Renderer(this);

    this->m_running = true;
    this->m_statusCode = 0;

    std::once_flag drawTriangleFlag;

    while (this->m_running)
    {
        std::call_once(drawTriangleFlag, []() {
            this->renderer->AddObject(new Triangle());
        });

        this->inputHandler->processInputs();
        this->renderer->render();
    }

    this->cleanup();
}

/*

#include <SDL_keycode.h>
#include <iostream>

#include <glad/glad.h>
#include <SDL2/SDL.h>

int height = 800;
int width = 600;

bool isJustAllBlack = false;

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to initialize SDL2: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("bigworld", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, height, width, SDL_WINDOW_OPENGL);

    SDL_SetWindowResizable(window, SDL_TRUE);
    SDL_AddEventWatch([](void* userdata, SDL_Event* event) -> int {
        if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
            glViewport(0, 0, event->window.data1, event->window.data2);
        }
        return 0;
    }, nullptr);

    if (window == NULL)
    {
        std::cout << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, height, width);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    bool running = true;
    while (running)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);

        SDL_Event event;
        while (SDL_PollEvent(&event) && running)
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_x:
                    if (!isJustAllBlack)
                    {
                        isJustAllBlack = true;
                        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    break;

                case SDLK_ESCAPE:
                    running = false;
                    break;

                default:
                    break;
                }
            }
        }
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

*/

} // namespace bigworld