void graph(int n, int root = -1, int m = -1)
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
    if (m != -1)
        for (int i = n; i <= m; i++)
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
    for (auto [x, y] : edge)
        fout << x << " " << y << endl;
}
