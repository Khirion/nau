#pragma once
#include "scol.hpp"
#include <glbinding/gl/gl.h>
#include <../glm/gtx/rotate_vector.hpp>

#include <random>
#include <map>
#include <list>
#include <vector>

class mainBranch {
    private:
        std::vector<node> tree;
        std::list<charge> charges;
        std::map<int, std::vector<int>> parIndMap;
        glm::vec3 root;
        glm::vec3 end;
        glm::vec3 middle;
        int lastNode;

        float pi = (float) 3.14159;
        float killDistance;
        float attDistance;
        int chargeNum;
        int weight;
        float growthChance;
        float growthLength;

        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> indices;

    public:
        mainBranch() {};

        mainBranch(float complexity, int _chargeNum, int _weight, float _growthChance, int _growthLength) :
            tree(std::vector<node>()),
            charges(std::list<charge>()),
            middle(glm::vec3(0, 0, 0)),
            lastNode(0),
            killDistance((2.0f - complexity) * _growthLength),
            attDistance(2.0f * _growthLength),
            chargeNum(_chargeNum),
            weight(_weight),
            growthChance(_growthChance),
            growthLength(static_cast<float>(_growthLength))
        {};

        ~mainBranch() {};

        void init(std::vector<glm::vec3> waypoints, float width);
        void genCharges(float width);
        void genRect(glm::vec3 center, glm::mat3 transform, float height, float maxRad);
        void genCyl(glm::vec3 center, glm::mat3 transform, float height, float maxRad);
        void genPyr(glm::vec3 center, glm::mat3 transform, float height, float side);
        void genCone(glm::vec3 center, glm::mat3 transform, float height, float maxRad);
        void grow();

        void makeMap();
        void makeIndexes();

        glm::vec3 randdir(glm::vec3 vec);
        std::pair<int, glm::vec3> getClosest(glm::vec3 pos);
        bool updateAttractors();
        bool checkDeletion();

        void addVector(std::vector<node> vector);
        int getSize();

        std::vector<glm::vec3> getVertices();
        std::vector<unsigned int> getIndices();
};