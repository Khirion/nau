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
    glm::vec3 root;
    glm::vec3 end;
    int mainIndex;
    int lastNode;

    float pi = (float)3.14159;
    float cplx;
    float killDistance;
    float attDistance;
    float growthLength;
    int cull;

public:
    branch() {};

    branch(float complexity, int _mainIndex, float _growthLength, int _cull) :
        tree(std::vector<node>()),
        charges(std::list<charge>()),
        middle(glm::vec3(0, 0, 0)),
        end(glm::vec3(0, 0, 0)),
        root(glm::vec3(0, 0, 0)),
        startDir(glm::vec3(0, 0, 0)),
        lastNode(0),
        mainIndex(_mainIndex),
        cplx(complexity * 0.875f),
        killDistance(_growthLength),
        attDistance(2.f * _growthLength),
        growthLength(static_cast<float>(_growthLength)),
        cull(_cull+1)
    {};

    ~branch() {};

    void init(int pInd, std::pair<glm::vec3, glm::vec3> waypoints, const std::vector<glm::vec3>& mainTree);
    void genCharges();
    void genCyl(glm::vec3 center, glm::mat3 transform, float height, float maxRad);
    float biModal();
    void grow();
    glm::vec3 randdir(glm::vec3 vec);
    bool updateAttractors();
    bool checkDeletion(const std::vector<node>& buffer);
    void checkTreeDel(const std::vector<glm::vec3>& mainTree);

    std::vector<node> getVector();
};