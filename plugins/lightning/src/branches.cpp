#include "branches.hpp"

void branch::init(int pInd, std::pair<glm::vec3, glm::vec3> waypoints, const std::vector<glm::vec3>& mainTree) {
    tree.push_back(node(pInd, waypoints.first, glm::normalize(waypoints.second - waypoints.first), false));

    middle = (waypoints.first + waypoints.second) / 2.f;
    startDir = glm::normalize(waypoints.second - waypoints.first);

    charges.push_back(waypoints.second);
    genCharges(waypoints.first, waypoints.second);
    checkTreeDel(mainTree);
    grow(waypoints.second);
}

void branch::genCharges(glm::vec3 root, glm::vec3 waypoint) {
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
    genCone(root, transform, height, height/2);
}

void branch::genCone(glm::vec3 center, glm::mat3 transform, float height, float maxRad) {
    std::default_random_engine generator;
    std::normal_distribution<float> rand(0.5, 0.25);
    auto genR = bind(rand, generator);

    float y = 0.f;
    float radius = 0.f;
    float angle = 0.f;

    for (int i = 0; i < 25000; i++) {
        y = genR();
        radius = sqrt(genR()) * maxRad * (y); // Random radius element * maximum radius for the disc * linear decrease
        angle = genR() * 2 * pi;
        charges.push_back(center + (transform * glm::vec3(radius * cos(angle), y * height, radius * sin(angle))));
    }
}

void branch::grow(glm::vec3 end) {
    std::vector<node>::iterator n;
    std::list<charge>::iterator c;
    std::vector<node> buffer = std::vector<node>();
    glm::vec3 tempPos;
    bool flag = true;
    node child;

    // Initial Growth
    while (!updateAttractors() && !charges.empty()) {
        const node& curNode = tree.back();
        tempPos = curNode.pos + (randdir(curNode.dir) * growthLength);
        node child = node(static_cast<int>(tree.size() - 1 + mainIndex), tempPos, glm::normalize(end - tempPos), false);
        tree.push_back(child);
    }

    // Space Colonization Growth
    while (updateAttractors() && flag) {
        for (const charge& charge : charges) {
            if (charge.closestIndex != -1) {
                n = tree.begin() + charge.closestIndex;
                n->attNum += 1;
                n->dir += normalize(charge.pos - n->pos);
            }
        }

        flag = checkDeletion(end);

        int i = 0;

        for (n = tree.begin(); n != tree.end(); n++, i++) {
            if (n->attNum > 0) {
                tempPos = n->pos + (glm::normalize(n->dir) * growthLength);
                child = node(i + mainIndex, tempPos, glm::normalize(end - tempPos), false);
                buffer.push_back(child);
            }
        }

        tree.insert(tree.end(), buffer.begin(), buffer.end());
        buffer.clear();
    }
}

glm::vec3 branch::randdir(glm::vec3 vec) {
    // Introduce jaggedness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(0.5);

    if (d(gen))
        vec = glm::rotateX(vec, (d(gen) ? 16.f : -16.f));
    if (d(gen))
        vec = glm::rotateZ(vec, (d(gen) ? 16.f : -16.f));
    return vec;
}

bool branch::updateAttractors() {
    std::default_random_engine generator;

    std::list<charge>::iterator c;
    bool flag = false;
    int i = 0;
    float dist = 0;

    for (const node& n : tree) {
            for (c = charges.begin(); c != charges.end(); c++) {
                dist = glm::distance(n.pos, c->pos);
                if (dist <= attDistance && (c->closestIndex == -1 || dist <= glm::distance(tree[c->closestIndex].pos, c->pos))) {
                    flag = true;
                    c->closestIndex = i;
                }
            }
        i += 1;
    }
    return flag;
}

bool branch::checkDeletion(glm::vec3 end) {
    std::list<charge>::iterator c;

    for (const node& n : tree) {
        for (c = charges.begin(); c != charges.end(); c++) {
            c->closestIndex = -1;
            if (glm::distance(n.pos, c->pos) <= killDistance)
                c->reached = true;
        }
    }

    charges.remove_if([](charge x) {return x.reached; });

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
