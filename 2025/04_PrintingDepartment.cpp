#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <utility>
#include <vector>

using namespace std;

using u_ll = unsigned long long;
using u_int = unsigned int;

constexpr char roll = '@';
constexpr char cleared_roll = 'x';
constexpr char empty_location = '.';

const vector<pair<int, int>> adjacent{{1, -1}, {1, 0}, {1, 1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}};
const vector<pair<int, int>> back_left_right{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}};

// Part 1
// thinking is that its a straightfoward loop through the grid, and check neighbors.
// this would be O(8 * n * m) which is O(n * m) which is basically the best we can do for something
// like this.
// perhaps there is a slightly more efficient solution that doesnt re-count neighbors. But not sure.

u_int sum_neighbors_with_symbol(const int i, const int j, const char symbol, const vector<string>& grid) {
	u_int result = 0;

	const size_t i_max = grid.size();
	const size_t j_max = grid[0].size();

	for (const auto& adj : adjacent) {
		const auto& [ii, jj] = adj;
		int iii = i + ii;
		int jjj = j + jj;

		if (iii >= 0 && jjj >= 0 && iii < i_max && jjj < j_max && grid[iii][jjj] == symbol) {
			++result;
		}
	}

	return result;
}

/**
 * @brief Get number of '@' in the grid with fewer than 4 neighbors with '@'
 *        Easiest solution is to loop through each, sum the neighbors.
 *
 *        Time : O(i * j * 8) = O(i * j)
 */
u_int get_num_accessible_rolls(const vector<string>& grid, const u_int max_neighbors) {
	const size_t rows = grid.size();
	const size_t cols = grid[0].size();
	u_int result = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (grid[i][j] != roll || sum_neighbors_with_symbol(i, j, roll, grid) > max_neighbors - 1)
				continue;
			++result;
		}
	}
	return result;
}

// Part 2
// The most obvious solution that comes to mind is to just iterate part 1 (with a clear)
// until the result is 0.
//
// Initially, I tried BFS idea and it was taking way too long. This was because
// i was initially enqueuing items repeatedly by not re-checking whether it was a roll
// or not. It could have been cleared and checked by an earlier iteration that shared it as
// a neighbor!
u_int attack_and_clear(vector<string>& grid, queue<pair<int, int>>& q, const u_int max_neighbors) {
	u_int result = 0;

	const size_t i_max = grid.size();
	const size_t j_max = grid[0].size();

	while (!q.empty()) {
		const auto [i, j] = q.front();
		q.pop();

		if (grid[i][j] != roll || sum_neighbors_with_symbol(i, j, roll, grid) >= max_neighbors) {
			// not clearable
			continue;
		}

		grid[i][j] = cleared_roll;
		++result;

		// add neighbors that are rolls
		for (const auto& adj : adjacent) {
			const auto& [ii, jj] = adj;
			int iii = i + ii;
			int jjj = j + jj;

			if (iii >= 0 && jjj >= 0 && iii < i_max && jjj < j_max && grid[iii][jjj] == roll) {
				q.push({iii, jjj});
			}
		}
	}

	return result;
}

u_int get_number_accessible_with_clear(vector<string>& grid, const u_int max_neighbors) {
	const size_t rows = grid.size();
	const size_t cols = grid[0].size();

	vector<vector<u_int>> visited_on_iteration(rows, vector<u_int>(cols, 0));
	queue<pair<int, int>> q;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (grid[i][j] == roll && sum_neighbors_with_symbol(i, j, roll, grid) < max_neighbors)
				q.push({i, j});
		}
	}
	return attack_and_clear(grid, q, max_neighbors);
}

void time_wrap(std::function<void(vector<string>&)> func, vector<string>& input) {
	auto start = std::chrono::high_resolution_clock::now();
	func(input);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	cout << "time_seconds=" << elapsed.count() << "\n";
}

int solve(vector<string>& input_lines) {
	vector<string> input_copy{input_lines}; // just to be safe
	auto solve_1 = [](vector<string>& grid) {
		u_int result = get_num_accessible_rolls(grid, 4);
		std::cout << "Part 1 : get_num_accessible_rolls=" << result << "\n";
	};
	time_wrap(solve_1, input_copy);

	auto solve_2 = [](vector<string>& grid) {
		u_int result = get_number_accessible_with_clear(grid, 4);
		std::cout << "Part 2 : get_number_accessible_with_clear=" << result << "\n";
	};
	time_wrap(solve_2, input_lines); // just use the input

	return 0;
}

void get_input_as_vector(vector<string>& input_lines) {
	string line;
	while (getline(cin, line)) {
		input_lines.push_back(line);
	}
}

int main() {
	vector<string> input_lines;
	get_input_as_vector(input_lines);

	if (0 != solve(input_lines)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
