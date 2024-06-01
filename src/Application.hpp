#pragma once

#include <SDL2/SDL.h>
#include <cxxopts.hpp>
#include <string>

#include "Renderer.hpp"
#include "InputHandler.hpp"

namespace bigworld
{

class Application
{
public:
    Application(cxxopts::ParseResult arguments);
    ~Application();

    static void showMessage(std::string message, bool quiet = false, bool error = false);
    static void showError(std::string message, bool quiet = false) { showMessage(message, quiet, true); }

    void run();
    void shutdown(); // called from renderer to force a shutdown
    void cleanup(); // called from dtor and is more friendly than calling shutdown directly

    int statusCode() { return this->m_statusCode; }
    int width() { return this->m_width; }
    int height() { return this->m_height; }
    bool quiet() { return this->m_quiet; }
    bool running() { return this->m_running; }

private:
    void processArguments(int argc, char* argv[]);

    // member variables that are important
    int m_width;
    int m_height;
    bool m_quiet;
    bool m_running;
    int m_statusCode;

    // these three big machines form the core of bigworld
    // chronologically they are created during initialization in the order that they are listed here
    SDL_Window* window;
    InputHandler* inputHandler;
    Renderer* renderer;
};

} // namespace bigworld