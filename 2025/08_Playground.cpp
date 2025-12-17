#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <list>
#include <numeric>
#include <queue>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

using u_ll = unsigned long long;
using u_int = unsigned int;

constexpr char COMMA = ',';

// Part 1
// The naive solution is to do an n^2 traversal of the data,
// and place the index pairs on a priority queue
// The time is n^2 logn and space is n^2
// This doesnt seem ideal on its face.
// maybe there is some sort of sorting you can do beforehand.
// sort by x, y & z.
//

// I think you may have to just do that?
// Double loop through nodes, sad... build a double-edged graph, keep track of disconnected
// nodes, traverse those nodes for size,
//
// Can also just look at circuits, and do the union of the edges as
// they are added
// then you just loop through circuits and find 3 max values.

struct Coord {
	double x;
	double y;
	double z;
};

struct JboxPair {
	u_int a_id;
	u_int b_id;
	double distance;
	JboxPair(u_int a, u_int b, double d) : a_id{a}, b_id{b}, distance{d} {}
};

struct ByDistanceMax {
	bool operator()(const JboxPair& a, const JboxPair& b) const { return a.distance < b.distance; }
};

struct ByDistanceMin {
	bool operator()(const JboxPair& a, const JboxPair& b) const { return a.distance > b.distance; }
};

using JBoxPairMaxHeap = priority_queue<JboxPair, vector<JboxPair>, ByDistanceMax>;
using JBoxPairMinHeap = priority_queue<JboxPair, vector<JboxPair>, ByDistanceMin>;

struct Circuit {
	vector<u_int> value_arr;
	unordered_set<u_int> value_set;

	Circuit(u_int node_1, u_int node_2) : value_arr{}, value_set{} {
		add_node(node_1);
		add_node(node_2);
	}

	bool contains(const u_int n) const { return value_set.find(n) != value_set.end(); }

	void merge_from(const Circuit& other) {
		for (u_int n : other.value_arr) {
			add_node(n);
		}
	}

	void add_node(u_int node) {
		// Only add if not already present to avoid duplicates
		if (value_set.insert(node).second) {
			value_arr.emplace_back(node);
		}
	}

	u_int size() const { return value_set.size(); }
};

using Circuits = list<Circuit>;

template <typename T>
constexpr T square(T x) {
	return x * x;
}

double square_diff(const Coord& a, const Coord& b) {
	return square(a.x - b.x) + square(a.y - b.y) + square(a.z - b.z);
}
double distance_between(const Coord& a, const Coord& b) {
	return sqrt(square_diff(a, b));
}

void parse_coords(Coord& result, const string& input) {
	// Expect format: "x,y,z" with optional spaces
	// dont worry about protecting
	size_t p1 = input.find(COMMA);
	size_t p2 = input.find(COMMA, p1 + 1);
	result.x = stod(input.substr(0, p1));
	result.y = stod(input.substr(p1 + 1, p2 - (p1 + 1)));
	result.z = stod(input.substr(p2 + 1));
}

void parse_input_to_coords(vector<Coord>& result, const vector<string>& input) {
	const u_int n = input.size();
	for (u_int i = 0; i < n; ++i) {
		parse_coords(result[i], input[i]);
	}
}

// make a bounded max heap
// thus, you get the smallest num_pairs elements
void order_closest_pairs(JBoxPairMaxHeap& result, const vector<Coord>& coords, u_int num_pairs) {
	const u_int n = coords.size();
	for (u_int i = 0; i < n; ++i) {
		// you should only loop through the j = i + 1 pairs because there will have already
		// been a comparison between j = 0 to i and i.
		for (u_int j = i + 1; j < n; ++j) {
			result.push(JboxPair{i, j, distance_between(coords[i], coords[j])});
			if (result.size() > num_pairs) {
				result.pop();
			}
		}
	}
}

void merge_circuits(Circuits::iterator it1, Circuits::iterator it2, Circuits& circuits) {
	if (it1 == it2)
		return;
	it1->merge_from(*it2);
	circuits.erase(it2);
}

void add_to_circuits(Circuits& circuits, u_int node_1, u_int node_2) {
	auto c1_it = circuits.end();
	auto c2_it = circuits.end();

	// check the existing circuits for any that conatain either node.
	// if they do, we want to add to the existing circuit.
	// if 2 separate circuits are found, we need to merge those circuits.
	for (auto it = circuits.begin(); it != circuits.end(); ++it) {

		bool in1 = it->contains(node_1);
		bool in2 = it->contains(node_2);

		if (in1 && !in2) {
			it->add_node(node_2);
			if (c1_it == circuits.end())
				c1_it = it;
			else
				c2_it = it;
		} else if (in2 && !in1) {
			it->add_node(node_1);
			if (c1_it == circuits.end())
				c1_it = it;
			else
				c2_it = it;
		} else if (in1 && in2) {
			if (c1_it == circuits.end())
				c1_it = it;
		}
	}

	if (c1_it == circuits.end() && c2_it == circuits.end()) {
		// need to add a new circuit, because no circuits found
		circuits.emplace_back(node_1, node_2);
	} else if (c1_it != circuits.end() && c2_it != circuits.end()) {
		merge_circuits(c1_it, c2_it, circuits);
	}
}

void make_circuits(list<Circuit>& result, JBoxPairMaxHeap& pq) {
	while (!pq.empty()) {
		const JboxPair& top = pq.top();
		add_to_circuits(result, top.a_id, top.b_id);
		pq.pop();
	}
}

u_ll construct_closest_circuits(const vector<string>& input, const u_int num_pairs) {
	const u_int n = input.size();
	vector<Coord> coords(n, Coord{});
	parse_input_to_coords(coords, input);

	JBoxPairMaxHeap pq;
	order_closest_pairs(pq, coords, num_pairs);

	list<Circuit> circuits;
	make_circuits(circuits, pq);

	priority_queue<u_int, vector<u_int>, greater<u_int>> max_size_min_heap;
	for (auto it = circuits.begin(); it != circuits.end(); ++it) {
		max_size_min_heap.push(it->size());
		if (max_size_min_heap.size() > 3) {
			max_size_min_heap.pop();
		}
	}

	u_ll result = 1;
	while (!max_size_min_heap.empty()) {
		result *= max_size_min_heap.top();
		max_size_min_heap.pop();
	}
	return result;
}

// Part 2
// most of the hard work has been done in part 1
// now, we just need to continue adding the closest circuits until
// list<Circuit> size is 1
// but you also need to check that all jboxes are added.
double last_pair_single_circuit(const vector<string>& input) {
	const u_int n = input.size();
	vector<Coord> coords(n, Coord{});
	parse_input_to_coords(coords, input);

	// add all pairs this time
	JBoxPairMinHeap pq;
	for (u_int i = 0; i < n; ++i) {
		for (u_int j = i + 1; j < n; ++j) {
			pq.push(JboxPair{i, j, distance_between(coords[i], coords[j])});
		}
	}

	// add circuits until you are at 1 total circuit of size n
	list<Circuit> circuits;

	double last_x1 = 0.0, last_x2 = 0.0;
	while (!pq.empty()) {
		const JboxPair& top = pq.top();

		add_to_circuits(circuits, top.a_id, top.b_id);
		last_x1 = coords[top.a_id].x;
		last_x2 = coords[top.b_id].x;

		pq.pop();

		if (1 == circuits.size() && n == circuits.front().size()) {
			break;
		}
	}

	cout << last_x1 << ", " << last_x2 << "\n";

	return last_x1 * last_x2;
}

void time_wrap(function<void(const vector<string>&)> func, const vector<string>& input) {
	auto start = chrono::system_clock::now();
	func(input);
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	cout << "time_seconds=" << elapsed.count() << endl;
}

int solve(const vector<string>& input_lines) {
	auto solve_1 = [](const vector<string>& input) {
		u_ll result = construct_closest_circuits(input, 1000);
		cout << "Part 1 : result=" << result << "\n";
	};
	time_wrap(solve_1, input_lines);

	auto solve_2 = [](const vector<string>& input) {
		double result = last_pair_single_circuit(input);
		cout << "Part 2 : last_pair_single_circuit=" << result << "\n";
	};
	time_wrap(solve_2, input_lines);
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
