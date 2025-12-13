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

constexpr char OPEN_SPACE = '.';
constexpr char START = 'S';
constexpr char BEAM_SPLITTER = '^';


// Part 1
// I think you could either recursively go down the beams and keep track
// of what cols have already been processed.
//
// I suppose you could also do a single row DP.
// The dp keeps track of beams heading down columns.
// If this changes, the state could contain num_beams
//
// vector<bool> beam_state(cols, false);
// if (manifold[i][j] == ^ && beam_state(j))
//     beam_state[j + 1] = true;
//     beam_state[j - 1] = true;
//     ++splits;
//
// But, we also need to remove the beam state from underneath the splitter.
// So, check the surrounding sides for splitters, and update the beam state to
// 
// 
// There is an issue if there are 2 next to each other.
// if you set the beam state to the next one to true, then it will think there is 
// a beam, even if its not on that col. SO we have to use indices.
// Initially, beam state indices will be set to rows.
// Beam state check is 
// if (beam_state[col] < col) 
//
// Time : O(rows * cols)
// Space : O(cols)

void print_beam_state(const vector<u_int>& beam_state, const u_int max) {
	const size_t n = beam_state.size();
	for (size_t i = 0; i < n; ++i) {
		cout << (beam_state[i] < max ? '|' : '.');
	}
	cout << endl;
}

u_ll number_splits(const vector<string>& manifold) {
	const u_int rows = manifold.size();
	const u_int cols = manifold[0].size();

	u_ll result = 0;
	vector<u_int> beam_state(cols, rows); // beams are all "below bottom"

	for (u_int row = 0; row < rows; ++row) {
		for (u_int col = 0; col < cols; ++col) {
			if (0 == row) {
				if (START == manifold[row][col])
					beam_state[col] = row; // beam going down this column from this row
			} else {
				if (BEAM_SPLITTER == manifold[row][col] && beam_state[col] < row) {
					// there is a splitter here & there is a beam from above
					++result;

					if (col > 0)
						beam_state[col - 1] = row; // beam going down left
					if (col < cols - 1)
						beam_state[col + 1] = row; // beam going down the right

					if ((0 == col || OPEN_SPACE == manifold[row][col - 1]) &&
						(cols - 1 == col || OPEN_SPACE == manifold[row][col + 1])) {
						// for the next iteration, there will be no beam.
						beam_state[col] = rows;
					}
				}
			}
		}
	}

	return result;
}

// Part 2 : get number of paths
// I feel like for part 2, you can still use the dp approach
//
// As you hit a split,
// num_paths[right] = num_paths[right] + num_paths[col];
// num_paths[left] = num_paths[left] + num_paths[col];
//
// You add the current value because it may have been updated by a neighbor
// also, it has its own state
// 2 down
//       |   | 
// . . . ^ . ^ . .
// . . | . | . . 

u_ll number_paths(const vector<string>& manifold) {
	const u_int rows = manifold.size();
	const u_int cols = manifold[0].size();

	vector<u_int> last_row_index_beam(cols, rows); // keep track of the beams above
	vector<u_ll> paths_to_end_positions(cols, 0); // there are 0 paths to begin with

	for (u_int row = 0; row < rows; ++row) {
		for (u_int col = 0; col < cols; ++col) {
			if (0 == row) {
				if (START == manifold[row][col]) {
					// beam going down this column from this row.
					// this is currently the only path
					paths_to_end_positions[col] = 1;
					last_row_index_beam[col] = row;
				}
			} else {
				if (BEAM_SPLITTER == manifold[row][col] && last_row_index_beam[col] < row) {
					// there is a splitter here & there is a beam from above
					if (col > 0) {
						// beam splits to the left. Update the last beam row index for the left col
						// and also update the number of paths to the left
						last_row_index_beam[col - 1] = row;
						paths_to_end_positions[col - 1] = paths_to_end_positions[col - 1] + paths_to_end_positions[col];
					}
	
					if (col < cols - 1) {
						// beam splits to the right. Update the last beam row index for the right col
						// add number of paths (check if you have already counted the +1)
						last_row_index_beam[col + 1] = row;
						paths_to_end_positions[col + 1] = paths_to_end_positions[col + 1] + paths_to_end_positions[col];
					}

					if ((0 == col || OPEN_SPACE == manifold[row][col - 1]) &&
						(cols - 1 == col || OPEN_SPACE == manifold[row][col + 1])) {
						// for the next iteration, there will be no beam.
						// reset the last row index & remove the number of paths to this position
						last_row_index_beam[col] = rows;
						paths_to_end_positions[col] = 0;
					}
				}
			}
		}
	}

	// so now, our paths_to_end_positions contains the final locations of the beams
	// and the number of ways to get to each location. sum and return.
	return accumulate(paths_to_end_positions.begin(), paths_to_end_positions.end(), 0ULL);
}

int solve(const vector<string>& input_lines) {
	u_ll result_1 = number_splits(input_lines);
	cout << "Part 1 : number_splits=" << result_1 << endl;

	u_ll result_2 = number_paths(input_lines);
	cout << "Part 2 : number_splits=" << result_2 << endl;
	return 0;
}

void get_input_as_vector(vector<string>& input) {
	string line;
	while (getline(cin, line)) {
		input.push_back(line);
	}
}

int main() {
	vector<string> input;
	get_input_as_vector(input);

	if (0 != solve(input)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
