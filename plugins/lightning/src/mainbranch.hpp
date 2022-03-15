#pragma once

#define GLM_FORCE_RADIANS

#include "scol.hpp"
#include <glbinding/gl/gl.h>
#include <../glm/gtx/rotate_vector.hpp>
#include <../glm/gtx/vector_angle.hpp>

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
        std::vector<std::pair<int, glm::vec3>> branchNodes;

        float pi = (float) 3.14159;
        float cplx;
        float width;
        float weight;
        float killDistance;
        float attDistance;
        float growthLength;

        std::vector<glm::vec3> vertices;
        std::vector<std::pair<bool, unsigned int>> indices;

    public:
        mainBranch() {};

        mainBranch(float complexity, float _width, float _weight, float _growthLength) :
            tree(std::vector<node>()),
            charges(std::list<charge>()),
            end(glm::vec3(0, 0, 0)),
            root(glm::vec3(0, 0, 0)),
            branchNodes(std::vector<std::pair<int, glm::vec3>>()),
            width(_width),
            weight(_weight),
            cplx(complexity * 0.2f),
            killDistance(_growthLength),
            attDistance(2*_growthLength),
            growthLength(static_cast<float>(_growthLength))
        {};

        ~mainBranch() {};

        void init(std::vector<glm::vec3> waypoints, int gen);
        void genCharges(int gen);
        void genCyl(glm::vec3 center, glm::mat3 transform, float height, float maxRad);
        void genCone(glm::vec3 center, glm::mat3 transform, float height, float maxRad);
        void genSphere(glm::vec3 center, glm::mat3 transform, float height);
        void grow();

        void noOrder();
        void makeMap();
        void makeIndexes();
        float biModal();

        glm::vec3 randdir(glm::vec3 vec);
        std::pair<int, glm::vec3> getClosest(glm::vec3 pos);
        bool updateAttractors();
        bool checkDeletion(const node& n);

        void addVector(std::vector<node> vector);
        int getSize();

        std::vector<std::pair<int, glm::vec3>> getBranchNodes();
        std::vector<glm::vec3> getVertices();
        unsigned int getIndiceSize();
        std::pair<std::vector<unsigned int>, std::vector<unsigned int>> getIndices(float size);
};