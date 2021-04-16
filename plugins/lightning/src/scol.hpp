#pragma once
#include <glm.hpp>
#include <../glm/gtx/rotate_vector.hpp>
#include <glbinding/gl/gl.h>


#include <random>
#include <functional>
#include <algorithm>
#include <list>
#include <vector>

struct node {
    glm::vec3 parent;
    glm::vec3 pos;
    glm::vec3 dir;
    bool scan;

    node() : // Root
        parent(glm::vec3(0, 1500, 0)),
        pos(glm::vec3(0, 1500, 0)),
        dir(glm::vec3(0, 0, 0)),
        scan(true)
    {};

    node(glm::vec3 _parent, glm::vec3 _pos) :
        parent(_parent),
        dir(0,0,0),
        pos(_pos),
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
    float attStrength;
    float attDistance;
    float killDistance;

    charge() : // Floor charge
        pos(glm::vec3(0, 0, 0)),
        closestNode(FLT_MAX, FLT_MAX, FLT_MAX),
        attStrength(0.5),
        attDistance(0.0),
        killDistance(0.0) {};

    charge(glm::vec3 _pos) :
        pos(_pos),
        closestNode(FLT_MAX, FLT_MAX, FLT_MAX),
        attStrength(2.0),
        attDistance(250.0),
        killDistance(125.0) {};

    ~charge() {}

    bool operator==(const charge& c) const {
        return (all(equal(c.pos, pos)));
    }
};

class scol {
private:
    std::vector<node> tree;
    std::list<charge> charges;
    struct charge floor;

public:
public:
    scol() {
        node root = node();
        tree = std::vector<node>(1, root);

        charges = std::list<charge>();

        std::default_random_engine generator;
        std::normal_distribution<float> disty(750, 275);
        std::normal_distribution<float> distxz(0, 200);


        auto rolly = bind(disty, generator);
        auto rollxz = bind(distxz, generator);

        for (int i = 0; i < 15; i++) {
            charges.push_back(charge(glm::vec3(rollxz(), rolly(), rollxz())));
        }

        for (int i = 0; i < 2; i++) {
            charges.push_back(charge(glm::vec3(rollxz(), rollxz() + 1350, rollxz())));
        }

        for (int i = 0; i < 3; i++) {
            charges.push_back(charge(glm::vec3(rollxz(), rollxz() + 250, rollxz())));
        }

        floor = charge();
    };

    std::vector<glm::vec3> getVertices();
    std::vector<unsigned int> getIndices();

    void grow();
    glm::vec3 randdir(glm::vec3 vec);
    void updateAttractors();
};