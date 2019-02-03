#include <iostream>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <tuple>
#include <ctime>
#include <unordered_map>

using namespace std;

constexpr double INF = 1e100;

class Environment {
public:
  Environment(const vector<string>& maze) : maze(maze), N(maze.size()), M(maze[0].size()) {
    for (int i = 0; i < N; ++ i) {
      for (int j = 0; j < M; ++ j) {
        if (maze[i][j] == 'S') {
          start_x = i; start_y = j;
        }
      }
    }
    x = start_x; y = start_y;
  }

  int start() {
    return start_x * N + start_y;
  }

  tuple<int, double, bool> step(int action) {
    /*
    cout << "-------------------" << endl;
    cout << x << " " << y << endl;
    cout << action << endl;
    */
    int xx = x + dir[action];
    int yy = y + dir[action + 1];

    if (xx < 0 || xx >= N || yy < 0 || yy >= M || maze[xx][yy] == '*') {
      xx = x; yy = y;
    } else {
      x = xx; y = yy;
    }

    /*
    cout << x << " " << y << endl;
    cout << "-------------------" << endl;
    */

    return make_tuple(xx * M + yy, -1, maze[xx][yy] == 'G');
  }

  void reset() {
    x = start_x; y = start_y;
  }

  char get_maze(int x, int y) const {
    return maze[x][y];
  }

  int n() const {
    return N;
  }

  int m() const {
    return M;
  }

  void print() const {
    for (const string& x : maze) {
      cout << x << endl;
    }
  }

private:
  vector<string> maze;
  int N, M;
  int x, y;
  int start_x, start_y;

  // 0: left, 1: up, 2: right, 3: down
  int dir[5] = {0, -1, 0, 1, 0};
};

vector<string> ReadMaze(const string filename) {
  ifstream fin(filename);
  vector<string> maze;
  string line;
  while (fin >> line) {
    maze.push_back(line);
  }
  fin.close();
  return maze;
}

int BestAction(unordered_map<int, double>& Q, int state) {
  int action = -1;
  double best_q = -INF;
  for (int a = 0; a < 4; ++ a) {
    if (Q[(state << 2) + a] > best_q) {
      action = a;
      best_q = Q[(state << 2) + a];
    }
  }
  return action;
}

unordered_map<int, double> QLearning(
  Environment& env, int num_episode, int max_episode,
  double gamma, double epsilon, double learning_rate) {

  unordered_map<int, double> Q;

  while (num_episode --) {
    // cout << "episode = " << num_episode << endl;

    env.reset();
    int state = env.start();

    int rest_episode = max_episode;
    while (rest_episode --) {
      // cout << "state = " << state << endl;
      const double explore_pr = rand() / RAND_MAX;
      int action = (explore_pr <= epsilon) ? rand() % 4 : BestAction(Q, state);

      // cout << "action = " << action << endl;

      double prev_q = Q[(state << 2) + action];

      tuple<int, int, bool> next = env.step(action);
      int next_state = get<0>(next);

      // if (next_state == state) continue;

      // cout << "next state = " << next_state << endl;
      double reward = get<1>(next);
      int next_action = BestAction(Q, next_state);
      bool terminated = get<2>(next);

      double next_q = terminated ? 0 : Q[(next_state << 2) + next_action];
      Q[(state << 2) + action] = (1 - learning_rate) * prev_q + learning_rate * (reward + gamma * next_q);

      state = next_state;

      if (terminated) break;
    }
  }

  return Q;
}

void PrintPolicyValueFiles(const string policy_filename, const string value_filename,
  const unordered_map<int, double>& Q, const Environment& env) {
  ofstream pout(policy_filename);
  ofstream vout(value_filename);

  const int N = env.n();
  const int M = env.m();
  // cout << "N = " << N << endl;
  // cout << "M = " << M << endl;

  // env.print();

  // cout << "Policy + Value:" << endl;
  for (int x = 0; x < N; ++ x) {
    for (int y = 0; y < M; ++ y) {
      if (env.get_maze(x, y) != '*') {
        double value = -INF;
        int policy = -1;
        for (int a = 0; a < 4; ++ a) {
          const auto it = Q.find(((x * M + y) << 2) + a);
          if (it != Q.end() && it->second > value) {
            value = it->second;
            policy = a;
          }
        }
        // cout << x << " " << y << " " < policy << " " << value << endl;
        pout << x << " " << y << " " << policy << endl;
        vout << x << " " << y << " " << value << endl;
      }
    }
  }

  pout.close();
  vout.close();
}

void PrintQValueFile(const string q_filename, const unordered_map<int, double>& Q, const Environment& env) {
  ofstream qout(q_filename);
  const int M = env.m();
  for (const auto& p : Q) {
    int x = (p.first >> 2) / M;
    int y = (p.first >> 2) % M;
    qout << x << " " << y << " " << (p.first & 3) << " " << p.second << endl;
  }
  qout.close();
}

int main(int argc, char* argv[]) {
  srand(time(NULL));

  Environment env(ReadMaze(argv[1]));

  const int num_episode = atoi(argv[5]);
  const int max_episode_length = atoi(argv[6]);
  const double learning_rate = atof(argv[7]);
  const double gamma = atof(argv[8]);
  const double epsilon = atof(argv[9]);

  // QLearning(env, 1000, 20, 0.9, 0, 0.8);
  unordered_map<int, double> Q =
    QLearning(env, num_episode, max_episode_length, gamma, epsilon, learning_rate);

  PrintPolicyValueFiles(argv[2], argv[4], Q, env);

  PrintQValueFile(argv[3], Q, env);


  // QLearning(env, 10000, 20, 0.9, 0.05, 0.8);
  // QLearning(env, 10000, 20, 0.9, 0.05, 0.8);
  // QLearning(env, 10000000, 100, 0.9, 0.1, 0.8);

  return 0;
}
