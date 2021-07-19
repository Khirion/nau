#pragma once
#include <glm.hpp>
#include <../glm/gtx/rotate_vector.hpp>
#include <glbinding/gl/gl.h>

#define M_PI 3.14159

#include <random>
#include <functional>
#include <algorithm>
#include <list>
#include <vector>
#include <iostream>

struct node {
    int parentIndex;
    glm::vec3 pos;
    glm::vec3 dir;
    float attNum;

    node() : // Root
        parentIndex(0),
        pos(glm::vec3(0, 1500, 0)),
        dir(glm::vec3(0, -1, 0)),
        attNum(0)
    {};

    node(int _parent, glm::vec3 _pos, glm:: vec3 _dir) :
        parentIndex(_parent),
        pos(_pos),
        dir(_dir),
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

class scol {
private:
    std::vector<node> tree;
    std::list<charge> charges;

public:
    scol() :
        tree(std::vector<node>()),
        charges(std::list<charge>())
    {};
    std::vector<glm::vec3> getVertices();
    std::vector<unsigned int> getIndices();

    void init(float killDst, float attDst, int chargeNum, int sphereNum, int gLength, std::vector<glm::vec3> ways);
    void grow();
    void genCharges(glm::vec3 root, glm::vec3 way, int sphereNum, int chargeNum);
    void genPyr(glm::vec3 root, float height, float side, int chargeNum);
    void genCone(glm::vec3 root, glm::vec3 waypoint, float height, float maxRad, int chargeNum);
    glm::vec3 randdir(glm::vec3 vec);
    bool updateAttractors();
};