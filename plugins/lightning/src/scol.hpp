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

    node(glm::vec3 _pos) :
        parent(_pos),
        pos(_pos),
        dir(0,-1,0),
        scan(true)
    {};

    node(glm::vec3 _pos, glm::vec3 _dir) :
        parent(_pos),
        pos(_pos),
        dir(_dir),
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
    float kd;

    charge(glm::vec3 _pos, float _kd) :
        pos(_pos),
        closestNode(FLT_MAX, FLT_MAX, FLT_MAX),
        kd(_kd)
    {};

    ~charge() {}

    bool operator==(const charge& c) const {
        return (all(equal(c.pos, pos)));
    }
};

class scol {
private:
    std::vector<node> tree;
    std::list<charge> charges;
    std::vector<charge> waypoints;

public:
    scol() {};
    std::vector<glm::vec3> getVertices();
    std::vector<unsigned int> getIndices();

    void init(float distYAvg, float distYDev, float distXZAvg, float distXZDev, float killDst, float attDst, int chargeNum, int gLength, std::vector<glm::vec3> ways);
    void grow();
    void addWaypoints(std::vector<glm::vec3> ways);
    void genCharges(glm::vec3 avg, glm::vec3 dev, float distYAvg, float distYDev, float distXZAvg, float distXZDev, int chargeNum);
    glm::vec3 randdir(glm::vec3 vec);
    bool updateAttractors();
};