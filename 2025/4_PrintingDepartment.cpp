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

void print(const vector<string>& grid)
{
	const size_t rows = grid.size();
	if (0 == rows)
	{
		return;
	}
	const size_t cols = grid[0].size();
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			cout << grid[i][j];
		}
		cout << "\n";
	}
}

/**
 * @brief Sums 8 neighbors with symbol.
 */
u_int sum_neighbors(const int i, const int j, const char symbol, const vector<string>& grid)
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
 * @brief BFS the grid and "clear" rolls until you cannot clear an item.
 */
u_int attack_and_clear(vector<string>& grid, const int i_start, const int j_start,
					   const u_int max_adjacent)
{
	u_int result = 0;

	const size_t i_max = grid.size();
	const size_t j_max = grid[0].size();

	vector<vector<bool>> visited(i_max, vector<bool>(j_max, false));

	queue<pair<int, int>> q;
	q.push({i_start, j_start});

	while (!q.empty())
	{
		const auto [i, j] = q.front();
		q.pop();

		if (grid[i][j] != roll)
		{
			// could have been cleared already.
			continue;
		}

		u_int num_rolls_nextdoor = 0;
		for (const auto& adj : adjacent)
		{
			const auto [ii, jj] = adj;
			int iii = i + ii;
			int jjj = j + jj;

			if (iii >= 0 && jjj >= 0 && iii < i_max && jjj < j_max && grid[iii][jjj] == roll)
			{
				++num_rolls_nextdoor;

				if (!visited[i][j])
				{
					q.push({iii, jjj});
				}
			}
		}

		if (num_rolls_nextdoor < max_adjacent)
		{
			++result;
			grid[i][j] = cleared_roll;
		}

		visited[i][j] = true;
	}

	return result;
}

/**
 * @brief Get total accessible rolls as you clear.
 *
 *        Algorithm is to find a clearable roll, then BFS until you hit rolls you can't move.
 *        Evenutally, you will have cleared the grid from all points of attack.
 */
u_int get_accessible_rolls_with_clear_bfs(vector<string>& grid, const u_int max_neighbors)
{
	const size_t rows = grid.size();
	const size_t cols = grid[0].size();
	u_int result = 0;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			if (grid[i][j] != roll)
				continue;
			result += attack_and_clear(grid, i, j, max_neighbors);
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
u_int get_num_accessible_rolls(const vector<string>& grid, const u_int max_neighbors)
{
	const size_t rows = grid.size();
	const size_t cols = grid[0].size();
	u_int result = 0;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			if (grid[i][j] == '.')
				continue;
			if (sum_neighbors(i, j, roll, grid) > max_neighbors - 1)
				continue;
			++result;
		}
	}
	return result;
}

/**
 * This ends up being way faster than the BFS approach.
 */
u_int solve_accessible_with_clear_iter(vector<string>& grid, const u_int max_neighbors)
{
	const size_t rows = grid.size();
	const size_t cols = grid[0].size();

	u_int result = 0;
	while (true)
	{
		u_int iter_result = 0;
		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < cols; ++j)
			{
				if (grid[i][j] != roll)
					continue;
				if (sum_neighbors(i, j, roll, grid) > max_neighbors - 1)
					continue;
				grid[i][j] = cleared_roll;
				++iter_result;
			}
		}

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
		u_int result = get_num_accessible_rolls(grid, 4);
		std::cout << "Part 1 : get_num_accessible_rolls=" << result << "\n";
	};
	timeWrap(solve_1, input_lines);

	auto solve_2 = [](vector<string>& grid)
	{
		u_int result = solve_accessible_with_clear_iter(grid, 4);
		std::cout << "Part 2 : solve_accessible_with_clear_iter=" << result << "\n";
	};
	vector<string> copy = input_lines;
	timeWrap(solve_2, copy);

	auto solve_3 = [](vector<string>& grid)
	{
		u_int result = get_accessible_rolls_with_clear_bfs(grid, 4);
		std::cout << "Part 2 : get_accessible_rolls_with_clear_bfs=" << result << "\n";
	};
	timeWrap(solve_3, input_lines);

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
