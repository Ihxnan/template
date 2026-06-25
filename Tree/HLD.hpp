template <class T> struct SegmentTree
{
    int n;
    vector<T> in;
    vector<T> tree, tag;

    SegmentTree(int n) : n(n), in(n + 1), tree(4 * n), tag(4 * n)
    {
    }

    SegmentTree(vector<T> &arr) : n(arr.size() - 1), in(arr), tree(4 * n), tag(4 * n)
    {
        build();
    }

    void push_up(int p)
    {
        tree[p] = tree[ls(p)] + tree[rs(p)];
    }

    void build(int p, int l, int r)
    {
        if (l == r)
        {
            tree[p] = in[l];
            return;
        }
        int mid = l + r >> 1;
        build(ls(p), l, mid);
        build(rs(p), mid + 1, r);
        push_up(p);
    }

    void change(int p, int l, int r, T k)
    {
        tree[p] += (r - l + 1) * k;
        tag[p] += k;
    }

    void push_down(int p, int l, int r)
    {
        if (tag[p])
        {
            int mid = l + r >> 1;
            change(ls(p), l, mid, tag[p]);
            change(rs(p), mid + 1, r, tag[p]);
            tag[p] = 0;
        }
    }

    void update(int ul, int ur, int p, int l, int r, T k)
    {
        if (ul <= l && r <= ur)
        {
            change(p, l, r, k);
            return;
        }
        push_down(p, l, r);
        int mid = l + r >> 1;
        if (ul <= mid)
            update(ul, ur, ls(p), l, mid, k);
        if (ur > mid)
            update(ul, ur, rs(p), mid + 1, r, k);
        push_up(p);
    }

    T query(int ql, int qr, int p, int l, int r)
    {
        if (ql <= l && r <= qr)
            return tree[p];
        push_down(p, l, r);
        T ans = 0;
        int mid = l + r >> 1;
        if (ql <= mid)
            ans += query(ql, qr, ls(p), l, mid);
        if (qr > mid)
            ans += query(ql, qr, rs(p), mid + 1, r);
        return ans;
    }

    void build()
    {
        build(1, 1, n);
    }

    void update(int ul, int ur, T k)
    {
        update(ul, ur, 1, 1, n, k);
    }

    T query(int ql, int qr)
    {
        return query(ql, qr, 1, 1, n);
    }
};

template <class T> struct HLD
{
    int n, dn;
    vector<T> in;
    vector<vector<int>> tree;
    vector<int> sz, dep, son, parent;
    vector<int> top, dfn;
    vector<T> rk;
    SegmentTree<T> segtree;

    HLD(int n)
        : n(n), dn(0), in(n + 1), tree(n + 1), sz(n + 1), dep(n + 1), son(n + 1), parent(n + 1), top(n + 1), dfn(n + 1),
          segtree(n), rk(n + 1)
    {
    }

    void add(int u, int v)
    {
        tree[u].push_back(v);
        tree[v].push_back(u);
    }

    void dfs1(int r)
    {
        sz[r] = 1;
        dep[r] = dep[parent[r]] + 1;
        for (auto &s : tree[r])
            if (s != parent[r])
            {
                parent[s] = r;
                dfs1(s);
                sz[r] += sz[s];
                if (sz[s] > sz[son[r]])
                    son[r] = s;
            }
    }

    void dfs2(int r, int up)
    {
        rk[dfn[r] = ++dn] = in[r];
        top[r] = up;
        if (son[r])
            dfs2(son[r], up);
        for (auto &s : tree[r])
            if (s != parent[r] && s != son[r])
                dfs2(s, s);
    }

    void update(int l, int r, T k)
    {
        while (top[l] != top[r])
        {
            if (dep[top[l]] < dep[top[r]])
                swap(l, r);
            segtree.update(dfn[top[l]], dfn[l], k);
            l = parent[top[l]];
        }
        if (dep[l] > dep[r])
            swap(l, r);
        segtree.update(dfn[l], dfn[r], k);
    }

    void update(int root, T k)
    {
        segtree.update(dfn[root], dfn[root] + sz[root] - 1, k);
    }

    T query(int l, int r)
    {
        T ans = 0;
        while (top[l] != top[r])
        {
            if (dep[top[l]] < dep[top[r]])
                swap(l, r);
            ans += segtree.query(dfn[top[l]], dfn[l]);
            l = parent[top[l]];
        }
        if (dep[l] > dep[r])
            swap(l, r);
        return ans + segtree.query(dfn[l], dfn[r]);
    }

    T query(int root)
    {
        return segtree.query(dfn[root], dfn[root] + sz[root] - 1);
    }

    void build(vector<T> &arr, int root = 1)
    {
        in = arr;
        dfs1(root);
        dfs2(root, root);
        segtree.in = rk;
        segtree.build();
    }

    void build(int root = 1)
    {
        dfs1(root);
        dfs2(root, root);
        segtree.in = rk;
        segtree.build();
    }
};
