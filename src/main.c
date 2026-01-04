#include "shambhala.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    Shambhala app = { 0 };

    if (!shambhala_init(&app, 800, 600, "shambhala")) {
        fprintf(stderr, "Failed to initialize shambhala\n");
        return 1;
    }

    shambhala_run(&app);
    shambhala_shutdown(&app);

    return 0;
}
