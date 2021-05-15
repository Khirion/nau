#include "scol.hpp"

float growthLength;
float killDistance;
float attDistance;

void scol::init(float distYAvg, float distYDev, float distXZAvg, float distXZDev, float killDst, float attDst, int chargeNum, int gLength, std::vector<glm::vec3> ways){
    growthLength = gLength;
    killDistance = killDst;
    attDistance = attDst;

    waypoints = std::vector<charge>();

    for (glm::vec3 way : ways) {
        waypoints.push_back(charge(way, growthLength));
    }

    std::vector<node>::iterator n;
    charge w;

    tree = std::vector<node>(1, node(waypoints[0].pos, glm::normalize(waypoints[1].pos - waypoints[0].pos)));

    for (int i = 0; i < waypoints.size() - 1; i++) {
        charges = std::list<charge>();

        if (i) {
            w = waypoints[i];
            n = find_if(tree.begin(), tree.end(), [w](node n) {return glm::distance(w.pos, n.pos) < w.kd;});
            tree.push_back(node(n->pos, waypoints[0].pos, glm::normalize(waypoints[1].pos - waypoints[0].pos)));
        }

        glm::vec3 avg = (waypoints[i].pos + waypoints[i+1].pos)/2.0f;
        glm::vec3 dev = avg/2.0f;

        // Random Engine
        std::default_random_engine generator;
        std::normal_distribution<float> disty(avg.y + distYAvg, dev.y + distYDev);
        std::normal_distribution<float> distx(avg.x + distXZAvg, dev.x + distXZDev);
        std::normal_distribution<float> distz(avg.z + distXZAvg, dev.z + distXZDev);
        auto rolly = bind(disty, generator);
        auto rollx = bind(distx, generator);
        auto rollz = bind(distz, generator);

        for (int i = 0; i < chargeNum; i++) {
            charges.push_back(charge(glm::vec3(rollx(), rolly(), rollz()), killDistance));
        }
        charges.push_back(waypoints[i+1]);

        grow();
    }
};

void scol::grow() {
    std::vector<node>::iterator n;
    std::list<charge>::iterator c;
    node curNode;
    bool end = false;

    updateAttractors();
    /*c = find_if(charges.begin(), charges.end(), [](charge x) {return !(glm::all(glm::equal(x.closestNode, glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX)))); });

    // Initial Growth
    while (c == charges.end()) {
        curNode = tree.back();
        node child = node(curNode.pos, curNode.pos + (curNode.dir * growthLength), glm::normalize(randdir(curNode.dir)));
        charges.remove_if([child](charge x) {return glm::distance(x.pos, child.pos) < x.kd; });
        tree.push_back(child);

        updateAttractors();
        c = find_if(charges.begin(), charges.end(), [](charge x) {return !(glm::all(glm::equal(x.closestNode, glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX)))); });
    }*/

    // Space Colonization Growth
    while (!charges.empty() && c != charges.end()) {
        for (const charge& charge : charges) {
            n = find_if(tree.begin(), tree.end(), [charge](node x) {return x == charge.closestNode;});
            if (n != tree.end()) {
                n->scan = true;
                n->dir += glm::normalize((charge.pos - n->pos) / glm::distance(charge.pos, n->pos));
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
        updateAttractors();
        c = find_if(charges.begin(), charges.end(), [](charge x) {return !(glm::all(glm::equal(x.closestNode, glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX)))); });
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
                if (dist < glm::distance(c->closestNode, c->pos) && dist <= attDistance)
                    c->closestNode = n->pos;
            }
        }
    }
}

glm::vec3 scol::randdir(glm::vec3 vec) {
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