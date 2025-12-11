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

const vector<pair<int, int>> adjacent{{1, -1}, {1, 0},	{1, 1},	 {-1, -1},
									  {-1, 0}, {-1, 1}, {0, -1}, {0, 1}};

// Part 1
// thinking is that its a straightfoward loop through the grid, and check neighbors.
// this would be O(8 * n * m) which is O(n * m) which is basically the best we can do for something
// like this.
// perhaps there is a slightly more efficient solution that doesnt re-count neighbors. But not sure.

u_int sum_neighbors_with_symbol(const int i, const int j, const char symbol,
								const vector<string>& grid)
{
	u_int result = 0;

	const size_t i_max = grid.size();
	const size_t j_max = grid[0].size();

	for (const auto& adj : adjacent)
	{
		const auto& [ii, jj] = adj;
		int iii = i + ii;
		int jjj = j + jj;

		if (iii >= 0 && jjj >= 0 && iii < i_max && jjj < j_max && grid[iii][jjj] == symbol)
		{
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
 *
 * 	      should_clear will clear the roll if it can be
 */
u_int get_num_accessible_rolls(vector<string>& grid, const u_int max_neighbors,
							   const bool should_clear)
{
	const size_t rows = grid.size();
	const size_t cols = grid[0].size();
	u_int result = 0;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			if (grid[i][j] != roll ||
				sum_neighbors_with_symbol(i, j, roll, grid) > max_neighbors - 1)
				continue;

			if (should_clear)
			{
				grid[i][j] = cleared_roll;
			}
			++result;
		}
	}
	return result;
}

// Part 2
// The most obvious solution that comes to mind is to just iterate until you can no
// longer can.
// Maybe there is some more efficient solution out there? Some BFS? But it would still end up
// repeating grid positions.
u_int solve_accessible_with_clear_iter(vector<string>& grid, const u_int max_neighbors)
{
	const size_t rows = grid.size();
	const size_t cols = grid[0].size();

	u_int result = 0;
	while (true)
	{
		u_int iter_result = get_num_accessible_rolls(grid, max_neighbors, true);
		result += iter_result;
		if (iter_result == 0)
			break;
	}

	return result;
}

void timeWrap(std::function<void(vector<string>&)> func, vector<string>& input)
{
	auto start = std::chrono::high_resolution_clock::now();
	func(input);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	cout << "time_seconds=" << elapsed.count() << "\n";
}

int solve(vector<string>& input_lines)
{
	auto solve_1 = [](vector<string>& grid)
	{
		u_int result = get_num_accessible_rolls(grid, 4, false);
		std::cout << "Part 1 : get_num_accessible_rolls=" << result << "\n";
	};
	timeWrap(solve_1, input_lines);

	auto solve_2 = [](vector<string>& grid)
	{
		u_int result = solve_accessible_with_clear_iter(grid, 4);
		std::cout << "Part 2 : solve_accessible_with_clear_iter=" << result << "\n";
	};
	// we can re-use input since it wasnt altered by solve_1
	timeWrap(solve_2, input_lines);

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

int main()
{
	vector<string> input_lines;
	get_input_as_vector(input_lines);

	if (0 != solve(input_lines))
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
