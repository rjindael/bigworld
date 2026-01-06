#include "shambhala.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    Shambhala app = { 0 };

    if (!shambhala_init(&app, 800, 600, "shambhala")) {
        fprintf(stderr, "Failed to initialize shambhala\n");
        return 1;
    }

    printf("Started Shambhala v%s!\n", VERSION);

    shambhala_run(&app);
    shambhala_shutdown(&app);

    printf("Goodbye!\n");

    return 0;
}
