#include "scol.hpp"

float growthLength;
float killDistance;

void scol::init(float distYAvg, float distYDev, float distXZAvg, float distXZDev, float killDst, int chargeNum, int gLength){
    for (int i = 0; i < waypoints.size() - 1; i++) {
        node root = node(waypoints[i].pos);
        tree = std::vector<node>(1, root);

        charges = std::list<charge>();

        std::default_random_engine generator;
        std::normal_distribution<float> disty(distYAvg, distYDev);
        std::normal_distribution<float> distxz(distXZAvg, distXZDev);

        glm::vec3 middlePoint = glm::vec3((waypoints[i].pos + waypoints[i].pos)/2.0f);

        auto rolly = bind(disty, generator);
        auto rollxz = bind(distxz, generator);

        for (int i = 0; i < chargeNum; i++) {
            charges.push_back(charge(glm::vec3(middlePoint + rollxz(), middlePoint + rolly(), middlePoint + rollxz()), killDistance));
        }
        charges.push_back(waypoints[i+1]);

        growthLength = gLength;
        killDistance = killDst;
    
        grow();
    }
};

void scol::grow() {
    std::vector<node>::iterator n;
    std::list<charge>::iterator c;
    node curNode;
    bool end = false;

    while (!charges.empty()) {
        updateAttractors();

        for (const charge& c : charges) {
            n = find_if(tree.begin(), tree.end(), [c](node x) {return x == c.closestNode;});
            if (n != tree.end()) {
                n->scan = true;
                n->dir += glm::normalize((c.pos - n->pos) / glm::distance(c.pos, n->pos));
                n->dir = glm::normalize(n->dir);
            }
        }

        int curSize = tree.size();
        for (int i = 0; i < curSize; i++) {
            curNode = tree[i];
            if (curNode.scan) {
                node child = node(curNode.pos, curNode.pos + (curNode.dir * growthLength), curNode.dir);
                charges.remove_if([child](charge x) {return glm::distance(x.pos, child.pos) < x.kd; });
                tree.push_back(child);
            }
        }
    }
}

void scol::updateAttractors() {
    std::vector<node>::iterator n;
    std::list<charge>::iterator c;
    float dist;

    for (n = tree.begin(); n != tree.end(); n++) {
        if (n->scan) {
            n->scan = false;
            for (c = charges.begin(); c != charges.end(); c++) {
                dist = glm::distance(n->pos, c->pos);
                if (dist < glm::distance(c->closestNode, c->pos))
                    c->closestNode = n->pos;
            }
        }
    }
}

glm::vec3  scol::randdir(glm::vec3 vec) {
    // Introduce jaggedness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(0.5);

    if(d(gen))
        vec = glm::rotateX(vec, (d(gen) ? 16.f : -16.f));

    if(d(gen))
        vec = glm::rotateY(vec, (d(gen) ? 16.f : -16.f));

    return vec;
}

std::vector<glm::vec3> scol::getVertices() {
    std::vector<glm::vec3> vertices;
    for (node n : tree) {
        vertices.push_back(n.pos);
    }

    return vertices;
}

std::vector<unsigned int> scol::getIndices() {
    std::vector<node>::iterator it;
    std::vector<unsigned int> indices;

    for (int i = 1; i < tree.size(); i++) {
        it = find(tree.begin(), tree.end(), tree[i].parent);
        indices.push_back(it - tree.begin());
        indices.push_back(i);
    }

    return indices;
}