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
    void render(Camera &camera) {
        for (auto &t : _things)
            t->render(camera);
    }

    void add_thing(const std::shared_ptr<Thing> &t) {
        _things.push_back(t);
    }

private:
    std::vector<std::shared_ptr<Thing>> _things;

};


#endif //F2_DUP_SCENE_H
