#pragma once
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
        glm::vec3 startDir;
        glm::vec3 root;
        glm::vec3 end;
        glm::vec3 middle;
        int lastNode;
        int cull;

        float pi = (float) 3.14159;
        float cplx;
        float width;
        float killDistance;
        float attDistance;
        float growthLength;

        std::vector<glm::vec3> vertices;
        std::vector<std::pair<bool, unsigned int>> indices;

    public:
        mainBranch() {};

        mainBranch(float complexity, float _width, float _growthLength, int _cull) :
            tree(std::vector<node>()),
            charges(std::list<charge>()),
            middle(glm::vec3(0, 0, 0)),
            end(glm::vec3(0, 0, 0)),
            root(glm::vec3(0, 0, 0)),
            startDir(glm::vec3(0, 0, 0)),
            lastNode(0),
            width(_width),
            cplx(complexity * 0.875f),
            killDistance(_growthLength),
            attDistance(2.f * _growthLength),
            growthLength(static_cast<float>(_growthLength)),
            cull(_cull)
        {};

        ~mainBranch() {};

        void init(std::vector<glm::vec3> waypoints);
        void genCharges();
        void genRect(glm::vec3 center, glm::mat3 transform, float height, float maxRad);
        void genCyl(glm::vec3 center, glm::mat3 transform, float height, float maxRad);
        void genPyr(glm::vec3 center, glm::mat3 transform, float height, float side);
        void genCone(glm::vec3 center, glm::mat3 transform, float height, float maxRad);
        void grow();

        void makeMap();
        void makeIndexes();
        float biModal();

        glm::vec3 randdir(glm::vec3 vec);
        std::pair<int, glm::vec3> getClosest(glm::vec3 pos);
        bool updateAttractors();
        bool checkDeletion(const std::vector<node>& buffer);

        void addVector(std::vector<node> vector);
        int getSize();

        std::vector<glm::vec3> getVertices();
        unsigned int getIndiceSize();
        std::pair<std::vector<unsigned int>, std::vector<unsigned int>> getIndices(float size);
};