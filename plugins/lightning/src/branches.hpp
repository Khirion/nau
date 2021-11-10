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
    glm::vec3 startDir;
    glm::vec3 middle;
    int mainIndex;

    float pi = (float)3.14159;
    float cplx;
    float killDistance;
    float attDistance;
    float growthLength;

public:
    branch() {};

    branch(int _mainIndex, float complexity, float _growthLength) :
        tree(std::vector<node>()),
        charges(std::list<charge>()),
        middle(glm::vec3(0, 0, 0)),
        mainIndex(_mainIndex),
        cplx(complexity),
        killDistance(_growthLength),
        attDistance(2.f * _growthLength),
        growthLength(static_cast<float>(_growthLength))
    {};

    ~branch() {};

    void init(int pInd, std::pair<glm::vec3, glm::vec3> waypoints, const std::vector<glm::vec3>& mainTree);
    void genCharges(glm::vec3 root, glm::vec3 waypoint);
    void genCone(glm::vec3 root, glm::mat3 transform, float height, float maxRad);
    void grow(glm::vec3 end);
    glm::vec3 randdir(glm::vec3 vec);
    bool updateAttractors();
    bool checkDeletion(glm::vec3 end);
    void checkTreeDel(const std::vector<glm::vec3>& mainTree);

    std::vector<node> getVector();
};