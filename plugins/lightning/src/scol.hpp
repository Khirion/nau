#pragma once
#include <glm.hpp>

struct node {
    int parentIndex;
    glm::vec3 pos;
    glm::vec3 dir;
    float attNum;

    node() : // Root
        parentIndex(0),
        pos(glm::vec3(0, 1500, 0)),
        dir(glm::vec3(0, 0, 0)),
        attNum(0)
    {};

    node(int _parent, glm::vec3 _pos, glm:: vec3 _dir) :
        parentIndex(_parent),
        pos(_pos),
        dir(_dir),
        attNum(1)
    {};

    ~node() {}

    bool operator==(const glm::vec3& p) const {
        return (all(equal(pos, p)));
    }
};

struct charge {
    glm::vec3 pos;
    int closestIndex;
    bool reached;

    charge() :
        pos(glm::vec3(0, 0, 0)),
        closestIndex(-1),
        reached(false)
    {};

    charge(glm::vec3 _pos) :
        pos(_pos),
        closestIndex(-1),
        reached(false)
    {};

    ~charge() {}

    bool operator==(const glm::vec3& p) const {
        return (all(equal(pos, p)));
    }
};