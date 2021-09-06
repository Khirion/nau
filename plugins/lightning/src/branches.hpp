#pragma once
#include "scol.hpp"
#include <glbinding/gl/gl.h>
#include <../glm/gtx/rotate_vector.hpp>

#include <random>
#include <list>
#include <vector>

class branch {
private:
    std::vector<node> tree;
    std::list<charge> charges;
    glm::vec3 middle;
    int mainIndex;

    float pi = (float)3.14159;
    float killDistance;
    float attDistance;
    int chargeNum;
    float growthChance;
    float growthLength;

public:
    branch() {};

    branch(int _mainIndex, float complexity, int _chargeNum, float _growthChance, int _growthLength) :
        tree(std::vector<node>()),
        charges(std::list<charge>()),
        middle(glm::vec3(0, 0, 0)),
        mainIndex(_mainIndex),
        killDistance((2.0f - complexity) * _growthLength),
        attDistance(2.0f * _growthLength),
        chargeNum(_chargeNum),
        growthChance(_growthChance),
        growthLength(_growthLength)
    {};

    ~branch() {};

    void init(std::pair<glm::vec3, glm::vec3> waypoints, const std::vector<glm::vec3>& mainTree);
    void genCharges(glm::vec3 root, glm::vec3 waypoint);
    void genCone(glm::vec3 root, glm::mat3 transform, float height, float maxRad);
    void grow(glm::vec3 end);
    glm::vec3 randdir(glm::vec3 vec);
    bool updateAttractors();
    bool checkDeletion(glm::vec3 end);
    void checkTreeDel(const std::vector<glm::vec3>& mainTree);

    std::vector<node> getVector();
};