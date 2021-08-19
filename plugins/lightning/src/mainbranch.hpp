#pragma once
#include "scol.hpp"
#include <../glm/gtx/rotate_vector.hpp>
#include <../glm/gtc/random.hpp>
#include <../glm/gtx/vector_angle.hpp>
#include <glbinding/gl/gl.h>

#include <random>
#include <functional>
#include <algorithm>
#include <list>
#include <vector>
#include <iostream>

class mainBranch {
    private:
        std::vector<node> tree;
        std::list<charge> charges;
        glm::vec3 bottom;
        int lastNode;

        float pi = (float) 3.14159;
        float killDistance;
        float attDistance;
        int chargeNum;
        int weight;
        float growthChance;
        float growthLength;

    public:
        mainBranch() {};

        mainBranch(float _killDistance, float _attDistance, int _chargeNum, int _weight, float _growthChance, int _growthLength) :
            tree(std::vector<node>()),
            charges(std::list<charge>()),
            bottom(glm::vec3(0, 0, 0)),
            lastNode(0),
            killDistance(_killDistance * _growthLength),
            attDistance(_attDistance * _growthLength),
            chargeNum(_chargeNum),
            weight(_weight),
            growthChance(_growthChance),
            growthLength(_growthLength)
        {};

        ~mainBranch() {};


        std::vector<glm::vec3> getVertices();
        std::vector<unsigned int> getIndices();

        void init(std::vector<glm::vec3> waypoints, int genType, int width);
        void grow(glm::vec3 end);
        bool checkDeletion(glm::vec3 end);
        void genCharges(glm::vec3 root, glm::vec3 waypoint, int genType, int width);
        void genRect(glm::vec3 root, glm::mat3 transform, float height, float maxRad);
        void genCyl(glm::vec3 root, glm::mat3 transform, float height, float maxRad);
        void genPyr(glm::vec3 root, glm::mat3 transform, float height, float side);
        void genCone(glm::vec3 center, glm::mat3 transform, float height, float maxRad);
        void genSphere(glm::vec3 root, glm::mat3 transform, float maxRad);
        glm::vec3 randdir(glm::vec3 vec);
        bool updateAttractors();
};