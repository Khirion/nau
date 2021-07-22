#include "scol.hpp"

float killDistance;
float attDistance;
float growthLength;

void scol::init(float killDst, float attDst, int chargeNum, int sphereNum, int gLength, std::vector<glm::vec3> waypoints, int genType){
    if (waypoints.size() % 2)
        return;

    growthLength = gLength;
    killDistance = killDst * growthLength;
    attDistance = attDst * growthLength;

    tree.push_back(node(0, waypoints[0], glm::normalize(waypoints[1] - waypoints[0])));

    for (int i = 0; i < waypoints.size() - 1; i += 2) {
        charges.push_back(waypoints[i+1]);

        genCharges(waypoints[i], waypoints[i+1], sphereNum, chargeNum, genType);

        grow();
    }
}

void scol::genCharges(glm::vec3 root, glm::vec3 waypoint, int sphereNum, int chargeNum, int genType) {
    // Random Engine - TBD
    // Divide the length between waypoints according to the amount of spheres to create, randomly assign spheres to left/right, front/back as per the radius defined by the length/2 of each segment
    glm::mat3 transform(1);

    glm::vec3 center = (waypoint + root) / 2.f;

    if (!glm::all(glm::equal(glm::normalize(waypoint - root), glm::vec3(0, 1, 0)))) {
        glm::vec3 new_y = glm::normalize(waypoint - root);
        glm::vec3 new_z = glm::normalize(glm::cross(new_y, glm::vec3(0, 1, 0)));
        glm::vec3 new_x = glm::normalize(glm::cross(new_y, new_z));
        transform = glm::mat3(new_x, new_y, new_z);
    }

    float height = glm::distance(root, waypoint);

    switch (genType){
        case 1: genRect(root, transform, height, height/4, chargeNum); 
            break;
        case 2: genCyl(root, transform, height, height / 4, chargeNum);
            break;
        case 3: genPyr(root, transform, height, height / 4, chargeNum);
            break;
        case 4: genCone(root, transform, height, height / 4, chargeNum);
            break;
        case 5: genSphere(root, transform, height / 2, chargeNum);
            break;
        default: genSphere(center, transform, height / 2, chargeNum);
            break;
    }
}

void scol::genRect(glm::vec3 root, glm::mat3 transform, float height, float side, int chargeNum) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand(0.f, 1.f);
    std::uniform_real_distribution<float> randXZ(-1.f, 1.f);
    auto genR = bind(rand, generator);
    auto genXZ = bind(randXZ, generator);

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    for (int i = 0; i < chargeNum; i++) {
        y = genR() * height; // calculate height
        x = genXZ() * side; // Random side element * maximum side length * linear decrease
        z = genXZ() * side; // Random side element * maximum side length * linear decrease
        charges.push_back(root + (transform * glm::vec3(x, y, z)));
    }
}

void scol::genCyl(glm::vec3 root, glm::mat3 transform, float height, float maxRad, int chargeNum) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand(0.f, 1.f);
    auto genR = bind(rand, generator);

    float y = 0.f;
    float radius = 0.f;
    float angle = 0.f;

    for (int i = 0; i < chargeNum; i++) {
        y = genR() * height;
        radius = sqrt(genR()) * maxRad; // Random radius element * maximum radius for the disc * linear decrease
        angle = genR() * 2 * M_PI;
        charges.push_back(root + (transform * glm::vec3(radius * cos(angle), y, radius * sin(angle))));
    }
}

void scol::genPyr(glm::vec3 root, glm::mat3 transform, float height, float side, int chargeNum) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand(0.f, 1.f);
    std::uniform_real_distribution<float> randXZ(-1.f, 1.f);
    auto genR = bind(rand, generator);
    auto genXZ = bind(randXZ, generator);

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    for (int i = 0; i < chargeNum; i++) {
        y = genR(); // calculate height
        x = genXZ() * side * y; // Random side element * maximum side length * linear decrease
        z = genXZ() * side * y; // Random side element * maximum side length * linear decrease
        charges.push_back(root + (transform * glm::vec3(x, y * height, z)));
    }
}

void scol::genCone(glm::vec3 root, glm::mat3 transform, float height, float maxRad, int chargeNum) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand(0.f, 1.f);
    auto genR = bind(rand, generator);

    float y = 0.f;
    float radius = 0.f;
    float angle = 0.f;

    for (int i = 0; i < chargeNum; i++) {
        y = genR();
        radius = sqrt(genR()) * maxRad * y; // Random radius element * maximum radius for the disc * linear decrease
        angle = genR() * 2 * M_PI;
        charges.push_back(root + (transform * glm::vec3(radius * cos(angle), y * height, radius * sin(angle))));
    }
}

void scol::genSphere(glm::vec3 center, glm::mat3 transform, float maxRad, int chargeNum) {
    for (int i = 0; i < chargeNum; i++) {
        charges.push_back(center + (transform * glm::ballRand(maxRad)));
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
                n->attNum += 1;
                n->dir += normalize(charge.pos - n->pos);
            }
        }

        int i = 0;

        for (n = tree.begin(); n != tree.end(); n++, i++) {
            if (n->attNum > 0) {
                n->dir = n->dir / n->attNum;
                node child = node(i, n->pos + (n->dir * growthLength), glm::vec3(0,0,0));
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
        n->attNum = 0;
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