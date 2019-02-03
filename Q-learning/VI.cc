#include <iostream>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <vector>

using namespace std;

constexpr double INF = 1e100;
constexpr int dir[5] = {0, -1, 0, 1, 0};

vector<string> maze;
int N, M;

void ReadMaze() {
  string line;
  while (cin >> line) {
    maze.push_back(line);
  }
  N = maze.size();
  M = maze[0].size();
}

void ValueIteration(int iterations, double reward, double gamma) {
  vector<vector<double>> V[2];
  vector<vector<int>> P;

  for (int i = 0; i < N; ++ i) {
    V[0].push_back(vector<double>(M, 0));
    V[1].push_back(vector<double>(M, 0));
    P.push_back(vector<int>(M, 0));
  }

  for (int i = 1; i <= iterations; ++ i) {
    int cur = i & 1;
    for (int x = 0; x < N; ++ x) {
      for (int y = 0; y < M; ++ y) {
        if (maze[x][y] == '*' || maze[x][y] == 'G') {
          V[cur][x][y] = 0.0;
          continue;
        }
        V[cur][x][y] = -INF;
        for (int a = 0; a < 4; ++ a) {
          int xx = x + dir[a];
          int yy = y + dir[a + 1];
          if (xx < 0 || xx >= N || yy < 0 || yy >= M || maze[xx][yy] == '*') {
            xx = x; yy = y;
          }
          double q = reward + gamma * V[1 - cur][xx][yy];
          // V[cur][x][y] = max(V[cur][x][y], q);
          if (q > V[cur][x][y]) {
            V[cur][x][y] = q;
            P[x][y] = a;
          }
        }
      }
    }
  }

  cout << "Value:" << endl;
  for (int x = 0; x < N; ++ x) {
    for (int y = 0; y < M; ++ y) {
      if (maze[x][y] != '*') {
        cout << x << " " << y << " " << V[iterations & 1][x][y] << endl;
      }
    }
  }
  cout << "Policy:" << endl;
  for (int x = 0; x < N; ++ x) {
    for (int y = 0; y < M; ++ y) {
      if (maze[x][y] != '*') {
        cout << x << " " << y << " " << P[x][y] << endl;
      }
    }
  }
}

int main() {
  ReadMaze();

  cout << N << endl;
  cout << M << endl;

  ValueIteration(5, -1, 0.9);

  ValueIteration(1000, -1, 0.9);

  ValueIteration(10000, -1, 0.9);
  
  return 0;
}
