#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

constexpr int START = 50;
constexpr int LOCK_MAX = 99;

int solve_part1(const int startingPosition, const int lockMax) {
    if (lockMax < 0 || lockMax < startingPosition) {
        cout << "Invalid lock max or stargin position. startingPosition=" << startingPosition << ", lockMax=" << lockMax;
        exit(1);
    }

    int curr = startingPosition;
    int result = 0;

    string line;
    while (getline(cin, line)) {

        const char direction = line[0];
        if ('L' != direction && 'R' != direction) {
            cerr << "Invalid direction=" << direction << "\n";
            exit(2);
        }

        int value = stoi(line.substr(1));
        value = value % (lockMax + 1);
        if ('L' == direction) {
            value = lockMax + 1 - value;
        }

        const int temp = curr + value;

        // cout << "lock at position=" << curr << ". Moving " << value << " spaces to overextended position " << (curr + value) << "\n";

        curr = (curr + value) % (lockMax + 1);

        if (0 == curr) {
            ++result;
        }
    }

    cout << "Result is=" << result << endl;

    return 0;
}

int main() {
    return solve_part1(START, LOCK_MAX);
}
