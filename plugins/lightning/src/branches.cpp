#include "branches.hpp"

void branch::init(int pInd, glm::vec3 _start, glm::vec3 _end, const std::vector<glm::vec3>& mainTree) {
    tree.push_back(node(pInd, _start, glm::normalize(_end - _start), false));

    root = _start;
    end = _end;

    charges.push_back(_end);
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

    for (int i = 0; i < 10 * height; i++) {
        y = biModal() * height;
        radius = sqrt(genR()) * maxRad; // Random radius element * maximum radius for the disc
        angle = genR() * 2 * pi;
        charges.push_back(center + (transform * glm::vec3(radius * cos(angle), y, radius * sin(angle))));
    }
}

float branch::biModal() {
    static std::default_random_engine generator;
    std::normal_distribution<float> randL(0.35, 0.35/6);
    std::normal_distribution<float> randH(0.75, 0.75/6);
    std::bernoulli_distribution randB(0.5);

    return(randB(generator) ? randL(generator) : randH(generator));
}

void branch::grow() {
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> rand(0.f, 1.f);
    auto genR = bind(rand, generator);

    std::vector<node>::iterator n;
    std::list<charge>::iterator c;
    glm::vec3 newPos = glm::vec3();
    bool flag = true;
    bool atts = false;
    node child;

    while (flag) {
        atts = updateAttractors();
        const node& curNode = tree.back();

        if (genR() < cplx)
            branchNodes.push_back(std::make_pair(static_cast<int>(tree.size()) + mainIndex - 1, curNode.pos));

        // Scol Growth
        if (atts)
            newPos = curNode.pos + (glm::normalize((curNode.dir * (1.f - weight)) + (curNode.weightDir * weight)) * growthLength);
        //16 degree random growth
        else
            newPos = curNode.pos + (glm::normalize(randdir(curNode.weightDir * (1.f - weight)) + (curNode.weightDir * weight)) * growthLength);

        node child = node(static_cast<int>(tree.size()) + mainIndex - 1, newPos, glm::normalize(end - newPos), false);
        tree.push_back(child);
        flag = checkDeletion(child);
    }
}

glm::vec3 branch::randdir(glm::vec3 vec) {
    // Introduce jaggedness
    static std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(0.5);

    bool flag = d(gen);

    if (flag)
        vec = glm::rotateX(vec, (d(gen) ? glm::radians(16.f) : glm::radians(-16.f)));
    else
        vec = glm::rotateZ(vec, (d(gen) ? glm::radians(16.f) : glm::radians(-16.f)));
    return vec;
}

bool branch::updateAttractors() {
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

bool branch::checkDeletion(const node& n) {
    std::list<charge>::iterator c;
    for (c = charges.begin(); c != charges.end(); c++) {
        if (glm::distance(n.pos, c->pos) < killDistance)
            c->deletionFlag = true;
    }

    charges.remove_if([](charge x) {return x.deletionFlag; });

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
                c->deletionFlag = true;
        }
    }

    charges.remove_if([](charge x) {return x.deletionFlag; });
}

std::vector<std::pair<int, glm::vec3>> branch::getBranchNodes() {
    return branchNodes;
}


std::vector<node> branch::getVector()
{
    return tree;
}
