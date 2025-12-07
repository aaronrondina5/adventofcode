#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

using u_ll = unsigned long long;
using u_int = unsigned int;

u_int get_power(u_ll value)
{
	if (value < 0)
	{
		exit(1);
	}

	u_int power = 0;
	while (value > 9)
	{
		value /= 10;
		++power;
	}
	return power;
}

void add_repeats_helper_part_2(u_ll& result, u_ll start, u_ll end)
{
	// now need to loop through possibilities 1 through power
	// x + (10 * x) + (10^2 * x) + ...
	// xy + (10^2 * xy) + (10^4*xy)
	// xyz + (10^3 * xyz) + (10^6*xyz) + ...
	cout << "[ " << start << " - " << end << " ] \n";

	u_int power_end = get_power(end);
	u_ll max_base = pow(10, (power_end + 1 ) / 2);
	cout << "max_base=" << max_base << "\n";

	unordered_set<u_ll> checked;

	// loop through values up to the max base
	for (u_ll base = 1; base < max_base; ++base)
	{
		const u_int base_power = get_power(base) + 1;
		u_int power = base_power;
		u_ll curr_value = (pow(10, power) * base) + base;

		if (curr_value > end)
		{
			cout << "First value too big. Will exit range. base=" << base << ", curr_value=" << curr_value << "\n";
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

	cout << "\n";
}

void add_repeats_helper_part_1(u_ll& result, u_ll start, u_ll end)
{
	// what can we do?

	// what is the smallest possible repeat after a value?
	// 1234 -> 1313
	// you know it will be at least on the magnitude of 10^3

	// start at using the power / 2
	// ie, if our number is 1234, we want to start with 10
	// so that we start with 1010, then try 1111, then 1212, then 1313
	// if 12345, 10^4, then we have 100100 as the first possible

	// we can generate all the possible repeats until we get higher than end maybe?

	cout << "Checking range. start=" << start << "; end=" << end << "\n";

	u_int power = get_power(start);
	u_ll repeat_base = pow(10, power / 2);

	cout << "Determined repeat base. repeat_base=" << repeat_base << "; start=" << start << "\n";

	u_ll possible_repeat = 0;
	u_ll count = 0;
	while (repeat_base < end)
	{
		u_int repeat_base_power = get_power(repeat_base);
		possible_repeat = (pow(10, repeat_base_power + 1) * repeat_base) + repeat_base;

		cout << "Found possible repeat. value=" << possible_repeat << "\n";

		if (possible_repeat < start)
		{
			++repeat_base;
			continue;
		}

		if (possible_repeat > end)
		{
			cout << "Reached a repeat out of range." << "\n";
			break;
		}

		result += possible_repeat;
		++repeat_base;
	}
}

int solve()
{
	string line;
	getline(cin, line);

	stringstream begin_range;
	stringstream end_range;

	u_ll result = 0;

	bool on_begin = true;

	for (const char c : line)
	{
		if ('-' == c)
		{
			on_begin = false;
		}
		else if (',' == c)
		{
			add_repeats_helper_part_2(result, stoll(begin_range.str()), stoll(end_range.str()));

			on_begin = true;
			begin_range.str("");
			end_range.str("");
		}
		else
		{
			if (on_begin)
			{
				begin_range << c;
			}
			else
			{
				end_range << c;
			}
		}
	}

	add_repeats_helper_part_2(result, stoll(begin_range.str()), stoll(end_range.str()));

	cout << "result is " << result << endl;

	return 0;
}

int main()
{
	return solve();
}
