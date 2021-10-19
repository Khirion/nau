#include "mainbranch.hpp"

void mainBranch::init(std::vector<glm::vec3> waypoints, float width){
    if (waypoints.size() % 2)
        return;

    tree.push_back(node(0, waypoints[0], glm::normalize(waypoints[1] - waypoints[0])));

    middle = (waypoints[0] + waypoints[1]) / 2.f;
        
    charges.push_back(waypoints[1]);
    genCharges(waypoints[0], waypoints[1], 2, width);
    grow(waypoints[1]);

    if (waypoints.size() > 2) {
        glm::vec3 cPoint = glm::vec3(0);
        for (int i = 1; i < waypoints.size() - 1; i++) {
            cPoint = getClosest(waypoints[i]).second;
            tree.push_back(node(tree.size(), cPoint, normalize(waypoints[i+1] - cPoint)));

            middle = (cPoint + waypoints[i+1]) / 2.f;

            charges.push_back(waypoints[i+1]);
            genCharges(cPoint, waypoints[i+1], 2, width);
            grow(waypoints[i+1]);
        }
    }
}

void mainBranch::genCharges(glm::vec3 root, glm::vec3 waypoint, int genType, float width) {
    glm::mat3 transform(1);

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

    genCone(waypoint, transform, height, height / (10/width));
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
        radius = sqrt(genR()) * maxRad * (y); // Random radius element * maximum radius for the disc * linear decrease
        angle = genR() * 2 * pi;
        charges.push_back(root + (-transform * glm::vec3(radius * cos(angle), y * height, radius * sin(angle))));
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
        node child = node(tree.size() - 1, curNode.pos + (randdir(curNode.dir) * growthLength), curNode.dir);
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
                    child = node(i, n->pos + (normalize(n->dir) * growthLength), normalize(n->dir));
                    lastNode = tree.size() + buffer.size();
                }
                else if (std::count_if(tree.begin(), tree.end(), [i](node n) {return i == n.parentIndex; }) < 2)
                    child = node(i, n->pos + (normalize(n->dir) * growthLength), glm::vec3(0, 0, 0));
                buffer.push_back(child);
            }
        }

        tree.insert(tree.end(), buffer.begin(), buffer.end());
        buffer.clear();
    }
}

glm::vec3 mainBranch::randdir(glm::vec3 vec) {
    // Introduce jaggedness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(0.25);

    if (d(gen))
        vec = glm::rotateX(vec, (d(gen) ? 16.f : -16.f));
    if (d(gen))
        vec = glm::rotateZ(vec, (d(gen) ? 16.f : -16.f));
    return vec;
}

bool mainBranch::updateAttractors() {
    std::default_random_engine generator;
    std::uniform_real_distribution<float> rand(0.f, 1.f);
    auto genR = bind(rand, generator);

    std::list<charge>::iterator c;
    bool flag = false;
    int i = 0;
    float distWeight;
    float dist = 0;

    for (const node &n : tree) {
        distWeight = 1 - (glm::distance(n.pos, middle) / glm::distance(tree[0].pos, middle));
        if (i == lastNode || genR() * distWeight * (weight) < growthChance) {
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

    if (charges.empty())
        return false;

    return charges.front() == end;
}

std::pair<int, glm::vec3> mainBranch::getClosest(glm::vec3 pos) {
    int index = 0;
    float closestDist = FLT_MAX;
    float dist = 0;
    int i = 0;

    for (const node& n : tree) {
        dist = glm::distance(n.pos, pos);
        if (closestDist > dist) {
            index = i;
            closestDist = dist;
        }
        i += 1;
    }

    return std::pair<int, glm::vec3>(index, tree[index].pos);
}

void mainBranch::makeMap() {
    std::map<int, std::vector<int>>::iterator it;
    int pInd;
    std::vector<int> vect;

    for (int i = 0; i < tree.size(); i++) {
        pInd = tree[i].parentIndex;
        it = parIndMap.find(pInd);

        if (it == parIndMap.end()) {
            vect.push_back(i);
            parIndMap[pInd] = vect;
            vect.clear();
        }
        
        else {
            vect = parIndMap[pInd];
            vect.push_back(i);
            parIndMap[pInd] = vect;
            vect.clear();
        }

        vertices.push_back(tree[i].pos);
    }
}

void mainBranch::makeIndexes() {
    std::map<int, std::vector<int>>::iterator it;
    std::vector<int> pQueue;
    std::vector<int> vaux;
    int parent = 0;

    vertices.clear();
    indices.clear();

    makeMap();

    // Root Setup
    it = parIndMap.begin();
    parent = it->first;
    vaux = it->second;
    
    for (int child : vaux) {
        indices.push_back(parent);
        indices.push_back(child);
    }

    pQueue.insert(pQueue.end(), vaux.begin(), vaux.end());

    // Height-based Traversal
    for (int key = 0; key < pQueue.size(); key++) {
        it = parIndMap.find(pQueue[key]);
        if (it != parIndMap.end()) {
            parent = it->first;
            vaux = it->second;

            for (int child : vaux) {
                indices.push_back(parent);
                indices.push_back(child);
            }

            pQueue.insert(pQueue.end(), vaux.begin(), vaux.end());
            parIndMap.erase(it);
        }
    }
}

void mainBranch::addVector(std::vector<node> vector) {
    tree.insert(tree.end(), vector.begin(), vector.end());
}

int mainBranch::getSize()
{
    return tree.size();
}

std::vector<glm::vec3> mainBranch::getVertices() {
    return vertices;
}

std::vector<unsigned int> mainBranch::getIndices(){
    return indices;
}
