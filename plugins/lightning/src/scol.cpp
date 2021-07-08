#include "scol.hpp"

float killDistance;
float attDistance;
float growthLength;

void scol::init(float killDst, float attDst, int chargeNum, int sphereNum, int gLength, std::vector<glm::vec3> waypoints){
    killDistance = killDst;
    attDistance = attDst;
    growthLength = gLength;

    /* for (int i = 0; i < waypoints.size() - 1; i++) {
       if (i) {
            glm::vec3 w = waypoints[i];
            // Need to fix this -- Need to check the CLOSEST to the waypoint
            n = find_if(tree.begin(), tree.end(), [w](node n) {return glm::distance(w, n.pos) < killDistance;});
            tree.push_back(node(n->pos, waypoints[i], glm::normalize(waypoints[i+1] - waypoints[i])));
        }*/

    tree.push_back(node(0, waypoints[0], glm::normalize(waypoints[1] - waypoints[0])));
    charges.push_back(waypoints[1]);
    
    genCharges(waypoints[0], waypoints[1], sphereNum, chargeNum);

    grow();
}

void scol::genCharges(glm::vec3 root, glm::vec3 waypoint, int sphereNum, int chargeNum) {
    // Random Engine - TBD
    // Divide the length between waypoints according to the amount of spheres to create, randomly assign spheres to left/right, front/back as per the radius defined by the length/2 of each segment
    float distance = glm::distance(root, waypoint);

    std::default_random_engine generator;
    std::uniform_real_distribution<float> randY(0.0, distance-5);
    std::uniform_real_distribution<float> randXZ(-distance/4, distance/4);
    auto genY = bind(randY, generator);
    auto genXZ = bind(randXZ, generator);

    for (int i = 0; i < chargeNum; i++) {
        charges.push_back(glm::vec3(genXZ(), genY(), genXZ()));
    }

}

/* Sphere beta
    float center = 0;
    sphereNum += 2;
    float distance = glm::distance(root, waypoint) / sphereNum;
    float length = distance / 4;

    /*std::default_random_engine generator;
    std::uniform_real_distribution<float> randDistance(0.0, distance);

    std::uniform_real_distribution<float> randRadius(-length, length);

    auto genDist = bind(randDistance,generator);
    auto genRadius = bind(randRadius,generator);*/



    /*for (int i = 0; i < (chargeNum / sphereNum); i++) {
        charges.push_back(charge(glm::vec3(waypoint.x + genRadius(), waypoint.y - abs(genRadius()), waypoint.z + genRadius()), killDistance));
        charges.push_back(charge(glm::vec3(root.x + genRadius(), root.y + abs(genRadius()), root.z + genRadius()), killDistance));
    }

    for (int i = 2; i < sphereNum; i++) {
        center = (distance * i) + genDist();
        glm::vec3 sphere = glm::vec3(genRadius(), center, genRadius());
        for (int j = 0; j < chargeNum / sphereNum; j++) {
            charges.push_back(charge(glm::vec3(sphere.x + genRadius(), sphere.y + genRadius(), sphere.z + genRadius()), killDistance));
        }
    }*/

void scol::grow() {
    std::vector<node>::iterator n;
    std::vector<node> buffer;

    // Initial Growth
   while (!updateAttractors() && !charges.empty()) {
        const node& curNode = tree.back();
        node child = node(tree.size(), curNode.pos + (curNode.dir * growthLength), curNode.dir);
        tree.push_back(child);
    }

    // Space Colonization Growth
    while (updateAttractors()) {
        for (const charge& charge : charges) {
            if (charge.closestIndex != -1){
                n = tree.begin() + charge.closestIndex;
                n->scan = true;
                n->dir += 1.f/(charge.pos - n->pos);
            }
        }

        int i = 0;

        for (n = tree.begin(); n != tree.end(); n++, i++) {
            if (n->scan) {
                n->dir = glm::normalize(n->dir);
                node child = node(i, n->pos + (n->dir * growthLength), n->dir);
                buffer.push_back(child);
            }
        }
        tree.insert(tree.end(), buffer.begin(), buffer.end());
        buffer.clear();
        charges.remove_if([](charge x) {return x.reached; });
    }
}

bool scol::updateAttractors() {
    std::vector<node>::iterator n;
    std::list<charge>::iterator c;
    bool flag = false;
    int i = 0;
    float dist;

    for (n = tree.begin(); n != tree.end(); n++, i++) {
        n->scan = false;
        for (c = charges.begin(); c != charges.end(); c++) {
            dist = glm::distance(n->pos, c->pos);
            if (dist <= attDistance) {
                if (dist <= killDistance)
                    c->reached = true;

                if (c->closestIndex == -1 || dist <= glm::distance(tree[c->closestIndex].pos, c->pos)) {
                    flag = true;
                    c->closestIndex = i;
                }
            }
        }
    }

    return flag;
}

glm::vec3 scol::randdir(glm::vec3 vec) {
    // Introduce jaggedness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(0.5);

    if(d(gen))
        vec = glm::rotateX(vec, (d(gen) ? 16.f : -16.f));

    if(d(gen))
        vec = glm::rotateZ(vec, (d(gen) ? 16.f : -16.f));

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
    std::vector<unsigned int> indices;

    for (int i = 1; i < tree.size(); i++) {
        indices.push_back(tree[i].parentIndex);
        indices.push_back(i);
    }

    return indices;
}