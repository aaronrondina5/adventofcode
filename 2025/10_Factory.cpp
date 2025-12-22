#include <algorithm>
#include <bit>
#include <chrono>
#include <climits>
#include <cmath>
#include <functional>
#include <iostream>
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
constexpr char BRACES_END = '}';
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
// BFS should be okay. There is probably a DP way to do this as well

struct Machine {
	Machine() : final_state{0}, buttons{}, joltage_requirements{} {}
	int final_state;
	int bit_size;
	vector<int> buttons;
	vector<int> joltage_requirements;
};

// state keeps track of the button index & the current toggle state
struct BfsStateLights {
	BfsStateLights(int c, size_t p) : curr_state{c}, pos{p} {}
	int curr_state;
	size_t pos;
};

struct BfsStateJoltage {
	BfsStateJoltage(size_t bidx, vector<int> j) : button_idx{bidx}, curr_joltage(std::move(j)) {}
	size_t button_idx;
	vector<int> curr_joltage;
};

string get_bit_str(int bits, int size) {
	stringstream ss;
	for (int i = size - 1; i >= 0; --i) {
		ss << ((bits >> i) & 1);
	}
	return ss.str();
}

void print_machine(const Machine& machine) {
	cout << "Machine=[\n"
		 << "\tfinal_schematic_state=" << get_bit_str(machine.final_state, machine.bit_size)
		 << "\n\tbuttons=(";
	const size_t n = machine.buttons.size();
	for (size_t i = 0; i < n; ++i) {
		cout << get_bit_str(machine.buttons[i], machine.bit_size);
		cout << (i == n - 1 ? ")" : ",");
	}
	cout << "\n\tjoltage_requirements=(";
	const size_t n_joltage = machine.joltage_requirements.size();
	for (size_t i = 0; i < n_joltage; ++i) {
		cout << machine.joltage_requirements[i];
		cout << (i == n_joltage - 1 ? ")" : ",");
	}
	cout << "\n]\n";
}

int bfs_buttons_for_lights(const Machine& machine) {
	const vector<int>& buttons = machine.buttons;
	const size_t n = buttons.size();
	queue<BfsStateLights> q;
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

			for (size_t j = 0; j < n; ++j) {
				const int toggled = curr_state ^ buttons[j];
				if (visited.insert(toggled).second) {
					q.emplace(toggled, j);
				}
			}
		}
		++steps; // Increment depth
	}

	return 0;
}

int get_min_steps_for_lights(const vector<Machine>& machines) {
	int result = 0;
	for (const Machine& machine : machines) {
		result += bfs_buttons_for_lights(machine);
	}
	return result;
}

int get_schematic_as_bits(const string& input, const size_t start, const size_t end_exclusive) {
	int bits = 0;
	for (size_t j = start; j < end_exclusive; ++j) {
		bits <<= 1;
		if (MACHINE_INDICATOR_ON == input[j]) {
			bits |= 1;
		}
	}
	return bits;
}

void parse_joltage(Machine& machine, const string& input) {
	size_t i = input.find(BRACES_START) + 1;
	size_t idx_end = input.find(BRACES_END, i);
	while (i < idx_end) {
		size_t pos_comma = input.find(COMMA, i);
		size_t jolt_end_idx = pos_comma == string::npos ? idx_end : pos_comma;
		machine.joltage_requirements.emplace_back(stoi(input.substr(i, jolt_end_idx)));
		i = jolt_end_idx + 1;
	}
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
			size_t end_pos =
				(pos_comma == string::npos || pos_comma > pos_parens_close) ? pos_parens_close : pos_comma;

			string num_str = input.substr(j, end_pos - j);
			int indicator_position = stoi(num_str);

			// we need to read bits as right to left
			int bit_idx = machine.bit_size - 1 - indicator_position;
			button_bits |= (1 << bit_idx);

			j = end_pos + 1; // Move past ',' or ')'
		}

		machine.buttons.push_back({button_bits}); // Store the binary representation
		i = pos_parens_close + 1;				  // Move to the next button
	}
}

void parse_input(vector<Machine>& results, const vector<string>& input) {
	const size_t n = input.size();
	for (size_t i = 0; i < n; ++i) {
		const string& raw_machine = input[i];
		const size_t pos_bracket_close = raw_machine.find(BRACKET_CLOSE);

		Machine& machine = results[i];
		machine.final_state = get_schematic_as_bits(raw_machine, 1, pos_bracket_close);
		machine.bit_size = pos_bracket_close - 1;
		parse_buttons(results[i], raw_machine, pos_bracket_close + 1);

		parse_joltage(machine, raw_machine);
	}
}

// Part 2
// We need to find the right linear combinations, and the minimum linear combinations
// that result in the joltage.
// You could just do a similar BFS, but only enqueue if you are not higher than
// the total.
//
// I think the BFS solution takes just too long
// In order to properly solve this, I think you need to use linear algebra to find 
// the possible linear combinations that result in the joltage requirements.
//
// Maybe would come back to this to actually implement that with a C++ lib,
//

int bfs_for_joltage(const Machine& machine) {
	const vector<int>& buttons = machine.buttons;
	const size_t n_buttons = buttons.size();
	queue<BfsStateJoltage> q;

	const size_t n_joltage = machine.joltage_requirements.size();

	// Track visited joltage vectors to avoid revisiting identical states
	auto vec_key = [](const vector<int>& v) {
		string s;
		s.reserve(v.size() * 4);
		for (size_t i = 0; i < v.size(); ++i) {
			if (i)
				s.push_back(',');
			s += to_string(v[i]);
		}
		return s;
	};
	unordered_set<string> visited;

	q.emplace(0, vector<int>(n_joltage, 0)); // Start at state 0
	visited.insert(vec_key(q.front().curr_joltage));

	int steps = 0;
	while (!q.empty()) {
		const size_t q_size = q.size();

		// Process all states at the current depth
		for (size_t i = 0; i < q_size; ++i) {
			vector<int> curr_joltage = q.front().curr_joltage;
			const size_t idx = q.front().button_idx;
			q.pop();
		
			bool all_match = true;
			for (size_t j = 0; j < n_joltage; ++j) {
				if (curr_joltage[j] != machine.joltage_requirements[j]) {
					all_match = false;
				}
			}
			if (all_match) {
				return steps;
			}

			for (size_t bidx = 0; bidx < n_buttons; ++bidx) {
				const int button_value = buttons[bidx];

				vector<int> next = curr_joltage; // copy the joltage for the current q item
				bool should_add = true; // will only add if the joltages dont eclipse the requirements
				
				// for each joltage index, find the corresponding bit
				for (size_t joltage_idx = 0; joltage_idx < n_joltage; ++joltage_idx) {
					// need to convert to the bit starting from the right
					const size_t bit_idx = machine.bit_size - joltage_idx - 1;

					if (button_value & (1u << bit_idx)) {
						// bit is on
						next[joltage_idx] += 1;	
						if (next[joltage_idx] > machine.joltage_requirements[joltage_idx]) {
							should_add = false;
							break;
						}
					}
				}
		
				if (should_add) {
					string key = vec_key(next);
					if (visited.insert(key).second) {
						q.emplace(bidx, std::move(next));
					}
				}
			}
		}
		++steps; // Increment depth
	}

	return 0;
}

int get_min_presses_joltage(const vector<Machine>& machines) {
	int result = 0;
	for (const Machine& machine : machines) {
		result += bfs_for_joltage(machine);
	}
	return result;
}

void time_wrap(function<void(const vector<Machine>&)> func, const vector<Machine>& input) {
	auto start = chrono::system_clock::now();
	func(input);
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	cout << "time_seconds=" << elapsed.count() << endl;
}

int solve(vector<string>& input_lines) {
	vector<Machine> machines(input_lines.size(), Machine{});
	parse_input(machines, input_lines);

	auto solve_1 = [](const vector<Machine>& m) {
		int result = get_min_steps_for_lights(m);
		cout << "Part 1 : result=" << result << "\n";
	};
	time_wrap(solve_1, machines);

	auto solve_2 = [](const vector<Machine>& m) {
		int result = get_min_presses_joltage(m);
		cout << "Part 2 : result=" << result << "\n";
	};
	time_wrap(solve_2, machines);

	return 0;
}

void get_input_as_vector(vector<string>& input) {
	string line;
	while (getline(cin, line)) {
		input.push_back(line);
	}
}

int main() {
	vector<string> input;
	get_input_as_vector(input);

	if (0 != solve(input)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
