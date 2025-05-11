#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
using namespace std;

extern pair<int, int> spawn_coordinates;
extern pair<int, int> goal_coordinates;

vector<vector<int>> generate_maze(int rows, int columns);