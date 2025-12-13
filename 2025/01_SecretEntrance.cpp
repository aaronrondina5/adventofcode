#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using u_int = unsigned int;

using namespace std;

constexpr char LEFT = 'L';
constexpr char RIGHT = 'R';

// Part 1
// iterate through the directives
// will have to account for left & right turns. easiest is to convert everything
// into a right turn by doing (right_turn = lock_max + 1 - left_turn)
// mod must happen first, so that its in the range of 0-lock_max
u_int find_num_times_land_on_zero(const vector<string>& lines, const int start_position,
								  const int lock_max) {
	const int num_lock_ticks = lock_max + 1;
	int curr = start_position;
	int result = 0;

	for (const auto& line : lines) {
		const char direction = line[0];
		int value = stoi(line.substr(1)) % num_lock_ticks;
		if (LEFT == direction) {
			value = num_lock_ticks - value;
		}

		curr = (curr + value) % num_lock_ticks;
		if (0 == curr) {
			++result;
		}
	}

	return result;
}

// Part 2
// similar to the first, but we cant convert everything to R, unless
// we do some other conversion on the other end.
// probably most clear to just convert to a negative value
// Now, we need to ADD the amount of times it goes into num ticks
// before mod-ing
//
// Then add the value based on the direction & check whether
// you cross the boundary 0.
// Now, there is a tricky edge case that will result in overcounting
// Say you land on 0 on one Left iteration & count this value. If you then
// have another left, you cannot count this value again.
// So you need to check this edge case.
u_int find_num_times_cross_zero(const vector<string>& lines, const u_int start_position,
								const u_int lock_max) {
	const u_int num_lock_ticks = lock_max + 1;
	u_int result = 0;
	int curr_position = start_position;

	for (const auto& line : lines) {
		const char direction = line[0];
		int increment = stoi(line.substr(1));
		// revolutions handling
		result += (increment / num_lock_ticks);
		increment = increment % num_lock_ticks;
		// direction handling
		increment = LEFT == direction ? -increment : increment;

		// if increment is 0, dont do any counting. we already handled the revolutions
		if (0 == increment)
			continue;

		const int new_position = curr_position + increment;

		if (LEFT == direction) {
			if (curr_position != 0 && new_position <= 0) {
				++result;
			}
			curr_position = new_position < 0 ? num_lock_ticks + new_position : new_position;
		} else {
			if (new_position > lock_max) {
				++result;
			}
			curr_position = new_position > lock_max ? new_position - num_lock_ticks : new_position;
		}
	}

	return result;
}

int solve(vector<string>& input) {
	cout << "Part 1 : result=" << find_num_times_land_on_zero(input, 50, 99) << "\n";
	cout << "Part 2 : result=" << find_num_times_cross_zero(input, 50, 99) << "\n";
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
