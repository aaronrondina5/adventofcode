#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

using ll = long long;

constexpr char ADD = '+';
constexpr char MULTIPLY = '*';
constexpr char SPACE = ' ';

const unordered_map<char, ll> initial_values = {{ADD, 0}, {MULTIPLY, 1}};

// Part 1
// could parse the input to a grid, right?
// then iterate column first, then row - ie for col : cols { for row : rows } { op(ans) }
// I think the toughest part about this problem is going to be the parsing,
// since the data isnt delineated clearly

ll do_operation(const ll l, const ll r, const char oper_char)
{
	if (ADD == oper_char)
	{
		return l + r;
	}
	else if (MULTIPLY == oper_char)
	{
		return l * r;
	}
	cerr << "Cannot perform op=" << oper_char << "\n";
	return l;
}

ll solve_column(const size_t col, const char op, const vector<vector<ll>>& data_set)
{
	ll result = initial_values.at(op);
	const size_t rows = data_set.size();
	for (size_t row = 0; row < rows; ++row)
	{
		result = do_operation(result, data_set[row][col], op);
	}
	return result;
}

ll compute_total_value(const vector<vector<ll>>& data_set, const vector<char>& operations)
{
	ll result = 0;
	const size_t cols = data_set[0].size(); // protect later
	for (size_t col = 0; col < cols; ++col)
	{
		result += solve_column(col, operations[col], data_set);
	}
	return result;
};

void parse_method_1(vector<vector<ll>>& data_set, vector<char>& operations,
					const vector<string>& input)
{
	const size_t rows = input.size();
	const size_t cols = input[0].size(); // protect later

	for (size_t row = 0; row < rows - 1; ++row)
	{
		vector<ll>& row_values = data_set.emplace_back();
		ll current_value = 0;

		// move column up to first value
		size_t col = 0;
		while (SPACE == input[row][col])
		{
			++col;
		}

		//
		while (col <= cols)
		{
			if (col == cols || SPACE == input[row][col])
			{
				// reached a space or the end of the data.
				// need to add the current value to vector, and move up counter
				row_values.emplace_back(current_value);

				while (col < cols && SPACE == input[row][col])
				{
					++col;
				}
				current_value = 0; // reset

				// without this, the function will infinitely loop
				if (col == cols)
				{
					break;
				}
			}
			else
			{
				// not a space. add to the value
				current_value = (current_value * 10) + (input[row][col] - '0');
				++col;
			}
		}
	}

	size_t col = 0;
	while (col < cols)
	{
		const char val = input[rows - 1][col];
		if (SPACE != val)
		{
			operations.emplace_back(val);
		}
		++col;
	}
}

ll solve_part_1(const vector<string>& input)
{
	vector<vector<ll>> data_set;
	vector<char> operations;
	parse_method_1(data_set, operations, input);

	return compute_total_value(data_set, operations);
}

// Part 2
// Yep, as I intuited in part 1, the difficulty of this problem lies in parsing the input
// so, what is the algorithm here?
// starting at the right-most col, we go down the rows to get our value.
// if all col values are space, we know the values are done.
// before this, you always check the final row for an existing operation to keep
// as curr_op.

ll apply_operation(const vector<ll>& values, const char oper_char)
{
	ll result = initial_values.at(oper_char); // protect later
	for (ll val : values)
	{
		result = do_operation(result, val, oper_char);
	}
	return result;
}

ll compute_method_2(const vector<string>& input)
{
	const size_t rows = input.size();
	const size_t cols = input[0].size(); // assume all rows same length

	ll result = 0;

	char curr_oper = SPACE;
	vector<ll> curr_operands;
	for (int col = cols - 1; col > -1; --col)
	{
		bool all_rows_space = true;

		// a single column either contains a value or does not. loop through the rows
		ll curr_value = 0;
		for (size_t row = 0; row < rows - 1; ++row)
		{
			const char raw_value = input[row][col];
			if (SPACE != raw_value)
			{
				all_rows_space = false;
				curr_value = (curr_value * 10) + (raw_value - '0');
			}
		}

		if (SPACE == curr_oper && SPACE != input[rows - 1][col])
		{
			// we found our operator
			curr_oper = input[rows - 1][col];
		}

		if (all_rows_space || 0 == col)
		{
			// All spaces. apply operations, add value, and reset for the next set
			result += apply_operation(curr_operands, curr_oper);
			curr_oper = SPACE;
			curr_operands.clear();
		}
		else
		{
			// add our value to the set, and reset the curr_value
			curr_operands.emplace_back(curr_value);
			curr_value = 0;
		}
	}

	return result;
}

void time_wrap(function<void(const vector<string>&)> func, const vector<string>& input1)
{
	auto start = chrono::system_clock::now();
	func(input1);
	auto end = chrono::system_clock::now();
	chrono::duration<double> elapsed = end - start;
	cout << "time_seconds=" << elapsed.count() << "\n";
}

int solve(const vector<string>& input)
{
	auto solve1 = [](const vector<string>& i)
	{
		ll result = solve_part_1(i);
		cout << "Part 1 : compute_total_value=" << result << "\n";
	};
	time_wrap(solve1, input);

	auto solve2 = [](const vector<string>& i)
	{
		ll result = compute_method_2(i);
		cout << "Part 2 : compute_total_value=" << result << "\n";
	};
	time_wrap(solve2, input);
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
