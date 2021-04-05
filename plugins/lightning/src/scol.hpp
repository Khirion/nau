#pragma once
#include <glm.hpp>
#include <glbinding/gl/gl.h>

#include <random>
#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>

using namespace std;

struct node{
    glm::vec3 parent;
    glm::vec3 pos;
    glm::vec3 dir;
    float growthLength;
    bool scan;
    bool grow;

    node(): // Root
        parent(glm::vec3(0,1500,0)),
        pos(glm::vec3(0,1500,0)),
        dir(glm::vec3(0,-1, 0)),
        growthLength(50),
        scan(true),
        grow(true) {};

    node(glm::vec3 _parent, glm::vec3 _pos, glm::vec3 _dir, bool _scan, bool _grow):
        parent(_parent),
        pos(_pos),
        dir(_dir),
        growthLength(50),
        scan(_scan),
        grow(_grow) {};

    ~node() {}

    bool operator==(const glm::vec3& p) const{
        return (all(equal(pos, p)));
    }
};

struct charge{
    glm::vec3 pos;
    glm::vec3 closestNode;
    float attStrength;
    float attDistance;
    float killDistance;

    charge():
        pos(glm::vec3(0,0,0)),
        closestNode(FLT_MAX, FLT_MAX, FLT_MAX),
        attStrength(2.0),
        attDistance(0.0),
        killDistance(50.0) {};

    charge(glm::vec3 _pos):
        pos(_pos),
        closestNode(FLT_MAX, FLT_MAX, FLT_MAX),
        attStrength(1.0),
        attDistance(500.0),
        killDistance(150.0) {};

    ~charge() {}

    bool operator==(const charge& c) const {
        return (all(equal(c.pos, pos)));
    }
};

class scol{
    private:
        vector<node> tree;
        list<charge> charges;
        struct charge floor;

    public:
        scol() {
            node root = node();
            tree = vector<node>(1,root);

            charges = list<charge>();

            default_random_engine generator;
            normal_distribution<float> disty(750, 300);
            normal_distribution<float> distxz(0, 75);


            auto rolly = bind(disty, generator);
            auto rollxz = bind(distxz, generator);

            for (int i = 0; i < 100; i++) {
                charges.push_back(charge(glm::vec3(rollxz(), rolly(), rollxz())));
            }

            for (int i = 0; i < 10; i++) {
                charges.push_back(charge(glm::vec3(rollxz(), rollxz() + 1350, rollxz())));
            }

            for (int i = 0; i < 10; i++) {
                charges.push_back(charge(glm::vec3(rollxz(), rollxz() + 250, rollxz())));
            }

            floor = charge();

            init();
        };

        vector<glm::vec3> getVertices();
        vector<unsigned int> getIndices();
        void init();
        bool grow();
        glm::vec3 randDir();
        void updateAttractors();
        void checkDeletion(const node& child);
        void print();
};