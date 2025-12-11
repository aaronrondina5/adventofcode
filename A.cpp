#include <algorithm>
#include <cmath>
#include <chrono>
#include <iostream>
#include <functional>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

using u_ll = unsigned long long;
using u_int = unsigned int;


void time_wrap(function<void(vector<string>&)> func, vector<string>& input)
{
	auto start = std::chrono::system_clock::now();
	func(input);
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	cout << "time_seconds=" << elapsed.count() << "\n";
}

int solve(vector<string>& input_lines)
{
	return 0;
}

void get_input_as_vector(vector<string>& input_lines)
{
	string line;
	while (getline(cin, line))
	{
		input_lines.push_back(line);
	}
}

int main() {
	vector<string> input_lines;
	get_input_as_vector(input_lines);

	if (0 != solve(input_lines))
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
