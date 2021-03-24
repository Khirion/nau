#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <stack>
#include <random>

#include <glm.hpp>
#include <gtx/transform.hpp>
#include <gtc/matrix_transform.hpp>

using namespace std;

constexpr auto len = 10;
constexpr auto angle = 25.f;

class Generator{
    private:
        string alphabet = "F+-[]^&<>|"; // Downwards, Turn Left, Turn Right, Save State, Restore State, Pitch Up, Pitch Down, Turn 180
        char axiom = 'F';
        map<char, vector<tuple<double, string>>> rules;
        vector<string> states = vector<string>(1,string(&axiom));
        stack<glm::mat4> mStack = stack<glm::mat4>();
        glm::mat4 matrix = glm::mat4(1.0f);
        vector<vector<glm::vec3>> lines = vector<vector<glm::vec3>>();

    public:
        Generator(map<char, vector<tuple<double, string>>> rules) : rules(rules){};
        void init(int cycles);
        void generate();
        string parseRules(const vector<tuple<double, string>> &rules);
        void draw(); // TODO
        void turtle(string state);
        void print();
        vector<string> getStates() {return states;}
};