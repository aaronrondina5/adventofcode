#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

using u_ll = unsigned long long;
using u_int = unsigned int;

using Range = pair<u_ll, u_ll>;

// Part 1
// First solution could be to loop through each value,
// then convert to a string, and pass to some is_periodic function
// that used pointers and checked the strings.
// However, this then adds time complexity on the number of characters
// It also requires that we look at each integer in a range.

// Instead, we could try to mathematically generate the values within the ranges.
//
// METHOD
// start at using the power / 2
// ie, if our number is 1234, we want to start with 10
// so that we start with 1010, then try 1111, then 1212, then 1313
// if 12345, 10^4, then we have 100100 as the first possible

u_int get_power(u_ll value)
{
	u_int power = 0;
	while (value > 9)
	{
		value /= 10;
		++power;
	}
	return power;
}

u_ll get_number_double_repeat_between(const u_ll start, const u_ll end)
{
	u_ll result = 0;

	u_int power = get_power(start);
	u_ll repeat_base = pow(10, power / 2);

	u_ll possible_repeat = 0;
	u_ll count = 0;
	while (repeat_base < end)
	{
		u_int repeat_base_power = get_power(repeat_base);
		possible_repeat = (pow(10, repeat_base_power + 1) * repeat_base) + repeat_base;

		if (possible_repeat < start)
		{
			++repeat_base;
			continue;
		}

		if (possible_repeat > end)
		{
			break;
		}

		result += possible_repeat;
		++repeat_base;
	}

	return result;
}

u_ll get_total_double_repeats(vector<Range>& ranges)
{
	u_ll result = 0;
	for (Range& range : ranges)
	{
		result += get_number_double_repeat_between(range.first, range.second);
	}
	return result;
}

// Part 2
// Extend our solution by generating the possible periodic integers

// This is the general pattern for periodics:
//
//      x + (10 * x) + (10^2 * x) + (10^3 * x) + ...
//      xy + (10^2 * xy) + (10^4*xy) + (10^6 * xy) + ...
//      xyz + (10^3 * xyz) + (10^6*xyz) + (10^9 * xyz) + ...
//      etc.
u_ll get_periodic_repeats_between(const u_ll start, const u_ll end)
{
	u_ll result = 0;

	u_int power_end = get_power(end);
	u_ll max_base = pow(10, (power_end + 1) / 2);
	u_ll min_base = 1; // maybe change later

	unordered_set<u_ll> checked;

	// loop through values up to the max base
	for (u_ll base = min_base; base < max_base; ++base)
	{
		const u_int base_power = get_power(base) + 1;
		u_int power = base_power;
		u_ll curr_value = (pow(10, power) * base) + base;

		if (curr_value > end)
		{
			// First possible repeat value too big. Will exit range
			break;
		}

		// loop through repeats -> base: xy -> xyxy, xyxyxy, etc.
		while (curr_value <= end)
		{
			if (!checked.contains(curr_value) && curr_value >= start)
			{
				checked.emplace(curr_value);
				result += curr_value;
			}

			power += base_power;
			curr_value = (pow(10, power) * base) + curr_value;
		}
	}

	return result;
}

u_ll get_total_periodic_repeats(vector<Range>& ranges)
{
	u_ll result = 0;
	for (Range& range : ranges)
	{
		result += get_periodic_repeats_between(range.first, range.second);
	}
	return result;
}

void time_wrap(function<void(vector<Range>&)> func, vector<Range>& input)
{
	auto start = std::chrono::high_resolution_clock::now();
	func(input);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	cout << "time_seconds=" << elapsed.count() << "\n";
}

void parse_input(vector<Range>& result, const string& input)
{
	size_t start = 0;
	while (start < input.size())
	{
		size_t i_dash = input.find('-', start);
		size_t i_comma = input.find(',', start);
		if (i_dash == string::npos)
			break;

		size_t end_pos = (i_comma == string::npos) ? input.size() : i_comma;
		result.emplace_back(Range{stoull(input.substr(start, i_dash - start)),
								  stoull(input.substr(i_dash + 1, end_pos - i_dash - 1))});

		start = (i_comma == string::npos) ? input.size() : i_comma + 1;
	}
}

int solve(const vector<string>& input_lines)
{
	vector<Range> ranges;
	parse_input(ranges, input_lines[0]);

	auto solve_1 = [](vector<Range>& ranges)
	{
		u_ll result = get_total_double_repeats(ranges);
		std::cout << "Part 1 : get_number_double_repeat_between=" << result << "\n";
	};
	time_wrap(solve_1, ranges);

	auto solve_2 = [](vector<Range>& ranges)
	{
		u_ll result = get_total_periodic_repeats(ranges);
		std::cout << "Part 2 : get_total_periodic_repeats=" << result << "\n";
	};
	time_wrap(solve_2, ranges);

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
