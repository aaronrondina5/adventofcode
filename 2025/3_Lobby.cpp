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

/**
 * Part 1
 *
 *        Basic 2 pass algorithm: Iterate through the array and find max
 *        values and keep position. On second pass, start at the index of
 *        your previous max and do the same thing.
 *
 *        runtime O(n * l) n - number of banks, l - number of batteries per bank
 *        size O(1)
 */

/**
 * @brief Get the index of the maximum value in a given range of a numeric string.
 */
void max_from(size_t& index_result, const size_t start_index_inclusive,
			  const size_t end_index_exclusive, const string& string_of_integers)
{
	index_result = start_index_inclusive;
	for (size_t i = start_index_inclusive + 1; i < end_index_exclusive; ++i)
	{
		if ((string_of_integers[i] - '0') > (string_of_integers[index_result] - '0'))
		{
			index_result = i;
		}
	}
}

u_ll get_max_joltage(const string& battery_bank, const size_t num_digits)
{
	const size_t n = battery_bank.size();

	u_ll result = 0;
	size_t last_max_index = 0;
	for (int i_digit = 0; i_digit < num_digits; ++i_digit)
	{
		const size_t lo_index_incl = i_digit == 0 ? 0 : last_max_index + 1;
		const size_t hi_index_excl = n + 1 - (num_digits - i_digit);

		max_from(last_max_index, lo_index_incl, hi_index_excl, battery_bank);

		result *= 10;
		result += (battery_bank[last_max_index] - '0');
	}

	return result;
}

u_ll compute_total_max_joltage(const vector<string>& battery_banks, const size_t num_digits)
{
	u_ll result = 0;
	const size_t n = battery_banks.size();
	for (size_t i = 0; i < n; ++i)
	{
		result += get_max_joltage(battery_banks[i], num_digits);
	}
	return result;
}

// Part 2
// solved simply by increasing the digits to 12.

void time_wrap(function<void(const vector<string>&)> func, const vector<string>& input)
{
	auto start = std::chrono::high_resolution_clock::now();
	func(input);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	cout << "time_seconds=" << elapsed.count() << "\n";
}

int solve(vector<string>& input_lines)
{
	auto solve_1 = [](const vector<string>& battery_banks)
	{
		u_ll result = compute_total_max_joltage(battery_banks, 2);
		std::cout << "Part 1 : compute_total_max_joltage=" << result << "\n";
	};
	time_wrap(solve_1, input_lines);

	auto solve_2 = [](const vector<string>& battery_banks)
	{
		u_ll result = compute_total_max_joltage(battery_banks, 12);
		std::cout << "Part 2 : compute_total_max_joltage=" << result << "\n";
	};
	time_wrap(solve_2, input_lines);

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
