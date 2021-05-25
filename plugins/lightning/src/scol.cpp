#include "scol.hpp"

float growthLength;
float killDistance;
float attDistance;

void scol::init(float distYAvg, float distYDev, float distXZAvg, float distXZDev, float killDst, float attDst, int chargeNum, int gLength, std::vector<glm::vec3> ways){
    growthLength = gLength;
    killDistance = killDst;
    attDistance = attDst;

    addWaypoints(ways);

    std::vector<node>::iterator n;

    tree = std::vector<node>(1, node(waypoints[0].pos, glm::normalize(waypoints[1].pos - waypoints[0].pos)));

    for (int i = 0; i < waypoints.size() - 1; i++) {
        charges = std::list<charge>();

        if (i) {
            charge w = waypoints[i];
            n = find_if(tree.begin(), tree.end(), [w](node n) {return glm::distance(w.pos, n.pos) < w.kd;});
            tree.push_back(node(n->pos, waypoints[i].pos, glm::normalize(waypoints[i+1].pos - waypoints[i].pos)));
        }

        glm::vec3 avg = (waypoints[i].pos + waypoints[i+1].pos)/2.0f;
        glm::vec3 dev = avg/4.0f;

        genCharges(avg, dev, distYAvg, distYDev, distXZAvg, distXZDev, chargeNum);
        charges.push_back(waypoints[i+1]);

        grow();
    }
};

void scol::addWaypoints(std::vector<glm::vec3> ways) {
    waypoints = std::vector<charge>();

    for (glm::vec3 way : ways) {
        waypoints.push_back(charge(way, growthLength));
    }
}

void scol::genCharges(glm::vec3 avg, glm::vec3 dev, float distYAvg, float distYDev, float distXZAvg, float distXZDev, int chargeNum) {
    // Random Engine
    std::default_random_engine generator;
    std::normal_distribution<float> disty((avg.y + distYAvg), (dev.y + distYDev));
    std::normal_distribution<float> distx((avg.x + distXZAvg), (dev.x + distXZDev));
    std::normal_distribution<float> distz((avg.z + distXZAvg), (dev.z + distXZDev));

    for (int i = 0; i < chargeNum; i++) {
        charges.push_back(charge(glm::vec3(distx(generator), disty(generator), distz(generator)), killDistance));
    }
}

void scol::grow() {
    std::vector<node>::iterator n;
    node curNode;
    bool end = false;

    // Initial Growth
    while (!updateAttractors()) {
        curNode = tree.back();
        node child = node(curNode.pos, curNode.pos + (curNode.dir * growthLength), glm::normalize(randdir(curNode.dir)));
        charges.remove_if([child](charge x) {return glm::distance(x.pos, child.pos) <= x.kd; });
        tree.push_back(child);
    }

    // Space Colonization Growth
    while (!charges.empty() && updateAttractors()) {
        for (const charge& charge : charges) {
            n = find_if(tree.begin(), tree.end(), [charge](node x) {return x == charge.closestNode;});
            if (n != tree.end()) {
                n->scan = true;
                float dt = glm::distance(charge.pos, n->pos);
                n->dir += normalize(charge.pos - n->pos)/dt*dt;
            }
        }

        for (n = tree.begin(); n != tree.end(); n++) {
            if (n->scan)
                n->dir = glm::normalize(n->dir);
        }

        size_t curSize = tree.size();
        for (int i = 0; i < curSize; i++) {
            curNode = tree[i];
            if (curNode.scan) {
                node child = node(curNode.pos, curNode.pos + (curNode.dir * growthLength), curNode.dir);
                charges.remove_if([child](charge x) {return glm::distance(x.pos, child.pos) <= x.kd; });
                tree.push_back(child);
            }
        }
    }
}

bool scol::updateAttractors() {
    std::vector<node>::iterator n;
    std::list<charge>::iterator c;
    float dist;

    for (n = tree.begin(); n != tree.end(); n++) {
        if (n->scan) {
            n->scan = false;
            for (c = charges.begin(); c != charges.end(); c++) {
                dist = glm::distance(n->pos, c->pos);
                if (dist <= attDistance && dist < glm::distance(c->closestNode, c->pos))
                    c->closestNode = n->pos;
            }
        }
    }

    c = find_if(charges.begin(), charges.end(), [](charge x) {return !(glm::all(glm::equal(x.closestNode, glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX)))); });

    if (c == charges.end())
        return false;
    else
        return true;
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