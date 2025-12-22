#include <algorithm>
#include <climits>
#include <cmath>
#include <chrono>
#include <iostream>
#include <functional>
#include <numeric>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

using u_ll = unsigned long long;

constexpr char BRACES_START = '{';
constexpr char BRACKET_CLOSE = ']';
constexpr char COMMA = ',';
constexpr char MACHINE_INDICATOR_ON = '#';
constexpr char PARENS_CLOSE = ')';
constexpr char PARENS_OPEN = '(';


// Initially, you can think of recursively trying the buttons.
// But how do we know our bounds? 
// Well, you will never click a button more than once.
// The final solution can be converted to binary so comparisons are O(1)
// 
// u_ll min_steps()
// it seems that you will have combinations of buttons
// for example [.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
// I wonder if you can sort of create a graph of connections, and BFS?
//
// BFS should be okay. There is probably a DP way to do this as well, that may be
// better in that it doesnt endlessly run if buttons do not converge.

struct Machine {
	Machine() : final_state{0}, buttons{} {}
	int final_state;
	int bit_size;
	vector<int> buttons;
};

// state keeps track of the button index & the current toggle state
struct BfsState {
	BfsState(int c, size_t p): curr_state{c}, pos{p} {}
	int curr_state;
	size_t pos;
};

void print_bits(int bits, int size) {
    for (int i = size - 1; i >= 0; --i) {
        cout << ((bits >> i) & 1);
    }
}

void print_machine_binary(const Machine& machine) {
    cout << "Machine in binary:\n";
    cout << "  Final State: ";
    print_bits(machine.final_state, machine.bit_size);
    cout << "\n  Buttons:\n";
    for (const int& button : machine.buttons) {
        cout << "    ";
        print_bits(button, machine.bit_size);
        cout << "\n";
    }
}

int bfs_buttons(const Machine& machine) {
    const vector<int>& buttons = machine.buttons;
    const size_t n = buttons.size();
    queue<BfsState> q;
    unordered_set<int> visited; // Track visited states

    q.push({0, 0}); // Start at state 0
    visited.insert(0);

    int steps = 0;
    while (!q.empty()) {
        const size_t q_size = q.size();

        // Process all states at the current depth
        for (size_t i = 0; i < q_size; ++i) {
            const int curr_state = q.front().curr_state;
            const size_t pos = q.front().pos;
            q.pop();

            // Check if the final state is reached
            if (curr_state == machine.final_state) {
				return steps;
            }

			for (size_t i = 0; i < n; ++i) {
				const int toggled = curr_state ^ buttons[i];
				if (visited.insert(toggled).second) {
					q.emplace(toggled, i);
				}
			}
        }
    	++steps; // Increment depth
    }

    return 0;
}

int get_min_steps(const vector<Machine>& machines) {
	int result = 0;
	for (const Machine& machine : machines) {
		int steps_for_machine = bfs_buttons(machine);
		print_machine_binary(machine);
		result += steps_for_machine;
	}
	return result;
}

int get_bits(const string& input, const size_t start, const size_t end_exclusive) {
	int bits = 0;
	for (size_t j = start; j < end_exclusive; ++j) {
		bits <<= 1;
		if (MACHINE_INDICATOR_ON == input[j]) {
			bits |= 1;
		}
	}
	return bits;
}

void parse_buttons(Machine& machine, const string& input, const size_t start) {
    size_t pos_end = input.find(BRACES_START, start);
    size_t i = start;
    while (i < pos_end) {
        size_t pos_parens_open = input.find(PARENS_OPEN, i);
        if (pos_parens_open == string::npos || pos_parens_open >= pos_end) {
            break; // No more buttons to parse
        }
        size_t pos_parens_close = input.find(PARENS_CLOSE, pos_parens_open);

        int button_bits = 0;
        size_t j = pos_parens_open + 1; // Start after '('
        while (j < pos_parens_close) {
            size_t pos_comma = input.find(COMMA, j);
            size_t end_pos = (pos_comma == string::npos || pos_comma > pos_parens_close)
                                 ? pos_parens_close
                                 : pos_comma;

			string num_str = input.substr(j, end_pos - j);
			int bit_position = stoi(num_str);

			int idx = machine.bit_size - 1 - bit_position;
			button_bits |= (1 << idx);

            j = end_pos + 1; // Move past ',' or ')'
        }

        machine.buttons.push_back({button_bits}); // Store the binary representation
        i = pos_parens_close + 1; // Move to the next button
    }
}

void parse_input(vector<Machine>& results, const vector<string>& input) {
	const size_t n = input.size();
	for (size_t i = 0; i < n; ++i) {
		const string& raw_machine = input[i];
		const size_t pos_bracket_close = raw_machine.find(BRACKET_CLOSE);

		Machine& machine = results[i];
		machine.final_state = get_bits(raw_machine, 1, pos_bracket_close);
		machine.bit_size = pos_bracket_close - 1;
		parse_buttons(results[i], raw_machine, pos_bracket_close + 1);
	}
}

void time_wrap(function<void(const vector<Machine>&)> func, const vector<Machine>& input)
{
	auto start = chrono::system_clock::now();
	func(input);
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	cout << "time_seconds=" << elapsed.count() << endl;
}

int solve(vector<string>& input_lines)
{
	vector<Machine> machines(input_lines.size(), Machine{});
	parse_input(machines, input_lines);

	auto solve_1 = [](const vector<Machine>& m) {
		int result = get_min_steps(m);
		cout << "Part 1 : result=" << result << "\n";
	};
	time_wrap(solve_1, machines);

	return 0;
}

void get_input_as_vector(vector<string>& input)
{
	string line;
	while (getline(cin, line))
	{
		input.push_back(line);
	}
}

int main() {
	vector<string> input;
	get_input_as_vector(input);

	if (0 != solve(input))
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
