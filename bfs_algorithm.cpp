#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unordered_set>
#include <utility>
#include <chrono>
#include "bfs_algorithm.h"
#include "maze_generation.h"

using namespace std;

struct hash_pair {
    size_t operator()(const pair<int, int>& p) const {
        return ((hash<int>()(p.first)) ^ (hash<int>()(p.second) << 1));
    }
};

BFSStats bfs_stats;

vector<pair<vector<vector<int>>, pair<int, int>>> bfs_algorithm(vector<vector<int>> maze) {

    auto startTime = chrono::high_resolution_clock::now();

    int rows = maze.size();
    int cols = maze.empty() ? 0 : maze[0].size();
    int threshold = (rows + cols) / 10;
    if (threshold < 1) threshold = 1;

    queue<pair<int, int>> path;

    unordered_set<pair<int, int>, hash_pair> visited;

    pair<int, int> player = { spawn_coordinates.first, spawn_coordinates.second };
    path.push(player);
    visited.insert(player);

    vector<pair<vector<vector<int>>, pair<int, int>>> snapshots;
    int snapshotCounter = 0;
    int totalMoves = 0;

    srand(time(0));

    bool found = false;

    while (!path.empty() && !found) {

        ++totalMoves;

        player = path.front();
        path.pop();

        maze[player.first][player.second] = 3;
        maze[goal_coordinates.first][goal_coordinates.second] = 2;

        if (++snapshotCounter % threshold == 0) {
            snapshots.emplace_back(maze, player);
        }

        int random_choice = rand() % 4 + 1;

        switch (random_choice) {
        case 1: {
            // upward
            pair<int, int> nbr = { player.first - 1, player.second };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
            // downward
            nbr = { player.first + 1, player.second };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
            // left
            nbr = { player.first, player.second - 1 };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
            // right
            nbr = { player.first, player.second + 1 };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
        } break;
        case 2: {
            // downward
            pair<int, int> nbr = { player.first + 1, player.second };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
            // right
            nbr = { player.first, player.second + 1 };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
            // upward
            nbr = { player.first - 1, player.second };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
            // left
            nbr = { player.first, player.second - 1 };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
        } break;
        case 3: {
            // left
            pair<int, int> nbr = { player.first, player.second - 1 };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
            // downward
            nbr = { player.first + 1, player.second };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
            // right
            nbr = { player.first, player.second + 1 };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
            // upward
            nbr = { player.first - 1, player.second };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
        } break;
        case 4: {
            // right
            pair<int, int> nbr = { player.first, player.second + 1 };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
            // left
            nbr = { player.first, player.second - 1 };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
            // downward
            nbr = { player.first + 1, player.second };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
            // upward
            nbr = { player.first - 1, player.second };
            if ((maze[nbr.first][nbr.second] == 0 || maze[nbr.first][nbr.second] == 2)
                && visited.find(nbr) == visited.end()) {
                path.push(nbr);
                visited.insert(nbr);
                if (nbr == goal_coordinates) { found = true; break; }
            }
        } break;
        }
    }

    snapshots.emplace_back(maze, player);

    auto endTime = chrono::high_resolution_clock::now();
    bfs_stats.totalMoves = totalMoves;
    bfs_stats.elapsedMillis = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

    return snapshots;
}