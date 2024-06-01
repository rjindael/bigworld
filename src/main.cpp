#include "Application.hpp"

// these are strings because cxxopts sucks
const char* kWidth = "800";
const char* kHeight = "600";

int main(int argc, char* argv[])
{
    cxxopts::Options options("bigworld", "OpenGL experiments");
    options.add_options()
        ("x,width", "Initial width of the window", cxxopts::value<int>()->default_value(kWidth))
        ("y,height", "Initial height of the window", cxxopts::value<int>()->default_value(kHeight))
        ("q,quiet", "Hide extraneous information")
        ("h,help", "Print usage");

    auto arguments = options.parse(argc, argv);

    if (arguments.count("help"))
    {
        bigworld::Application::showMessage(options.help());
        return 0;
    }

    bigworld::Application my_world(options.parse(argc, argv));
    my_world.run();

    return my_world.statusCode();
}