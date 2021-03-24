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
        parent(glm::vec3(0,150,0)),
        pos(glm::vec3(0,150,0)),
        dir(glm::vec3(0,-1, 0)),
        growthLength(5),
        scan(true),
        grow(true) {};

    node(glm::vec3 _parent, glm::vec3 _pos, glm::vec3 _dir, bool _scan, bool _grow):
        parent(_parent),
        pos(_pos),
        dir(_dir),
        growthLength(1),
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
        attStrength(0.5),
        attDistance(0.0),
        killDistance(1.0) {};

    charge(glm::vec3 _pos):
        pos(_pos),
        closestNode(FLT_MAX, FLT_MAX, FLT_MAX),
        attStrength(1.0),
        attDistance(10.0),
        killDistance(1.0) {};

    ~charge() {}

    bool operator==(const charge& c) const {
        return (all(equal(c.pos, pos)));
    }
};

class Lightning{
    private:
        vector<node> tree;
        list<charge> charges;
        struct charge floor;

    public:
        Lightning() {
            node root = node();
            tree = vector<node>(1,root);

            charges = list<charge>();

            default_random_engine generator;
            normal_distribution<float> distribution(75.0, 25.0);
            normal_distribution<float> dist(2.5, 1.0);


            auto rolly = bind(distribution, generator);
            auto rollxz = bind(dist, generator);

            for (int i = 0; i < 100; i++) {
                charges.push_back(charge(glm::vec3(rollxz(), rolly(), rollxz())));
            }

            floor = charge();

            init();
        };
        
        void init();
        bool grow();
        glm::vec3 randDir();
        void updateAttractors();
        void draw(); // TODO
        void checkDeletion(const node& child);
        void print();
};