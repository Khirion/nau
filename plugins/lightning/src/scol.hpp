#pragma once
#include <glm.hpp>

struct node {
    int parentIndex;
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 weightDir;
    bool mainBranch;

    node() : // Root
        parentIndex(0),
        pos(glm::vec3(0, 0, 0)),
        dir(glm::vec3(0, 0, 0)),
        weightDir(glm::vec3(0, 0, 0)),
        mainBranch(false)
    {};

    node(int _parent, glm::vec3 _pos, glm::vec3 _weightDir, bool _mainBranch) :
        parentIndex(_parent),
        pos(_pos),
        dir(glm::vec3(0, 0, 0)),
        weightDir(_weightDir),
        mainBranch(_mainBranch)
    {};

    ~node() {}

    bool operator==(const glm::vec3& p) const {
        return (all(equal(pos, p)));
    }
};

struct charge {
    glm::vec3 pos;
    bool deletionFlag;

    charge() :
        pos(glm::vec3(0, 0, 0)),
        deletionFlag(false)
    {};

    charge(glm::vec3 _pos) :
        pos(_pos),
        deletionFlag(false)
    {};

    ~charge() {}

    bool operator==(const glm::vec3& p) const {
        return (all(equal(pos, p)));
    }
};