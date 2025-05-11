#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <unordered_set>
using namespace std;


// Hash function for unordered set
struct hash_pair {
	size_t operator()(const pair<int, int>& p) const {
		return ((hash<int>()(p.first)) ^ (hash<int>()(p.second) << 1));
	}
};

// Function to add two pairs
pair<int, int> addPairs(pair<int, int> a, pair<int, int> b) {
	return { a.first + b.first, a.second + b.second };
}


// Directions to move in the maze
vector<pair<int, int>> directions = {
	{-2, 0},  // Up
	{2, 0},   // Down
	{0, -2},  // Left
	{0, 2}    // Right
};

// Random number generator
random_device rd;
mt19937 rng(rd());

// Global spawn coordinates and goal coordinates
pair <int, int> spawn_coordinates;
pair <int, int> goal_coordinates;

// Function to generate a maze
vector<vector<int>> generate_maze(int rows, int columns) {

	// Initialize the maze with all walls
	vector<vector<int>> maze(rows, vector<int>(columns, 1));

	// Coordinates of goal
	uniform_int_distribution<> distrib1(5, rows - 5);
	uniform_int_distribution<> distrib2(5, columns - 5);
	int randomNumber1 = distrib1(rng);
	int randomNumber2 = distrib2(rng);
	if (randomNumber1 % 2 == 0) {
		randomNumber1 += 1;
		if (randomNumber1 > rows - 5) randomNumber1 -= 2; // prevent overflow beyond range
	}
	if (randomNumber2 % 2 == 0) {
		randomNumber2 += 1;
		if (randomNumber2 > rows - 5) randomNumber2 -= 2;
	}
	maze[randomNumber1][randomNumber2] = 0;
	pair<int, int> maze_bot = { randomNumber1, randomNumber2 };

	// Stack to keep map of path
	vector<pair<int, int>> path;

	// Unordered set to keep track of visited cells
	unordered_set<pair<int, int>, hash_pair> visited;

	// Loop to generate maze
	do {
		path.push_back(maze_bot);
		visited.insert(maze_bot);

		pair<int, int> next_cell;

		while (true) {
			shuffle(directions.begin(), directions.end(), rng);;
			if (
				visited.find(addPairs(maze_bot, directions[0])) == visited.end()
				&& (addPairs(maze_bot, directions[0]).first > 0
					&& addPairs(maze_bot, directions[0]).first < rows - 1)
				&& (addPairs(maze_bot, directions[0]).second > 0
					&& addPairs(maze_bot, directions[0]).second < columns - 1)
				&& maze[((addPairs(maze_bot, directions[0])).first + maze_bot.first) / 2][((addPairs(maze_bot, directions[0])).second + maze_bot.second) / 2] != 0
				) {
				next_cell = addPairs(maze_bot, directions[0]);
				break;
			}
			else if (
				visited.find(addPairs(maze_bot, directions[1])) == visited.end()
				&& (addPairs(maze_bot, directions[1]).first > 0
					&& addPairs(maze_bot, directions[1]).first < rows - 1)
				&& (addPairs(maze_bot, directions[1]).second > 0
					&& addPairs(maze_bot, directions[1]).second < columns - 1)
				&& maze[((addPairs(maze_bot, directions[1])).first + maze_bot.first) / 2][((addPairs(maze_bot, directions[1])).second + maze_bot.second) / 2] != 0
				) {
				next_cell = addPairs(maze_bot, directions[1]);
				break;
			}
			else if (
				visited.find(addPairs(maze_bot, directions[2])) == visited.end()
				&& (addPairs(maze_bot, directions[2]).first > 0
					&& addPairs(maze_bot, directions[2]).first < rows - 1)
				&& (addPairs(maze_bot, directions[2]).second > 0
					&& addPairs(maze_bot, directions[2]).second < columns - 1)
				&& maze[((addPairs(maze_bot, directions[2])).first + maze_bot.first) / 2][((addPairs(maze_bot, directions[2])).second + maze_bot.second) / 2] != 0
				) {
				next_cell = addPairs(maze_bot, directions[2]);
				break;
			}
			else if (visited.find(addPairs(maze_bot, directions[3])) == visited.end()
				&& (addPairs(maze_bot, directions[3]).first > 0
					&& addPairs(maze_bot, directions[3]).first < rows - 1)
				&& (addPairs(maze_bot, directions[3]).second > 0
					&& addPairs(maze_bot, directions[3]).second < columns - 1)
				&& maze[((addPairs(maze_bot, directions[3])).first + maze_bot.first) / 2][((addPairs(maze_bot, directions[3])).second + maze_bot.second) / 2] != 0
				) {
				next_cell = addPairs(maze_bot, directions[3]);
				break;
			}
			else {
				next_cell = path.back();
				path.pop_back();
				if (!path.empty()) {
					maze_bot = path.back();
				}
				else {
					break;
				}
			}
		}

		// Update the maze
		if (next_cell.first != 0 && next_cell.second != 0) {
			maze[next_cell.first][next_cell.second] = 0;
			maze[(next_cell.first + maze_bot.first) / 2][(next_cell.second + maze_bot.second) / 2] = 0;
			visited.insert({ (next_cell.first + maze_bot.first) / 2, (next_cell.second + maze_bot.second) / 2 });
			maze_bot = next_cell;
		};

	} while (!path.empty());

	// Goal
	maze[randomNumber1][randomNumber2] = 2;
	goal_coordinates = { randomNumber1, randomNumber2 };

	// Set spawn point
	uniform_int_distribution<> distrib3(1, 4);
	int spawn_point = distrib3(rng);
	switch (spawn_point) {
	case 1:
		spawn_coordinates = { 1, 1 };
		break;
	case 2:
		spawn_coordinates = { rows - 2, 1 };
		break;
	case 3:
		spawn_coordinates = { 1, columns - 2 };
		break;
	case 4:
		spawn_coordinates = { rows - 2, columns - 2 };
		break;
	}
	
	maze[spawn_coordinates.first][spawn_coordinates.second] = 3;

	return maze;
}
