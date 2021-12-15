#include "branches.hpp"

void branch::init(int pInd, std::pair<glm::vec3, glm::vec3> waypoints, const std::vector<glm::vec3>& mainTree) {
    tree.push_back(node(pInd, waypoints.first, glm::normalize(waypoints.second - waypoints.first), false));

    root = waypoints.first;
    middle = (root + waypoints.second) / 2.f;
    startDir = glm::normalize(waypoints.second - root);
    end = waypoints.second;

    charges.push_back(waypoints.second);
    genCharges();
    checkTreeDel(mainTree);
    grow();
}

void branch::genCharges() {
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

    genCyl(end, transform, height, height);
}

void branch::genCyl(glm::vec3 center, glm::mat3 transform, float height, float maxRad) {
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> randG(0.f, 1.f);
    auto genR = bind(randG, generator);

    float y = 0.f;
    float radius = 0.f;
    float angle = 0.f;

    for (int i = 0; i < 100 * height; i++) {
        y = biModal() * height;
        radius = sqrt(genR()) * maxRad; // Random radius element * maximum radius for the disc
        angle = genR() * 2 * pi;
        charges.push_back(center + (transform * glm::vec3(radius * cos(angle), y, radius * sin(angle))));
    }
}

float branch::biModal() {
    static std::default_random_engine generator;
    std::normal_distribution<float> randL(0.35, 0.25);
    std::normal_distribution<float> randH(0.75, 0.25);
    std::bernoulli_distribution randB(0.5);

    return(randB(generator) ? randL(generator) : randH(generator));
}

void branch::grow() {
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
        node child = node(static_cast<int>(tree.size() - 1) + mainIndex, tempPos, glm::normalize(end - tempPos), false);
        tree.push_back(child);
        flag = checkDeletion(std::vector<node>(1, child));
    }

    lastNode = tree.size() - 1;

    // Space Colonization Growth
    while (updateAttractors() && flag) {
        for (const charge& charge : charges) {
            if (charge.closestIndex > -1) {
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
                    child = node(mainIndex + i, tempPos, glm::normalize(end - tempPos), false);
                    lastNode = tree.size() + buffer.size();
                }
                else {
                    tempPos = n->pos + (glm::normalize(n->dir + n->startDir * 0.75f) * growthLength);
                    child = node(mainIndex + i, tempPos, glm::normalize(end - tempPos), false);
                }
                buffer.push_back(child);
            }
        }
        tree.insert(tree.end(), buffer.begin(), buffer.end());
        flag = checkDeletion(buffer);
        buffer.clear();
    }
}

glm::vec3 branch::randdir(glm::vec3 vec) {
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

bool branch::updateAttractors() {
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> rand(0.f, 1.f);
    auto genR = bind(rand, generator);

    std::list<charge>::iterator c;
    bool flag = false;
    float dist = 0;
    int count = 0;
    int index = 0;

    for (int i = 0; i < tree.size(); i++) {
        index = i + mainIndex;
        count = std::count_if(tree.begin(), tree.end(), [i, index](node n) {return (n.parentIndex == index); }) - tree[i].mainBranch;
        if (i == lastNode || (genR() * std::max(1, count * 2) < cplx && count < cull)) {
            for (c = charges.begin(); c != charges.end(); c++) {
                dist = glm::distance(tree[i].pos, c->pos);
                if (dist <= attDistance && (c->closestIndex == -1 || dist <= glm::distance(tree[c->closestIndex].pos, c->pos))) {
                    flag = true;
                    c->closestIndex = i;
                }
            }
        }
    }
    return flag;
}

bool branch::checkDeletion(const std::vector<node>& buffer) {
    std::list<charge>::iterator c;

    for (const node& n : buffer) {
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

void branch::checkTreeDel(const std::vector<glm::vec3>& mainTree)
{
    std::list<charge>::iterator c;

    for (const glm::vec3& n : mainTree) {
        for (c = charges.begin(); c != charges.end(); c++) {
            if (glm::distance(n, c->pos) <= killDistance)
                c->reached = true;
        }
    }

    charges.remove_if([](charge x) {return x.reached; });
}

std::vector<node> branch::getVector()
{
    return tree;
}
