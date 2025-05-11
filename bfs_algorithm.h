#include <vector>

using namespace std;

struct BFSStats {
    int totalMoves;
    long long elapsedMillis;
};

extern BFSStats bfs_stats;

vector<pair<vector<vector<int>>, pair<int, int>>> bfs_algorithm(vector<vector<int>> maze);