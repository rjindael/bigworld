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