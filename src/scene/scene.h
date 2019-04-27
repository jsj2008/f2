//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_SCENE_H
#define F2_DUP_SCENE_H

#include <vector>
#include "camera.h"
#include "thing.h"

class Scene {
public:
    ~Scene() {
        for (Thing *t : _things)
            delete t;
    }

    void render(Camera &camera) {
        for (Thing *t : _things)
            t->render(camera);
    }

    void add_thing(Thing *t) {
        _things.push_back(t);
    }

private:
    std::vector<Thing *> _things;

};


#endif //F2_DUP_SCENE_H
