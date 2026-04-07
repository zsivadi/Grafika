#include "app.h"

#include <stdio.h>
#include <time.h>


int main(int argc, char* argv[]) {

    FILE *f = fopen("debug.log", "w");
    fprintf(f, "Program started!\n");
    fclose(f);

    srand(time(NULL));
    
    App app;

    init_app(&app, 1920, 1080);
    while (app.is_running) {
        handle_app_events(&app);
        update_app(&app);
        render_app(&app);
    }
    destroy_app(&app);

    return 0;
}
