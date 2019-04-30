//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_APP_H
#define F2_DUP_APP_H

#include "input.h"
#include "sim.h"

class App {
public:
    bool init();

    void run();
    void terminate();

private:
    View _view;

    bool _running;

    void update();
    void render();

    std::unique_ptr<Sim> _sim;
};


#endif //F2_DUP_APP_H
