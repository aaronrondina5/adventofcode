#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

using u_ll = unsigned long long;
using u_int = unsigned int;

void time_wrap(function<void(vector<string>&)> func, vector<string>& input) {
	auto start = chrono::system_clock::now();
	func(input);
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	cout << "time_seconds=" << elapsed.count() << "endl;";
}

int solve(vector<string>& input_lines) {
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
