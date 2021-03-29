#include "scol.hpp"

bool scol::grow(){
    vector<node>::iterator n;
    node curNode;
    bool end = false;
    
    updateAttractors();
    
    for(const charge& c : charges){
        n = find_if(tree.begin(), tree.end(), [c](node x){return x == c.closestNode;});
        if (n != tree.end() && n->scan) {
            n->grow = true;
            n->dir += glm::normalize((c.pos - n->pos) * (c.attStrength / glm::distance(c.pos, n->pos)));
            n->dir += glm::normalize((floor.pos - n->pos) * (floor.attStrength / glm::distance(floor.pos, n->pos)));
            n->dir = glm::normalize(n->dir);
        }
    }

    n = find_if(tree.begin(), tree.end(), [this](node x){return x == floor.closestNode;});
    if (n != tree.end()) {
        n->grow = true;
        n->dir += glm::normalize((floor.pos - n->pos) * (floor.attStrength / glm::distance(floor.pos, n->pos)));
        n->dir = glm::normalize(n->dir);
    }

    int curSize = tree.size();
    for(int i = 0; i < curSize; i++){
        curNode = tree[i];
        if(curNode.grow){
            node child = node(curNode.pos, curNode.pos + (curNode.dir * curNode.growthLength), glm::normalize(curNode.dir * randDir()), true, true);

            // Ground Check
            if (child.pos.y <= 0)
                end = true;
            else {
                checkDeletion(child);
                tree.push_back(child);
            }
        }
    }
    return end;
}

glm::vec3 scol::randDir() {
    // Introduce jaggedness
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution d(0.5);
    
    // return (cos(16) || cos(-16), 1, sin(16) || sin (-16)) <- Average deviation observed.
    return(glm::vec3((d(gen) ? 0.9613 : -0.9613), 1, (d(gen) ? 0.2756 : -0.2756)));
}

void scol::updateAttractors(){
    vector<node>::iterator n;
    list<charge>::iterator c;
    float dist;

    for(n = tree.begin() ; n != tree.end() ; n++){
        if(n->scan){
            n->scan = false;
            for(c = charges.begin() ; c != charges.end() ; c++){
                dist = glm::distance(n->pos, c->pos);
                if(dist <= c->attDistance){
                    n->scan = true;
                    if (dist < glm::distance(c->closestNode, c->pos))
                        c->closestNode = n->pos;
                }
            }
        }

        n->grow = false;
        if(glm::distance(n->pos, floor.pos) < glm::distance(floor.closestNode, floor.pos))
            floor.closestNode = n->pos;
    }
}

void scol::checkDeletion(const node& child){
    list<charge>::iterator c;
    c = find_if(charges.begin(), charges.end(), [child](charge x) {return glm::distance(child.pos, x.pos) < x.killDistance; });
        if(c != charges.end())
            charges.remove(*c);
}

vector<glm::vec3> scol::getVertices() {
    vector<glm::vec3> vertices;
    for (node n : tree) {
        vertices.push_back(n.pos);
    }

    return vertices;
}

vector<unsigned int> scol::getIndices() {
    vector<node>::iterator it;
    vector<unsigned int> indices;

    for (int i = 1; i < tree.size(); i++) {
        it = find(tree.begin(), tree.end(), tree[i].parent);
        indices.push_back(it - tree.begin());
        indices.push_back(i);
    }

    return indices;
}

void scol::print(){
    ofstream file;
    file.open("output.txt");
    for(const node& n : tree){
        file << n.pos.x << " , " << n.pos.y << " , " << n.pos.z << "\n";
    }
    file.close();
}

void scol::init() {
    bool end = false;
    while (!end)
        end = grow();
}