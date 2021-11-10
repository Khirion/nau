#pragma once
#include <glm.hpp>

struct node {
    int parentIndex;
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 startDir;
    bool mainBranch;
    unsigned int attNum;

    node() : // Root
        parentIndex(0),
        pos(glm::vec3(0, 1500, 0)),
        dir(glm::vec3(0, 0, 0)),
        startDir(glm::vec3(0, 0, 0)),
        mainBranch(false),    
        attNum(0)
    {};

    node(int _parent, glm::vec3 _pos, glm::vec3 _startDir, bool _mainBranch) :
        parentIndex(_parent),
        pos(_pos),
        dir(glm::vec3(0, 0, 0)),
        startDir(_startDir),
        mainBranch(_mainBranch),
        attNum(0)
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