#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <chrono>
#include "random_walk.h"
#include "maze_generation.h"

using namespace std;

RandomWalkStats random_stats;

vector<pair<vector<vector<int>>, pair<int, int>>> random_walk_algorithm(vector<vector<int>> maze) {

    auto startTime = chrono::high_resolution_clock::now();

    int rows = maze.size();
    int cols = maze.empty() ? 0 : maze[0].size();
    int threshold = (rows + cols) / 10;
    if (threshold < 1) threshold = 1;

    pair<int, int> player = spawn_coordinates;

    vector<pair<vector<vector<int>>, pair<int, int>>> snapshots;
    int snapshotCounter = 0;
    int totalMoves = 0;

    srand(static_cast<unsigned>(time(0)));

    vector<pair<int, int>> frontier;
    auto add_neighbors = [&](int r, int c) {
        // up
        if (r > 0 && (maze[r - 1][c] == 0 || maze[r - 1][c] == 2))
            frontier.emplace_back(r - 1, c);
        // down
        if (r < rows - 1 && (maze[r + 1][c] == 0 || maze[r + 1][c] == 2))
            frontier.emplace_back(r + 1, c);
        // left
        if (c > 0 && (maze[r][c - 1] == 0 || maze[r][c - 1] == 2))
            frontier.emplace_back(r, c - 1);
        // right
        if (c < cols - 1 && (maze[r][c + 1] == 0 || maze[r][c + 1] == 2))
            frontier.emplace_back(r, c + 1);
        };

    add_neighbors(player.first, player.second);

    while (true) {
        for (auto it = frontier.begin(); it != frontier.end(); ++it) {
            if (*it == goal_coordinates) {
                player = goal_coordinates;
                maze[player.first][player.second] = 2;
                ++totalMoves;
                if (++snapshotCounter % threshold == 0) {
                    snapshots.emplace_back(maze, player);
                }
                snapshots.emplace_back(maze, player);
                goto finish;
            }
        }

        if (frontier.empty()) break;

        int idx = rand() % frontier.size();
        auto next = frontier[idx];

        frontier.erase(frontier.begin() + idx);

        player = next;
        maze[player.first][player.second] = 3;
        ++totalMoves;

        add_neighbors(player.first, player.second);

        if (++snapshotCounter % threshold == 0) {
            snapshots.emplace_back(maze, player);
        }
    }

finish:

    auto endTime = chrono::high_resolution_clock::now();
    random_stats.totalMoves = totalMoves;
    random_stats.elapsedMillis = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    return snapshots;
}