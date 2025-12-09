#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

using u_ll = unsigned long long;
using u_int = unsigned int;

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

/**
 * @brief Get the maximum number formed for bank.
 *        Uses the same concept as simple_solve_part_1(), but loops through num_digits.
 */
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

/**
 * @brief Cleaner, final solution for part 2.
 * 
 *        O(n * l * j) n = num banks, l = length bank, j = num digits
 */
u_ll compute_total_max_joltage(const vector<string>& battery_banks, const size_t num_digits)
{
	u_ll result = 0;
	const size_t n = battery_banks.size();
	for (size_t i = 0; i < n; ++i)
	{
		result += get_max_joltage(battery_banks[i], num_digits);
	}

	cout << "Total charge = " << result << endl;
	return 0;
}

/**
 * @brief Get the maximum 2-digit number formed for each bank and sum.
 *        Bank is a string in the vector with digits - 1 through 9;
 *
 *        Basic 2 pass algorithm: Iterate through the array and find max
 *        values and keep position. On second pass, start at the index of
 *        your previous max and do the same thing.
 *
 *        runtime O(n * l) n - number of banks, l - number of batteries per bank
 *        size O(1)
 */
int simple_solve_part_1(const vector<string>& battery_banks)
{
	const size_t n = battery_banks.size();
	u_int result = 0;
	for (u_int i = 0; i < n; ++i)
	{
		const auto& battery_bank = battery_banks[i];
		const size_t n_i = battery_bank.size();

		size_t tens_index = 0;
		size_t ones_index = 1;
		max_from(tens_index, 0, n_i - 1, battery_bank);
		max_from(ones_index, tens_index + 1, n_i, battery_bank);

		cout << "Max charge for battery bank. index=" << i
			 << ", charge=" << battery_bank[tens_index] << battery_bank[ones_index] << "\n";

		result += ((10 * (battery_bank[tens_index] - '0')) + (battery_bank[ones_index] - '0'));
	}

	cout << "Total charge = " << result << endl;
	return 0;
}

int solve(const vector<string>& battery_banks)
{
	simple_solve_part_1(battery_banks);
	compute_total_max_joltage(battery_banks, 12);
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
