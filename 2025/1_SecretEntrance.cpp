#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using u_int = unsigned int;

using namespace std;

u_int find_num_times_cross_zero(const vector<string>& lines, const u_int start_position,
								const u_int lock_max)
{
	const u_int num_lock_ticks = lock_max + 1;

	u_int result = 0;
	int curr_position = start_position;

	for (const auto& line : lines)
	{
		const char direction = line[0];
		int i_num_ticks = stoi(line.substr(1));

		result += (i_num_ticks / num_lock_ticks); // revolutions

		i_num_ticks = i_num_ticks % num_lock_ticks;
		if (0 == i_num_ticks)
		{
			continue;
		}

		if ('L' == direction)
		{
			const int new_position = curr_position - i_num_ticks;
			if (curr_position != 0 && new_position <= 0)
			{
				++result;
			}
			curr_position = new_position < 0 ? num_lock_ticks + new_position : new_position;
		}
		else
		{
			const int new_position = curr_position + i_num_ticks;
			if (new_position > lock_max)
			{
				++result;
			}
			curr_position = new_position > lock_max ? new_position - num_lock_ticks : new_position;
		}
	}

	return result;
}

u_int find_num_times_land_on_zero(const vector<string>& lines, const int start_position,
								  const int lock_max)
{
	const int num_lock_ticks = lock_max + 1;
	int curr = start_position;
	int result = 0;

	for (const auto& line : lines)
	{
		const char direction = line[0];
		int value = stoi(line.substr(1));
		value = value % num_lock_ticks;

		if ('L' == direction)
		{
			value = num_lock_ticks - value;
		}

		curr = (curr + value) % num_lock_ticks;
		if (0 == curr)
		{
			++result;
		}
	}

	return result;
}

int solve(vector<string>& input)
{
	cout << "Part 1 : result=" << find_num_times_land_on_zero(input, 50, 99) << "\n";
	cout << "Part 2 : result=" << find_num_times_cross_zero(input, 50, 99) << "\n";
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
