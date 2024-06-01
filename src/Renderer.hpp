#pragma once


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
};

} // namespace bigworld