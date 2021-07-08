#pragma once
#include <glm.hpp>
#include <../glm/gtx/rotate_vector.hpp>
#include <glbinding/gl/gl.h>


#include <random>
#include <functional>
#include <algorithm>
#include <list>
#include <vector>

#include <iostream>

struct node {
    glm::vec3 parent;
    glm::vec3 pos;
    glm::vec3 dir;
    bool scan;

    node() : // Root
        parent(glm::vec3(0, 1500, 0)),
        pos(glm::vec3(0, 1500, 0)),
        dir(glm::vec3(0, -1, 0)),
        scan(true)
    {};

    node(glm::vec3 _parent, glm::vec3 _pos, glm:: vec3 _dir) :
        parent(_parent),
        pos(_pos),
        dir(_dir),
        scan(true)
    {};

    ~node() {}

    bool operator==(const glm::vec3& p) const {
        return (all(equal(pos, p)));
    }
};

struct charge {
    glm::vec3 pos;
    glm::vec3 closestNode;

    charge() :
        pos(glm::vec3(0, 0, 0)),
        closestNode(glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX))
    {};

    charge(glm::vec3 _pos) :
        pos(_pos),
        closestNode(glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX))
    {};

    ~charge() {}

    bool operator==(const glm::vec3& p) const {
        return (all(equal(pos, p)));
    }
};

class scol {
private:
    std::vector<node> tree;
    std::list<charge> charges;

public:
    scol() {};
    std::vector<glm::vec3> getVertices();
    std::vector<unsigned int> getIndices();

    void init(float killDst, float attDst, int chargeNum, int sphereNum, int gLength, std::vector<glm::vec3> ways);
    void grow();
    void genCharges(glm::vec3 root, glm::vec3 way, int sphereNum, int chargeNum);
    glm::vec3 randdir(glm::vec3 vec);
    bool updateAttractors();
};