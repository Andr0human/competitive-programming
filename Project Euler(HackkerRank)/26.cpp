/***
 * Project Euler #26: Reciprocal cycles
 * Approach : Precompute cycle for every number.
 * Numbers whose prime factors are only 2 and 5 have no cycles.
 * Each query can be handled in O(N) Time.
***/


#include <iostream>
#include <cstring>
#include <vector>
#include <set>
// #include "lib/perf.h"
using namespace std;

#define yes "Yes"
#define no "No"
#define ll long long
#define ull unsigned long long
const int mod = (int)1e9 + 7;
const int maxN = (int)1e4;

void
print(vector<int>& arr)
{
    for (int i : arr)
        cout << i << " ";
    cout << endl;
}

int
simplify(int N)
{
    while (N % 2 == 0) N /= 2;
    while (N % 5 == 0) N /= 5;
    return N;
}

vector<int>
generator(int N, int& num)
{
    vector<int> res;

    // Store the remainder after every division
    bool occured[N] = {0};
    while (true)
    {
        while (num < N)
        {
            num *= 10;
            res.emplace_back(0);
        }
        int div = num / N;
        if (res.size()) res.pop_back();
        num %= N;
        
        // Check if remainder already occured, if yes cycle found!
        if (occured[num]) break;
        occured[num] = true;

        // Can be removed for speed, but is usefull
        // if we want to see the cycle and still fast enough
        res.emplace_back(div);
    }

    return res;
}

int
generate_cycle(int N)
{
    int num = 1;
    while (num < N)
        num *= 10;

    // Cycle can be wrong because of digits before the cycle
    const auto cycle1 = generator(N, num);

    // Correct cycle as it starts with no leading digits
    const auto cycle2 = generator(N, num);

    // print(arr1);
    // print(arr2);
    // Use print function to see both cycles and their diff on 48

    return cycle2.size();
}

vector<int>
preCompute()
{
    // Generating cycle for every number upto maxN(10'000)

    vector<int> arr(maxN + 1, 0);
    for (int i = 3; i <= maxN; i++)
    {
        int x = simplify(i);
        if (x > 1) arr[i] = generate_cycle(x);
    }

    return arr;
}

int
mySolution(int N, vector<int> &arr)
{
    // O(1) solution for query exists, but O(N) also does the job!

    int best = 3;
    for (int i = 3; i < N; i++)
    {
        if (arr[i] > arr[best])
            best = i;
    }
    return best;
}

int main()
{
    ios::sync_with_stdio(0); cin.tie(0);

    // perf::Time(preCompute);
    vector<int> cycle_list = preCompute();
    int testcases; cin >> testcases;

    while(testcases--)
    {
        int N; cin >> N;
        cout << mySolution(N, cycle_list) << endl;
    }
}
