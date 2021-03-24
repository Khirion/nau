#include "generator.hpp"

void Generator::generate() {
    string statement = states.back();
    string nextState;
    string block;
    map<char, vector<tuple<double, string>>>::iterator it;

    for (char c : statement) {
        it = rules.find(c);
        if (it != rules.end()) 
           block = parseRules(it->second);

        else 
           block = c;

        turtle(block);
        nextState = nextState + block;
    }
    states.push_back(nextState);
}

string Generator::parseRules(const vector<tuple<double, string>>& rules) {
    default_random_engine generator;
    vector<double> probs;
    for (tuple<double, string> p : rules) {
        probs.push_back(get<0>(p));
    }
    discrete_distribution<int> distribution (probs.begin(), probs.end());

    return get<1>(rules[distribution(generator)]);
}

/* TODO
void Generator::draw() {
    VSPolyLine pl;
    for (vector<Point3> line : lines) {
        pl.set(line, 0);
        pl.render();
    }
}
*/

void Generator::turtle(string state) {
    glm::vec3 prevPoint;
    vector<glm::vec3> line = vector<glm::vec3>(2);
    for (char c : state) {
        switch (c) {
            case 'F':
                line[0] = matrix * glm::vec4(0, 0, 0, 1);
                matrix = glm::translate(matrix, glm::vec3(0,(len / (states.size() + 1)), 0));
                line[1] = matrix * glm::vec4(0, 0, 0, 1);

                lines.push_back(line);
                break;
            case '+':
                matrix = glm::rotate(matrix, glm::radians(angle), glm::vec3(1, 0, 0));
                break;
            case '-':
                matrix = glm::rotate(matrix, glm::radians(angle), glm::vec3(-1, 0, 0));
                break;
            case '[':
                mStack.push(matrix);
                break;
            case ']':
                matrix = mStack.top();
                mStack.pop();
                break;
            case '^':
                matrix = glm::rotate(matrix, glm::radians(angle), glm::vec3(0, 0, 1));
                break;
            case '&':
                matrix = glm::rotate(matrix, glm::radians(angle), glm::vec3(0, 0, -1));
                break;
            case '<':
                matrix = glm::rotate(matrix, glm::radians(angle), glm::vec3(0, 1, 0));
                break;
            case '>':
                matrix = glm::rotate(matrix, glm::radians(angle), glm::vec3(0, -1, 0));
                break;
            case '|':
                matrix = glm::rotate(matrix, glm::radians(180.f), glm::vec3(0, 1, 0));
                break;
        }
    }
}

void Generator::print() {
    for (int i = 0; i < states.size(); i++) {
        for (char c : states[i]) {
            cout << c;
        }
    }
}

void Generator::init(int cycles) {
    turtle(states.back()); // parse axiom
    for (int i = 0; i < cycles; i++) {
        generate();
    }
}