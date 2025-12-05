#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

constexpr int START = 50;

int solve() {
    string line;
    while (getline(cin, line)) {
        const char directive = line[0];
        if ('L' != directive && 'R' != directive) {
            cerr << "Invalid directive=" << directive << "\n";
            continue;
        }
        const int value = stoi(line.substr(1));

    }

    return 0;
}

int main() {
    return solve();
}
