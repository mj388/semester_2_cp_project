#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unordered_set>
#include <utility>
#include <chrono>
#include "dfs_algorithm.h"
#include "maze_generation.h"

using namespace std;

typedef pair<int, int> Coord;
struct hash_pair {
    size_t operator()(const Coord& p) const {
        return (hash<int>()(p.first) ^ (hash<int>()(p.second) << 1));
    }
};

DFSStats dfs_stats;

vector<pair<vector<vector<int>>, pair<int, int>>> dfs_algorithm(vector<vector<int>> maze) {

    auto startTime = chrono::high_resolution_clock::now();

    int rows = maze.size();
    int cols = maze.empty() ? 0 : maze[0].size();
    int threshold = (rows + cols) / 10;
    if (threshold < 1) threshold = 1;

    vector<Coord> path;
    unordered_set<Coord, hash_pair> visited;

    Coord player = spawn_coordinates;
    path.push_back(player);

    vector<pair<vector<vector<int>>, pair<int, int>>> snapshots;
    int snapshotCounter = 0;
    int totalMoves = 0;

    srand(time(0));

    while (player != goal_coordinates) {

        ++totalMoves;

        player = path.back();
        path.pop_back();

        maze[player.first][player.second] = 3;
        maze[goal_coordinates.first][goal_coordinates.second] = 2;
        visited.insert(player);

        if (++snapshotCounter % threshold == 0) {
            snapshots.emplace_back(maze, player);
        }

        int random_choice = rand() % 4 + 1;
        switch (random_choice) {
        case 1:
            if ((maze[player.first - 1][player.second] == 0 || maze[player.first - 1][player.second] == 2)
                && visited.find({ player.first - 1, player.second }) == visited.end()) {
                path.push_back({ player.first - 1, player.second });
                visited.insert({ player.first - 1, player.second });
            }
            if ((maze[player.first + 1][player.second] == 0 || maze[player.first + 1][player.second] == 2)
                && visited.find({ player.first + 1, player.second }) == visited.end()) {
                path.push_back({ player.first + 1, player.second });
                visited.insert({ player.first + 1, player.second });
            }
            if ((maze[player.first][player.second - 1] == 0 || maze[player.first][player.second - 1] == 2)
                && visited.find({ player.first, player.second - 1 }) == visited.end()) {
                path.push_back({ player.first, player.second - 1 });
                visited.insert({ player.first, player.second - 1 });
            }
            if ((maze[player.first][player.second + 1] == 0 || maze[player.first][player.second + 1] == 2)
                && visited.find({ player.first, player.second + 1 }) == visited.end()) {
                path.push_back({ player.first, player.second + 1 });
                visited.insert({ player.first, player.second + 1 });
            }
            break;
        case 2:
            if ((maze[player.first + 1][player.second] == 0 || maze[player.first + 1][player.second] == 2)
                && visited.find({ player.first + 1, player.second }) == visited.end()) {
                path.push_back({ player.first + 1, player.second });
                visited.insert({ player.first + 1, player.second });
            }
            if ((maze[player.first][player.second + 1] == 0 || maze[player.first][player.second + 1] == 2)
                && visited.find({ player.first, player.second + 1 }) == visited.end()) {
                path.push_back({ player.first, player.second + 1 });
                visited.insert({ player.first, player.second + 1 });
            }
            if ((maze[player.first - 1][player.second] == 0 || maze[player.first - 1][player.second] == 2)
                && visited.find({ player.first - 1, player.second }) == visited.end()) {
                path.push_back({ player.first - 1, player.second });
                visited.insert({ player.first - 1, player.second });
            }
            if ((maze[player.first][player.second - 1] == 0 || maze[player.first][player.second - 1] == 2)
                && visited.find({ player.first, player.second - 1 }) == visited.end()) {
                path.push_back({ player.first, player.second - 1 });
                visited.insert({ player.first, player.second - 1 });
            }
            break;
        case 3:
            if ((maze[player.first][player.second - 1] == 0 || maze[player.first][player.second - 1] == 2)
                && visited.find({ player.first, player.second - 1 }) == visited.end()) {
                path.push_back({ player.first, player.second - 1 });
                visited.insert({ player.first, player.second - 1 });
            }
            if ((maze[player.first + 1][player.second] == 0 || maze[player.first + 1][player.second] == 2)
                && visited.find({ player.first + 1, player.second }) == visited.end()) {
                path.push_back({ player.first + 1, player.second });
                visited.insert({ player.first + 1, player.second });
            }
            if ((maze[player.first][player.second + 1] == 0 || maze[player.first][player.second + 1] == 2)
                && visited.find({ player.first, player.second + 1 }) == visited.end()) {
                path.push_back({ player.first, player.second + 1 });
                visited.insert({ player.first, player.second + 1 });
            }
            if ((maze[player.first - 1][player.second] == 0 || maze[player.first - 1][player.second] == 2)
                && visited.find({ player.first - 1, player.second }) == visited.end()) {
                path.push_back({ player.first - 1, player.second });
                visited.insert({ player.first - 1, player.second });
            }
            break;
        case 4:
            if ((maze[player.first][player.second + 1] == 0 || maze[player.first][player.second + 1] == 2)
                && visited.find({ player.first, player.second + 1 }) == visited.end()) {
                path.push_back({ player.first, player.second + 1 });
                visited.insert({ player.first, player.second + 1 });
            }
            if ((maze[player.first][player.second - 1] == 0 || maze[player.first][player.second - 1] == 2)
                && visited.find({ player.first, player.second - 1 }) == visited.end()) {
                path.push_back({ player.first, player.second - 1 });
                visited.insert({ player.first, player.second - 1 });
            }
            if ((maze[player.first + 1][player.second] == 0 || maze[player.first + 1][player.second] == 2)
                && visited.find({ player.first + 1, player.second }) == visited.end()) {
                path.push_back({ player.first + 1, player.second });
                visited.insert({ player.first + 1, player.second });
            }
            if ((maze[player.first - 1][player.second] == 0 || maze[player.first - 1][player.second] == 2)
                && visited.find({ player.first - 1, player.second }) == visited.end()) {
                path.push_back({ player.first - 1, player.second });
                visited.insert({ player.first - 1, player.second });
            }
            break;
        }
    }

    snapshots.emplace_back(maze, player);

    auto endTime = chrono::high_resolution_clock::now();
    dfs_stats.totalMoves = totalMoves;
    dfs_stats.elapsedMillis = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    return snapshots;
}