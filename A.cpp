#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

constexpr int START = 50;
constexpr int LOCK_MAX = 99;

void protect_input(const int start_position, const int lock_max)
{
	if (lock_max < 0 || lock_max < start_position)
	{
		cerr << "Invalid lock max or stargin position. start_position=" << start_position
			 << ", lock_max=" << lock_max;
		exit(1);
	}
}

void protect_line_direction(const char direction)
{
	if ('L' != direction && 'R' != direction)
	{
		cerr << "Invalid direction=" << direction << "\n";
		exit(2);
	}
}

int solve_part2(const vector<string>& lines, const int start_position, const int lock_max)
{
	protect_input(start_position, lock_max);

	const int num_lock_ticks = lock_max + 1;
	int curr_position = start_position;
	int result = 0;

	for (const auto& line : lines)
	{
		const char direction = line[0];
		protect_line_direction(direction);

		int i_num_ticks = stoi(line.substr(1));

		cout << "current_position=" << curr_position << "; directive=" << direction << i_num_ticks
			 << ", curr_result=" << result << "\n";

		const int revolutions = i_num_ticks / num_lock_ticks;
		result += revolutions;

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

	cout << "Result is = " << result << endl;

	return 0;
}

int solve_part1(const vector<string>& lines, const int start_position, const int lock_max)
{
	protect_input(start_position, lock_max);

	const int num_lock_ticks = lock_max + 1;
	int curr = start_position;
	int result = 0;

	for (const auto& line : lines)
	{
		const char direction = line[0];
		protect_line_direction(direction);

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

	cout << "Result is=" << result << endl;

	return 0;
}

int solve(vector<string>& input)
{
	return solve_part2(input, START, LOCK_MAX);
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
