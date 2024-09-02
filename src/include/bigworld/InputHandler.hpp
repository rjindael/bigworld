#pragma once

#include <SDL_keycode.h>

namespace bigworld
{

class Application;

class InputHandler
{
public:
    InputHandler(bigworld::Application* app);
    ~InputHandler();

    void processInputs();
    void shutdown();

private:
    bigworld::Application* app;
};

} // namespace bigworld