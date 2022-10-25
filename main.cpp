#include <iostream>
#include <fstream>
#include <queue>
#include <stack>
#include <unordered_map>
using namespace std;

int** allocMatr(int size) {
    int** matr = new int*[size];
    for (int i = 0; i < size; ++i)
        matr[i] = new int[size];
    return matr;
}
void deallocMatr(int** matr, int size) {
    for (int i = 0; i < size; ++i)
        delete[] matr[i];
    delete[] matr;
}

struct State {
    int size;
    int** state;
    struct {
        int i, j;
    } blankPosition;

    void printState(ostream &out) const {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j)
                out << state[i][j] << ' ';
            out << '\n';
        }
    }
};

struct Node {
    State currentState;
    int fatherNode;
    int positionInVector;
    string str;
    int level;
    int move; // 0(up) / 1(right) / 2(down) / 3(left)
    int f; // level + manhattan
    int manhattan;
};

struct CompareNode {
    bool operator()(Node const& n1, Node const& n2) {
        if (n1.f == n2.f) {
            if (n1.manhattan == n2.manhattan)
                return n1.level > n2.level;
            return n1.manhattan > n2.manhattan;
        }
        return n1.f > n2.f;
    }
};

State readState(istream &in) {
    State res{};

    in >> res.size;
    res.state = allocMatr(res.size);

    for (int i = 0; i < res.size; ++i)
        for (int j = 0; j < res.size; ++j) {
            in >> res.state[i][j];
            if (res.state[i][j] == 0)
                res.blankPosition = {i, j};
        }

    return res;
}
void printState(ofstream &out, State s) {
//    out << s.size << '\n';

    for (int i = 0; i < s.size; ++i) {
        for (int j = 0; j < s.size; ++j)
            out << s.state[i][j] << ' ';
        out << '\n';
    }
}

int calculateTotalInversions(State s) {
    int totalInversions = 0;
    for (int i = 0; i < s.size; ++i)
        for (int j = 0; j < s.size; ++j) {
            int inversions = 0;
            for (int k = i; k < s.size; ++k)
                for (int t = 0; t < s.size; ++t)
                    if (k > i || (k == i && t > j))
                        if (s.state[i][j] > s.state[k][t] && s.state[k][t] != 0)
                            ++inversions;
            totalInversions += inversions;
        }
    return totalInversions;
}
bool isSolvable(State s) {
    int totalInversions = calculateTotalInversions(s);
    if (s.size % 2 != 0 && totalInversions % 2 == 0)
       return true;
    int lineOfBlank = s.size - s.blankPosition.i;
    if (s.size % 2 == 0 && lineOfBlank % 2 == 0 && totalInversions % 2 != 0)
        return true;
    if (s.size % 2 == 0 && lineOfBlank % 2 != 0 && totalInversions % 2 == 0)
        return true;
    return false;
}

bool areStatesEqual(State s1, State s2) {
    for (int i = 0; i < s1.size; ++i)
        for (int j = 0; j < s1.size; ++j)
            if (s1.state[i][j] != s2.state[i][j])
                return false;
    return true;
}
bool areThereDuplicates(const vector<Node>& v) {
    // unused function
    // unordered_map used for preventing duplicate states
    int n = (int) v.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j)
            if (areStatesEqual(v[i].currentState, v[j].currentState))
                return true;
    }
    return false;
}

void generateChilds(State s, State childs[4]) {
    int di[] = {-1, 0, 1, 0};
    int dj[] = {0, 1, 0, -1};

    for (int k = 0; k < 4; ++k) {
        int newLine = s.blankPosition.i + di[k];
        int newColumn = s.blankPosition.j + dj[k];

        if (newLine >= s.size || newLine < 0 || newColumn >= s.size || newColumn < 0) {
            childs[k] = {-1, nullptr, {-1, -1}};
            continue;
        }

        childs[k] = {s.size, allocMatr(s.size), {newLine, newColumn}};
        for (int i = 0; i < s.size; ++i)
            for (int j = 0; j < s.size; ++j)
                childs[k].state[i][j] = s.state[i][j];

        swap(childs[k].state[s.blankPosition.i][s.blankPosition.j], childs[k].state[newLine][newColumn]);
    }
}
int manhattanDistance(State s) {
    // h = abs(current_cell.i - goal.i) + abs(current_cell.j - goal.j)

    int dist = 0;
    for (int i = 0; i < s.size; ++i)
        for (int j = 0; j < s.size; ++j) {
            if (s.state[i][j] == 0) continue;

            int goalI = (s.state[i][j] - 1) / s.size;
//            int goalJ = goalI % s.size;
            int goalJ = (s.state[i][j] - 1) % s.size;

            dist += abs(goalI - i) + abs(goalJ - j);
        }

    return dist;
}
string generateString(State s) {
    string str;
    for (int i = 0; i < s.size; ++i)
        for (int j = 0; j < s.size; ++j)
            str += to_string(s.state[i][j]) + " ";
    return str;
}

int aStar(State s, vector<Node>& v, ostream &out) {
    // 11111111: are you ill?
    // 11111110: no.. just feeling a bit off

    string str = generateString(s);
    Node start = {s, 0, 0, str, 0, -1, 0, 0};

    priority_queue<Node, vector<Node>, CompareNode> pq;
    unordered_map<string, int> map;

    pq.push(start);
    map.insert({str, 1});
    v.push_back(start);

    while (!pq.empty()) {
        Node current = pq.top(); // papa
        pq.pop();
        v.push_back(current);
        current.positionInVector = (int)v.size() - 1;

        //cout << "Level: " << current.level << " Nodes: " << v.size() << '\n';

        State childs[4];
        generateChilds(current.currentState, childs);

        for (int i = 0; i < 4; ++i) {
            if (childs[i].size == -1)
                continue;
            str = generateString(childs[i]);
            if (map[str] == 0) {
                int manhattan = manhattanDistance(childs[i]);
                Node child = {childs[i], current.positionInVector, static_cast<int>(v.size()), str, current.level + 1, i, current.level + 1 + manhattan, manhattan};
                pq.push(child);
                //v.push_back(child);
                map.insert({str, 1});

                if (manhattan == 0) {
                    out << "Solved in " << child.level << " moves!\n";
                    //child.currentState.printState(out);
                    //map.clear();
                    //while (!pq.empty()) pq.pop();
                    v.push_back(child);
                    return (int) v.size() - 1; // child position
                }
            }
        }
    }

    return 0;
}
void printSolution(int sol, vector<Node> v, ostream &out) {
    stack<Node> s;
    while (sol != 0) {
        s.push(v[sol]);
        sol = v[sol].fatherNode;
    }

    while (!s.empty()) {
        Node node = s.top();
        s.pop();
        switch (node.move) {
            case 0: { // blank space goes up
                out << "Move down\n"; // so tile goes down
                break;
            }
            case 1: { // blank space goes right
                out << "Move left\n"; // so tile goes left
                break;
            }
            case 2: { // blank space goes down
                out << "Move up\n"; // so tile goes up
                break;
            }
            case 3: { // blank space goes left
                out << "Move right\n"; // so tile goes right
                break;
            }
        }

        node.currentState.printState(out);
        out << '\n';
    }
}

int main() {
    ifstream in("date.in");
    ofstream out("date.out");

    State s = readState(in);

    if (isSolvable(s)) {
        out << "Puzzle is solvable!\n";
        vector<Node> v;
        int sol = aStar(s, v, out);
        printSolution(sol, v, out);
        // for (auto i : v) deallocMatr(i.currentState.state, i.currentState.size);
    }
    else {
        cout << "Puzzle is not solvable!\n";
    }

    in.close();
    out.close();
    return 0;
}
