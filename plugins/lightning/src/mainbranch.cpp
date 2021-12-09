#include "mainbranch.hpp"

void mainBranch::init(std::vector<glm::vec3> waypoints){
    if (waypoints.size() % 2)
        return;

    root = waypoints[0];
    end = waypoints[1];
    startDir = glm::normalize(end - root);

    tree.push_back(node(0, root, startDir, true));

    middle = (root + end) / 2.f;
        
    charges.push_back(end);
    genCharges();
    grow();

    if (waypoints.size() > 2) {
        for (int i = 1; i < waypoints.size() - 1; i++) {
            root = getClosest(waypoints[i]).second;
            end = waypoints[i + 1];

            tree.push_back(node(static_cast<int>(tree.size()), root, startDir, true));

            middle = (root + end) / 2.f;

            charges.push_back(end);
            genCharges();
            grow();
        }
    }
}

void mainBranch::genCharges() {
    glm::mat3 transform(1);

    glm::vec3 vector = glm::normalize(root - end);

    if (!(vector.x == 0 && vector.z == 0)) {
        glm::vec3 new_y = glm::normalize(root - end);
        glm::vec3 new_z = glm::normalize(glm::cross(new_y, glm::vec3(0, 1, 0)));
        glm::vec3 new_x = glm::normalize(glm::cross(new_y, new_z));
        transform = glm::mat3(new_x, new_y, new_z);
    }
    else {
        if (vector.y == -1)
            transform = transform * -1.f;
    }

    float height = glm::distance(root, end);

    genCyl(end, transform, height, (height/6) * width);
}

void mainBranch::genRect(glm::vec3 center, glm::mat3 transform, float height, float side) {
    std::default_random_engine generator;
    std::normal_distribution<float> rand(0.5, 0.25);
    std::normal_distribution<float> randXZ(0, 0.5);
    auto genR = bind(rand, generator);
    auto genXZ = bind(randXZ, generator);

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    for (int i = 0; i < 10000; i++) {
        y = genR() * height; // calculate height
        x = genXZ() * side; // Random side element * maximum side length * linear decrease
        z = genXZ() * side; // Random side element * maximum side length * linear decrease
        charges.push_back(center + (transform * glm::vec3(x, y, z)));
    }
}

void mainBranch::genCyl(glm::vec3 center, glm::mat3 transform, float height, float maxRad) {
    std::default_random_engine generator;
    std::normal_distribution<float> rand(0.5, 0.5);
    auto genR = bind(rand, generator);

    float y = 0.f;
    float radius = 0.f;
    float angle = 0.f;

    for (int i = 0; i < 10000; i++) {
        y = genR() * height;
        radius = sqrt(genR()) * maxRad; // Random radius element * maximum radius for the disc
        angle = genR() * 2 * pi;
        charges.push_back(center + (transform * glm::vec3(radius * cos(angle), y, radius * sin(angle))));
    }
}

void mainBranch::genPyr(glm::vec3 center, glm::mat3 transform, float height, float side) {
    std::default_random_engine generator;
    std::normal_distribution<float> rand(0.5, 0.25);
    std::normal_distribution<float> randXZ(0, 0.5);
    auto genR = bind(rand, generator);
    auto genXZ = bind(randXZ, generator);

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    for (int i = 0; i < 10000; i++) {
        y = genR(); // calculate height
        x = genXZ() * side * y; // Random side element * maximum side length * linear decrease
        z = genXZ() * side * y; // Random side element * maximum side length * linear decrease
        charges.push_back(center + (transform * glm::vec3(x, y * height, z)));
    }
}

void mainBranch::genCone(glm::vec3 center, glm::mat3 transform, float height, float maxRad) {
    std::default_random_engine generator;
    std::normal_distribution<float> rand(0.5, 0.5);
    auto genR = bind(rand, generator);

    float y = 0.f;
    float radius = 0.f;
    float angle = 0.f;

    for (int i = 0; i < 10000; i++) {
        y = genR();
        radius = sqrt(genR()) * maxRad * (y); // Random radius element * maximum radius for the disc * linear decrease
        angle = genR() * 2 * pi;
        charges.push_back(center + (transform * glm::vec3(radius * cos(angle), y * height, radius * sin(angle))));
    }
}

void mainBranch::grow() {
    std::vector<node>::iterator n;
    std::list<charge>::iterator c;
    std::vector<node> buffer = std::vector<node>();
    glm::vec3 tempPos = glm::vec3();
    bool flag = true;
    node child;

    flag = checkDeletion(tree);

    // Initial Growth
   while (!updateAttractors() && !charges.empty() && flag) {
        const node& curNode = tree.back();
        tempPos = curNode.pos + (randdir(glm::normalize(curNode.dir + curNode.startDir)) * growthLength);
        node child = node(static_cast<int>(tree.size() - 1), tempPos, glm::normalize(end - tempPos), true);
        tree.push_back(child);
        flag = checkDeletion(std::vector<node>(1, child));
    }

   lastNode = tree.size() - 1;

    // Space Colonization Growth
    while (updateAttractors() && flag) {
        for (const charge& charge : charges) {
            if (charge.closestIndex != -1){
                n = tree.begin() + charge.closestIndex;
                n->attNum += 1;
                n->dir += glm::normalize(charge.pos - n->pos);
            }
        }

        int i = 0;

        for (n = tree.begin(); n != tree.end(); n++, i++) {
            if (n->attNum > 0) {
                n->dir = glm::normalize(n->dir);
                n->attNum = 0;

                if (i == lastNode) {
                    tempPos = n->pos + (glm::normalize(n->dir * 0.75f + n->startDir) * growthLength);
                    child = node(i, tempPos, glm::normalize(end - tempPos), true);
                    lastNode = tree.size() + buffer.size();
                }
                else {
                    tempPos = n->pos + (glm::normalize(n->dir + n->startDir * 0.75f) * growthLength);
                    child = node(i, tempPos, glm::normalize(end - tempPos), false);
                }
                buffer.push_back(child);
            }
        }
        tree.insert(tree.end(), buffer.begin(), buffer.end());
        flag = checkDeletion(buffer);
        buffer.clear();
    }
}

glm::vec3 mainBranch::randdir(glm::vec3 vec) {
    // Introduce jaggedness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(0.5);

    bool flag = d(gen);

    if (flag)
        vec = glm::rotateX(vec, (d(gen) ? 16.f : -16.f));
    else
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
    float dist = 0;

    for (const node &n : tree) {
        if (i == lastNode || (genR() < cplx && (std::count_if(tree.begin(), tree.end(), [i](node n) {return i == n.parentIndex; }) - n.mainBranch < cull))) {
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

bool mainBranch::checkDeletion(const std::vector<node>& buffer) {
    std::list<charge>::iterator c;

    for (const node &n : buffer) {
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


    for(int i = 0; i < tree.size(); i++){
        dist = glm::distance(tree[i].pos, pos);
        if (closestDist > dist) {
            index = i;
            closestDist = dist;
        }
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
    it = parIndMap.find(0);
    parent = it->first;
    vaux = it->second;
    
    for (int child : vaux) {
        if (tree[child].mainBranch) {
            indices.push_back(std::make_pair(1, parent));
            indices.push_back(std::make_pair(1, child));
        }
        else {
            indices.push_back(std::make_pair(0, parent));
            indices.push_back(std::make_pair(0, child));
        }
    }
    pQueue.insert(pQueue.end(), vaux.begin(), vaux.end());
    parIndMap.erase(0);

    // Height-based Traversal
    for (int key = 0; key < pQueue.size(); key++) {
        it = parIndMap.find(pQueue[key]);
        if (it != parIndMap.end()) {
            parent = it->first;
            vaux = it->second;

            for (int child : vaux) {
                if (tree[child].mainBranch) {
                    indices.push_back(std::make_pair(1, parent));
                    indices.push_back(std::make_pair(1, child));
                }
                else {
                    indices.push_back(std::make_pair(0, parent));
                    indices.push_back(std::make_pair(0, child));
                }
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

unsigned int mainBranch::getIndiceSize(){
    return indices.size();
}

std::pair<std::vector<unsigned int>, std::vector<unsigned int>> mainBranch::getIndices(float size) {
    std::vector<unsigned int> mBranchi;
    std::vector<unsigned int> branchi;

    for (int i = 0; i < size; i++) {
        if (indices[i].first)
            mBranchi.push_back(indices[i].second);
        else
            branchi.push_back(indices[i].second);
    }
    
    return std::make_pair(mBranchi, branchi);
}