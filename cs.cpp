#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

struct Node {
    int row, col;  
    int g;          
    double h;      
    int parent;    
};

class Maze {
public:
    vector<string> grid;
    int rows, cols;
    int goalRow, goalCol;

    Maze(vector<string> g) {
        grid = g;
        rows = grid.size();
        cols = grid[0].size();
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                if (grid[r][c] == 'X') { goalRow = r; goalCol = c; }
    }

    // Check if cell is walkable (Empty '.', Flooded '~' or 'G', or Goal 'X')
    bool isFree(int r, int c) {
        if (r < 0 || r >= rows || c < 0 || c >= cols) return false;
        char cell = grid[r][c];
        return cell == '.' || cell == '~' || cell == 'G' || cell == 'X';
    }

    // Heuristic 1: Euclidean Distance
    double h1(int r, int c) {
        double dr = r - goalRow, dc = c - goalCol;
        return sqrt(dr * dr + dc * dc);
    }

    // Heuristic 2: Manhattan Distance
    double h2(int r, int c) {
        return abs(r - goalRow) + abs(c - goalCol);
    }

    double heuristic(int type, int r, int c) {
        return (type == 1) ? h1(r, c) : h2(r, c);
    }

    int search(int startR, int startC, bool useAstar, int hType,
               int &expanded, vector<pair<int,int> > &path) {
        vector<Node> all;                
        vector<int> open;                
        vector<vector<bool> > visited(rows, vector<bool>(cols, false));

        Node start = {startR, startC, 0, heuristic(hType, startR, startC), -1};
        all.push_back(start);
        open.push_back(0);
        expanded = 0;

        // 5 Actions: Up, Down, Left, Right, Stay
        int dr[5] = {-1, 1, 0, 0, 0};
        int dc[5] = {0, 0, -1, 1, 0};

        while (!open.empty()) {
            int bestPos = 0;
            for (int i = 1; i < (int)open.size(); i++) {
                Node &a = all[open[i]];
                Node &b = all[open[bestPos]];
                double fa = useAstar ? a.g + a.h : a.h;
                double fb = useAstar ? b.g + b.h : b.h;
                if (fa < fb) bestPos = i;
            }
            int curIdx = open[bestPos];
            open.erase(open.begin() + bestPos);
            Node cur = all[curIdx];

            if (visited[cur.row][cur.col]) continue;  
            visited[cur.row][cur.col] = true;
            expanded++;

            if (cur.row == goalRow && cur.col == goalCol) {
                int idx = curIdx;
                while (idx != -1) {
                    path.insert(path.begin(), make_pair(all[idx].row, all[idx].col));
                    idx = all[idx].parent;
                }
                return cur.g;
            }

            for (int k = 0; k < 5; k++) {
                int nr = cur.row + dr[k];
                int nc = cur.col + dc[k];
                if (isFree(nr, nc) && !visited[nr][nc]) {
                    Node child = {nr, nc, cur.g + 1, heuristic(hType, nr, nc), curIdx};
                    all.push_back(child);
                    open.push_back(all.size() - 1);
                }
            }
        }
        return -1;
    }

    void printPath(int startR, int startC, vector<pair<int,int> > &path) {
        vector<string> g = grid;
        for (int i = 0; i < (int)path.size(); i++) {
            int r = path[i].first, c = path[i].second;
            if (g[r][c] == '.' ||  g[r][c] == '~' || g[r][c] == 'G') g[r][c] = '*';
        }
        g[startR][startC] = 'A';
        for (int r = 0; r < rows; r++) cout << "   " << g[r] << endl;
    }
};

int main() {
    // Standard Maze layout matching assignment description
    vector<string> layout;
    layout.push_back("....X");
    layout.push_back("#~###");
    layout.push_back(".~...");
    layout.push_back("#~#.#");
    layout.push_back(".....");

    Maze maze(layout);

    // 5 Start States
    int starts[5][2] = { {4,0}, {4,4}, {2,2}, {0,0}, {2,0} };

    cout << "Start | Algorithm       | Heuristic | Cost | Expanded\n";
    cout << "---------------------------------------------------\n";

    for (int s = 0; s < 5; s++) {
        int sr = starts[s][0], sc = starts[s][1];
        for (int alg = 0; alg < 2; alg++) {          
            for (int h = 1; h <= 2; h++) {          
                int expanded = 0;
                vector<pair<int,int> > path;
                int cost = maze.search(sr, sc, alg == 1, h, expanded, path);
                cout << "(" << sr << "," << sc << ") | "
                     << (alg == 0 ? "Greedy         " : "A*             ") << "| "
                     << (h == 1 ? "h1 Euclid  " : "h2 Manhat. ") << "| "
                     << cost << "    | " << expanded << endl;

                if (alg == 1 && h == 2) {
                    maze.printPath(sr, sc, path);
                    cout << endl;
                }
            }
        }
        cout << "---------------------------------------------------\n";
    }
    return 0;
}