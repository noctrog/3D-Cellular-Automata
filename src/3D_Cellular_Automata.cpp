#include <iostream>
#include <App.h>

int main(int argc, char *argv[])
{
    gl3wInit();
    App *app = new App();
    app->run();
    delete app;
    return 0;
}
