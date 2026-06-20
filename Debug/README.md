# dbg.hpp — 竞赛调试头文件

## 快速入手

```cpp
#ifdef IHXNAN
    #include <dbg>
#else
    #define gdb(...)              ((void)0)
    #define gdbif(c, ...)         ((void)0)
    #define gdb_assert(cond, msg) ((void)0)
    #define TIME(name)            ((void)0)
    #define gc() (p1==p2&&(p2=(p1=buf)+fread(buf,1,S,stdin),p1==p2)?EOF:*p1++)
#endif
```

---

## 宏一览

| 宏 | 作用 |
|---|---|
| `gdb(x, y, ...)` | 打印变量名和值，多变量逗号分隔一行 |
| `gdbif(cond, ...)` | 条件 gdb |
| `gdb_assert(cond, msg)` | 断言失败打印消息并 `exit(1)` |
| `TIME(name)` | RAII 计时器，离开作用域打印耗时 |

## 函数/类型一览（非宏，需 C++20 或 POSIX）

| 名称 | 作用 | 依赖 |
|---|---|---|
| `DbgStream` | 流式输出 `[file:line]`，适合表达式打印 | C++20 `source_location` |
| `dbg_binary(os, val)` | 整数二进制打印（四位分组加下划线） | 无 |
| `Bench` | 多轮计时器，析构输出 run count / avg / min / max | 无 |
| `dbg_register_signal_handlers()` | 注册 SIGSEGV/SIGABRT/SIGFPE 处理器，崩溃时打印 backtrace | POSIX (`execinfo.h`) |
| `dbg_set_color(bool)` | 运行时开关 ANSI 颜色 | 无 |

> 首次触发任何调试输出时，会先打印一个空行，防止与前置 `cout` 输出粘连。
> 非宏的功能**不受** `#ifdef IHXNAN` / `#else` 分支控制——需要在 `#ifdef IHXNAN` 块内 `#include <dbg>`，否则不会被编译。

---

## gdb

### 基础变量

```cpp
int n = 5, m = 10;
gdb(n, m);
// [main.cpp:42]
// n = 5, m = 10
```

### 顺序容器

```cpp
vector<int> v = {1, 2, 3};
gdb(v);          // [main.cpp:42]
// v = [1, 2, 3]

deque<int> dq = {7, 8, 9};
gdb(dq);         // [main.cpp:42]
// dq = [7, 8, 9]

list<int> lst = {100, 200, 300};
gdb(lst);        // [main.cpp:42]
// lst = [100, 200, 300]

array<int, 4> arr = {0, 1, 2, 3};
gdb(arr);        // [main.cpp:42]
// arr = [0, 1, 2, 3]
```

### 关联容器

```cpp
set<int> st = {3, 1, 4};
gdb(st);         // [main.cpp:42]
// st = {1, 3, 4}

map<string, int> mp = {{"alice", 90}, {"bob", 85}};
gdb(mp);         // [main.cpp:42]
// mp = {"alice": 90, "bob": 85}
```

### 容器适配器

> 自动拷贝后遍历，原容器不受影响。

```cpp
stack<int> stk; stk.push(1); stk.push(2); stk.push(3);
gdb(stk);        // [main.cpp:42]
// stk = [3, 2, 1]    (弹栈顺序)

queue<int> q; q.push(10); q.push(20); q.push(30);
gdb(q);          // [main.cpp:42]
// q = [10, 20, 30]   (FIFO)

priority_queue<int> pq; pq.push(5); pq.push(2); pq.push(8);
gdb(pq);         // [main.cpp:42]
// pq = [8, 5, 2]      (优先级降序)
```

### 结构类型

```cpp
pair<int, double> p = {42, 3.14};
gdb(p);          // [main.cpp:42]
// p = <42, 3.14>

tuple<int, char, string> t = {1, 'x', "hello"};
gdb(t);          // [main.cpp:42]
// t = (1, 'x', "hello")

optional<int> opt = 7, nopt = nullopt;
gdb(opt, nopt);  // [main.cpp:42]
// opt = 7, nopt = nullopt

variant<int, string> var = "world";
gdb(var);        // [main.cpp:42]
// var = "world"

bitset<8> bs(42);
gdb(bs);         // [main.cpp:42]
// bs = 00101010

__int128_t big = (__int128_t)1e18 * 1e9;
gdb(big);        // [main.cpp:42]
// big = 1000000000000000000000000000
```

### 嵌套容器

> `vector<vector<T>>` 自动以网格格式输出（行列号红色高亮），其余嵌套容器多行缩进。

```cpp
vector<vector<int>> dp = {{1, 0}, {0, 1}};
gdb(dp);
// dp = 
//        0     1
//    0       1     0
//    1       0     1

vector<set<int>> vec_set = {{1, 2}, {3}};
gdb(vec_set);
// vec_set = [
//   {1, 2},
//   {3}
// ]
```

---

## gdbif — 条件调试

```cpp
gdbif(i > 100, i, arr[i]);    // 仅当 i > 100 时打印
```

---

## gdb_assert — 调试断言

```cpp
gdb_assert(cnt > 0, "cnt must be positive but got " + to_string(cnt));
// 失败输出: [file:line] ASSERT FAILED [...msg...]  然后 exit(1)
```

---

## TIME — 计时器

```cpp
{ TIME("dp"); solve(); }    // 离开作用域打印耗时
// [Timer] dp : 0.001234 s

TIME("sort"); sort(v.begin(), v.end());  // 单语句也适用
```

---

## dbg_print — 直接调用

```cpp
dbg_print(cerr, 42);              // 42
dbg_print(cerr, vector{1,2});     // [1, 2]
dbg_print(cerr, x); cerr << '\n'; // 手动换行
```

---

## DbgStream — 流式临时调试

适合打印表达式（无变量名）：

```cpp
DbgStream() << "partial = " << (a + b) * c;
// [main.cpp:42]
// partial = 42
```

> 需要 `-std=c++20`（`source_location`）。

---

## dbg_binary — 整数二进制打印

```cpp
dbg_binary(cerr, 0b10110110u);
// 0b1011_0110
```

---

## Bench — 多轮计时统计

对比两种做法时，多轮跑出 min / avg / max：

```cpp
Bench b("quick_vs_merge");
for (int t = 0; t < 10; ++t) {
    auto data = gen_data(1e6);
    b.start(); quick_sort(data); b.stop();
    b.start(); merge_sort(data); b.stop();
}
// [Bench] quick_vs_merge : 20 runs, avg 0.0123 s, min 0.0101 s, max 0.0145 s
```

---

## dbg_backtrace — 崩溃定位

在 `main()` 开头调一次，段错误/断言/浮点异常时自动打印调用栈：

```cpp
int main() {
    dbg_register_signal_handlers();
    // ...
}
// [CRASH] Caught SIGSEGV
// 0x... (调用栈)
```

> 编译加 `-g -rdynamic` 可获得函数名。仅 Linux（POSIX）。

---

## dbg_set_color — 运行时颜色开关

```cpp
dbg_set_color(false);   // 关闭所有 ANSI 颜色，适合输出重定向到文件
```

---

## 支持的容器/类型

- `pair`, `tuple`, `optional`, `variant`
- `vector`, `deque`, `list`, `forward_list`, `array`
- `set`, `multiset`, `unordered_set`
- `map`, `multimap`, `unordered_map`
- `stack`, `queue`, `priority_queue` (自动拷贝后遍历)
- `bitset`
- `__int128` / `__uint128`
- 所有有 `operator<<` 的基础类型
- 嵌套容器 (元素为容器时自动多行缩进)
- 整数二进制打印 (`dbg_binary`)

## 需要额外运行时支持的功能

| 功能 | C++ 标准 | 平台 |
|---|---|---|
| `DbgStream` | C++20 (`source_location`) | 全平台 |
| `dbg_backtrace` | C++11 | Linux（POSIX） |
| 其余全部 | C++17 | 全平台 |
