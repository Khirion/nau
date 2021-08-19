#include "mainbranch.hpp"

void mainBranch::init(std::vector<glm::vec3> waypoints, int genType, int width){
    if (waypoints.size() % 2)
        return;

    tree.push_back(node(0, waypoints[0], glm::normalize(waypoints[1] - waypoints[0]), glm::vec3(0,-1,0)));

    for (int i = 0; i < waypoints.size() - 1; i += 2) {
        bottom = (waypoints[i] + waypoints[i + 1])/2.f;
        
        charges.push_back(waypoints[i+1]);

        genCharges(waypoints[i], waypoints[i+1], genType, width);

        grow(waypoints[i+1]);
    }
}

void mainBranch::genCharges(glm::vec3 root, glm::vec3 waypoint, int genType, int width) {
    // Random Engine - TBD
    // Divide the length between waypoints according to the amount of spheres to create, randomly assign spheres to left/right, front/back as per the radius defined by the length/2 of each segment
    glm::mat3 transform(1);

    glm::vec3 center = (root + waypoint) / 2.f;
    glm::vec3 vector = glm::normalize(waypoint - root);

    if (!(vector.x == 0 && vector.z == 0)) {
        glm::vec3 new_y = glm::normalize(waypoint - root);
        glm::vec3 new_z = glm::normalize(glm::cross(new_y, glm::vec3(0, 1, 0)));
        glm::vec3 new_x = glm::normalize(glm::cross(new_y, new_z));
        transform = glm::mat3(new_x, new_y, new_z);
    }
    else {
        if (vector.y == -1)
            transform = transform * -1.f;
    }

    float height = glm::distance(root, waypoint);

    switch (genType){
        case 1: genRect(root, transform, height, height / width); 
            break;
        case 2: genCyl(root, transform, height, height / width);
            break;
        case 3: genPyr(root, transform, height, height / width);
            break;
        case 4: genCone(root, transform, height, height / width);
            break;
        case 5: genSphere(root, transform, height / width);
            break;
        default: genSphere(center, transform, height / width);
            break;
    }
}

void mainBranch::genRect(glm::vec3 root, glm::mat3 transform, float height, float side) {
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

void mainBranch::genCyl(glm::vec3 root, glm::mat3 transform, float height, float maxRad) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand(0.f, 1.f);
    auto genR = bind(rand, generator);

    float y = 0.f;
    float radius = 0.f;
    float angle = 0.f;

    for (int i = 0; i < chargeNum; i++) {
        y = genR() * height;
        radius = sqrt(genR()) * maxRad; // Random radius element * maximum radius for the disc * linear decrease
        angle = genR() * 2 * pi;
        charges.push_back(root + (transform * glm::vec3(radius * cos(angle), y, radius * sin(angle))));
    }
}

void mainBranch::genPyr(glm::vec3 root, glm::mat3 transform, float height, float side) {
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

void mainBranch::genCone(glm::vec3 root, glm::mat3 transform, float height, float maxRad) {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand(0.f, 1.f);
    auto genR = bind(rand, generator);

    float y = 0.f;
    float radius = 0.f;
    float angle = 0.f;

    for (int i = 0; i < chargeNum; i++) {
        y = genR();
        radius = sqrt(genR()) * maxRad * y; // Random radius element * maximum radius for the disc * linear decrease
        angle = genR() * 2 * pi;
        charges.push_back(root + (transform * glm::vec3(radius * cos(angle), y * height, radius * sin(angle))));
    }
}

void mainBranch::genSphere(glm::vec3 center, glm::mat3 transform, float maxRad) {
    for (int i = 0; i < chargeNum; i++) {
        charges.push_back(center + (transform * glm::ballRand(maxRad)));
    }
}

void mainBranch::grow(glm::vec3 end) {
    std::vector<node>::iterator n;
    std::list<charge>::iterator c;
    std::vector<node> buffer = std::vector<node>();
    bool flag = true;
    node child;

    // Initial Growth
   while (!updateAttractors() && !charges.empty()) {
        const node& curNode = tree.back();
        node child = node(tree.size(), curNode.pos + (curNode.dir * growthLength), curNode.dir, curNode.dir);
        tree.push_back(child);
    }

    // Space Colonization Growth
    while (updateAttractors() && flag) {
        for (const charge& charge : charges) {
            if (charge.closestIndex != -1){
                n = tree.begin() + charge.closestIndex;
                n->attNum += 1;
                n->dir += normalize(charge.pos - n->pos);
            }
        }

        flag = checkDeletion(end);

        int i = 0;

        for (n = tree.begin(); n != tree.end(); n++, i++) {
            if (n->attNum > 1) {
                n->dir = n->dir / n->attNum;
                n->attNum = 1;

                if (i == lastNode) {
                    child = node(i, n->pos + (normalize(n->dir) * growthLength), glm::vec3(0, -1, 0), normalize(n->dir));
                    lastNode = tree.size() + buffer.size();
                }
                else if (std::count_if(tree.begin(), tree.end(), [i](node n) {return i == n.parentIndex; }) < 2)
                    child = node(i, n->pos + (normalize(n->dir) * growthLength), glm::vec3(0, 0, 0), normalize(n->dir));
                buffer.push_back(child);
            }
        }

        tree.insert(tree.end(), buffer.begin(), buffer.end());
        buffer.clear();
    }
}

bool mainBranch::updateAttractors() {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand(0.f, 1.f);
    auto genR = bind(rand, generator);

    std::list<charge>::iterator c;
    bool flag = false;
    int i = 0;
    float dist;

    for (const node &n : tree) {
        dist = 1 - (glm::distance(n.pos, bottom) / glm::distance(tree[0].pos, bottom));
        if (i == lastNode || genR() * dist * (weight) < growthChance) {
            for (c = charges.begin(); c != charges.end(); c++) {
                dist = glm::distance(n.pos, c->pos);
                    if (dist <= attDistance && (c->closestIndex == -1 || dist <= glm::distance(tree[c->closestIndex].pos, c->pos))) {
                        flag = true;
                        c->closestIndex = i;
                    }
                }
            }
        i += 1;
    }
    return flag;
}

bool mainBranch::checkDeletion(glm::vec3 end) {
    std::list<charge>::iterator c;

    for (const node &n : tree) {
        for (c = charges.begin(); c != charges.end(); c++) {
            c->closestIndex = -1;
            if (glm::distance(n.pos, c->pos) <= killDistance)
                c->reached = true;
        }
    }

    charges.remove_if([](charge x) {return x.reached; });

    return charges.front() == end;
}

glm::vec3 mainBranch::randdir(glm::vec3 vec) {
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

std::vector<glm::vec3> mainBranch::getVertices() {
    std::vector<glm::vec3> vertices;
    for (node n : tree) {
        vertices.push_back(n.pos);
    }

    return vertices;
}

std::vector<unsigned int> mainBranch::getIndices() {
    std::vector<unsigned int> indices;

    for (int i = 1; i < tree.size(); i++) {
        indices.push_back(tree[i].parentIndex);
        indices.push_back(i);
    }

    return indices;
}