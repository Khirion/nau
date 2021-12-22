#pragma once

#define GLM_FORCE_RADIANS

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
    glm::vec3 root;
    glm::vec3 end;
    std::vector<std::pair<int, glm::vec3>> branchNodes;
    int mainIndex;

    float pi = (float)3.14159;
    float cplx;
    float weight;
    float width;
    float killDistance;
    float attDistance;
    float growthLength;

public:
    branch() {};

    branch(float complexity, int _mainIndex, float _weight, float _width, float _growthLength, int _degree) :
        tree(std::vector<node>()),
        charges(std::list<charge>()),
        end(glm::vec3(0, 0, 0)),
        root(glm::vec3(0, 0, 0)),
        branchNodes(std::vector<std::pair<int, glm::vec3>>()),
        mainIndex(_mainIndex),
        weight(_weight),
        width(_width),
        cplx(complexity * (0.75f/(_degree*2.f))),
        killDistance(_growthLength),
        attDistance(2.f * _growthLength),
        growthLength(static_cast<float>(_growthLength))
    {};

    ~branch() {};

    void init(int pInd, glm::vec3 _start, glm::vec3 _end, const std::vector<glm::vec3>& mainTree, int gen);
    void genCharges(int gen);
    void genCyl(glm::vec3 center, glm::mat3 transform, float height, float maxRad);
    void genCone(glm::vec3 center, glm::mat3 transform, float height, float maxRad);
    void genSphere(glm::vec3 center, glm::mat3 transform, float height);
    float biModal();
    void grow();
    glm::vec3 randdir(glm::vec3 vec);
    bool updateAttractors();
    bool checkDeletion(const node& n);
    void checkTreeDel(const std::vector<glm::vec3>& mainTree);

    std::vector<std::pair<int, glm::vec3>> getBranchNodes();
    std::vector<node> getVector();
};