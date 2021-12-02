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
    glm::vec3 end;
    int mainIndex;

    float pi = (float)3.14159;
    float cplx;
    float killDistance;
    float attDistance;
    float growthLength;

public:
    branch() {};

    branch(int _mainIndex) :
        tree(std::vector<node>()),
        charges(std::list<charge>()),
        middle(glm::vec3(0, 0, 0)),
        mainIndex(_mainIndex),
        killDistance(40),
        attDistance(50),
        growthLength(static_cast<float>(30))
    {};

    ~branch() {};

    void init(int pInd, std::pair<glm::vec3, glm::vec3> waypoints, const std::vector<glm::vec3>& mainTree);
    void genCharges(glm::vec3 root, glm::vec3 waypoint);
    void genCone(glm::vec3 root, glm::mat3 transform, float height, float maxRad);
    void grow();
    glm::vec3 randdir(glm::vec3 vec);
    bool updateAttractors();
    bool checkDeletion(const std::vector<node>& buffer);
    void checkTreeDel(const std::vector<glm::vec3>& mainTree);

    std::vector<node> getVector();
};