#include "lightning.hpp"

bool Lightning::grow(){
    vector<node>::iterator n;
    node curNode;
    bool end = false;
    
    updateAttractors();
    
    for(const charge& c : charges){
        n = find_if(tree.begin(), tree.end(), [c](node x){return x == c.closestNode;});
        if (n != tree.end()) {
            n->grow = true;
            n->dir += glm::normalize((c.pos - n->pos) * (c.attStrength / glm::distance(c.pos, n->pos)));
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
            if (child.pos.y <= 0){
                child.pos.y = 0;
                end = true;
            }

            // Add to tree
            tree.push_back(child);
            checkDeletion(child);
        }
    }

    return end;
}

glm::vec3 Lightning::randDir() {
    // Introduce jaggedness
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution d(0.5);
    
    // return (cos(16) || cos(-16), 1, sin(16) || sin (-16)) <- Average deviation observed.
    return(glm::vec3((d(gen) ? 0.9613 : -0.9613), 1, (d(gen) ? 0.2756 : -0.2756)));
}

void Lightning::updateAttractors(){
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

void Lightning::checkDeletion(const node& child){
    list<charge>::iterator c;
    c = find_if(charges.begin(), charges.end(), [child](charge x) {return glm::distance(child.pos, x.pos) < x.killDistance; });
        if(c != charges.end())
            charges.remove(*c);
}


void Lightning::draw() {
    for (const node& n : tree) {
        gl::glBegin(gl::GL_LINE);
            gl::glColor3f(0, 0, 0);
            gl::glVertex3f(n.parent.x, n.parent.y, n.parent.z);
            gl::glVertex3f(n.pos.x, n.pos.y, n.pos.z);
        gl::glEnd();
    }
}


void Lightning::print(){
    ofstream file;
    file.open("output.txt");
    for(const node& n : tree){
        file << n.pos.x << " , " << n.pos.y << " , " << n.pos.z << "\n";
    }
    file.close();
}

void Lightning::init() {
    bool end = false;
    while (!end)
        end = grow();
}