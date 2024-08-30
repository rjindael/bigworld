#include "InputHandler.hpp"
#include <SDL2/SDL.h>
#include "Application.hpp"

namespace bigworld
{

InputHandler::InputHandler(bigworld::Application* app) : app(app)
{
    //
}

InputHandler::~InputHandler()
{
    //
}

void InputHandler::processInputs()
{
    SDL_Event event;
    while (SDL_PollEvent(&event) && this->app->running())
    {
        if (event.type == SDL_QUIT)
        {
            this->app->cleanup();
        }
    }
}

void InputHandler::shutdown()
{
    //
}

} // namespace bigworld