#include <iostream>
#include <random>
#include <chrono>

using namespace std;

mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

int main() {
    long long n = uniform_int_distribution<long long>(0,1000000)(rng);
    int m = uniform_int_distribution<long long>(1,n)(rng);
    long long s0 = uniform_int_distribution<long long>(-1000000,1000000)(rng);

    cout << n << ' ' << m << ' ' << s0 << '\n';
}