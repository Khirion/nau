#include "mainbranch.hpp"

void mainBranch::init(std::vector<glm::vec3> waypoints){
    root = waypoints[0];
    end = waypoints[1];

    tree.push_back(node(0, root, glm::normalize(end - root), true));
        
    charges.push_back(end);
    genCharges();
    grow();

    if (waypoints.size() > 2) {
        for (int i = 2; i < waypoints.size(); i+=2) {
            std::pair<int, glm::vec3> tempp = getClosest(waypoints[i]);
            root = tempp.second;
            end = waypoints[i + 1];

            tree.push_back(node(tempp.first, root, glm::normalize(end - root), true));

            charges.push_front(end);
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

    genCyl(end, transform, height, (height * width)/2);
}

void mainBranch::genRect(glm::vec3 center, glm::mat3 transform, float height, float side) {
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> randG(-1.f, 1.f);
    auto genR = bind(randG, generator);

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    for (int i = 0; i < 250 * height; i++) {
        y = biModal() * height; // calculate height
        x = genR() * side; // Random side element * maximum side length * linear decrease
        z = genR() * side; // Random side element * maximum side length * linear decrease
        charges.push_back(center + (transform * glm::vec3(x, y, z)));
    }
}

void mainBranch::genCyl(glm::vec3 center, glm::mat3 transform, float height, float maxRad) {
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> randG(0.f, 1.f);
    auto genR = bind(randG, generator);

    float y = 0.f;
    float radius = 0.f;
    float angle = 0.f;

    for (int i = 0; i < 0 * height; i++) {
        y = biModal() * height;
        radius = sqrt(genR()) * maxRad; // Random radius element * maximum radius for the disc
        angle = genR() * 2 * pi;
        charges.push_back(center + (transform * glm::vec3(radius * cos(angle), y, radius * sin(angle))));
    }
}

float mainBranch::biModal() {
    static std::default_random_engine generator;
    std::normal_distribution<float> randL(0.35, 0.25);
    std::normal_distribution<float> randH(0.75, 0.25);
    std::bernoulli_distribution randB(0.5);
    
    return(randB(generator) ? randL(generator) : randH(generator));
}

void mainBranch::genPyr(glm::vec3 center, glm::mat3 transform, float height, float side) {
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> randG(-1.f, 1.f);
    auto genR = bind(randG, generator);

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    for (int i = 0; i < 250 * height; i++) {
        y = biModal();
        x = genR() * side * y; // Random side element * maximum side length * linear decrease
        z = genR() * side * y; // Random side element * maximum side length * linear decrease
        charges.push_back(center + (transform * glm::vec3(x, y * height, z)));
    }
}

void mainBranch::genCone(glm::vec3 center, glm::mat3 transform, float height, float maxRad) {
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> randG(-1.f, 1.f);
    auto genR = bind(randG, generator);

    float y = 0.f;
    float radius = 0.f;
    float angle = 0.f;

    for (int i = 0; i < 250 * height; i++) {
        y = biModal();
        radius = sqrt(genR()) * maxRad * (y); // Random radius element * maximum radius for the disc * linear decrease
        angle = genR() * 2 * pi;
        charges.push_back(center + (transform * glm::vec3(radius * cos(angle), y * height, radius * sin(angle))));
    }
}

void mainBranch::grow() {
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> rand(0.f, 1.f);
    auto genR = bind(rand, generator);

    std::vector<node>::iterator n;
    std::list<charge>::iterator c;
    glm::vec3 newPos = glm::vec3();
    bool flag = true;
    bool atts = false;
    node child;

    while (flag && tree.back().pos.y > 0) {
        atts = updateAttractors();
        const node& curNode = tree.back();

        if (genR() < cplx)
            branchNodes.push_back(curNode.pos);

        // Scol Growth
        if(atts)
            newPos = curNode.pos + (glm::normalize((curNode.dir * 0.4f) + (curNode.weightDir * 0.6f)) * growthLength);
        //16 degree random growth
        else
            newPos = curNode.pos + (glm::normalize(randdir(curNode.weightDir * 0.4f) + (curNode.weightDir * 0.6f)) * growthLength);

        node child = node(static_cast<int>(tree.size() - 1), newPos, glm::normalize(end - newPos), true);
        tree.push_back(child);
        flag = checkDeletion(child);
    }
}

glm::vec3 mainBranch::randdir(glm::vec3 vec) {
    // Introduce jaggedness
    static std::random_device rd;
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
    std::vector<node>::iterator n = tree.end() - 1;;
    std::list<charge>::iterator c;
    bool flag = false;
    float dist = 0;

    for (c = charges.begin(); c != charges.end(); c++) {
        dist = glm::distance(tree.back().pos, c->pos);
        if (dist <= attDistance) {
            flag = true;
            n->dir += glm::normalize(c->pos - n->pos);
        }
    }

    n->dir = glm::normalize(n->dir);

    return flag;
}

bool mainBranch::checkDeletion(const node& n) {
    std::list<charge>::iterator c;
    for (c = charges.begin(); c != charges.end(); c++) {
        if (glm::distance(n.pos, c->pos) <= killDistance)
            c->deletionFlag = true;
    }

    charges.remove_if([](charge x) {return x.deletionFlag; });

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