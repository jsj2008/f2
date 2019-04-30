#include "core/app.h"
#include "please.h"

int main() {
    App app;
    if (!app.init())
        return 1;

    app.run();
    return 0;
}