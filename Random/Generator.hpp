#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <set>
#include <vector>

using namespace std;
using ll = long long;

class Generator
{
    mt19937_64 gen;

  public:
    Generator() : gen(chrono::steady_clock::now().time_since_epoch().count())
    {
    }

    ll rd(ll l, ll r)
    {
        return uniform_int_distribution<ll>(l, r)(gen);
    }

    void str(int n, bool num = false)
    {
        string s;
        while (n--)
            if (!num)
                s += 'a' + rd(0, 25);
            else
                s += rd(0, 1) ? 'a' + rd(0, 25) : rd(0, 1) ? 'A' + rd(0, 25) : '0' + rd(0, 9);
        cout << s << endl;
    }

    void array(int n, int range = 1e5)
    {
        for (int i = 0; i < n; ++i)
            cout << rd(1, range) << ' ';
        cout << endl;
    }

    void permutation(int n)
    {
        vector<int> res(n);
        iota(res.begin(), res.end(), 1);
        shuffle(res.begin(), res.end(), gen);
        for (auto &p : res)
            cout << p << ' ';
        cout << endl;
    }

    void graph(int n, int m, bool weight = false, int range = 1e5, int root = -1)
    {
        vector<pair<int, int>> tmp;
        for (int i = 1; i < n; i++)
            tmp.emplace_back(i, rd(0, i - 1));
        vector<pair<int, int>> edge;
        set<pair<int, int>> uni;
        if (root == -1)
            root = rd(0, n - 1);
        for (auto [x, y] : tmp)
        {
            x = (x + root) % n + 1;
            y = (y + root) % n + 1;
            edge.emplace_back(x, y);
            uni.emplace(x, y);
        }
        int add = m - (n - 1);
        for (int i = 0; i < add; i++)
            while (true)
            {
                int x = rd(1, n), y = rd(1, n);
                if (x == y || uni.count({x, y}) || uni.count({y, x}))
                    continue;
                edge.emplace_back(x, y);
                uni.emplace(x, y);
                break;
            }
        shuffle(edge.begin(), edge.end(), gen);
        for (auto &[a, b] : edge)
            cout << a << ' ' << b << (weight ? ' ' + to_string(rd(1, range)) + '\n' : string(1, '\n'));
    }

    void tree(int n, bool weight = false, int range = 1e5, int root = -1)
    {
        graph(n, n - 1, weight, range, root);
    }
} g;
