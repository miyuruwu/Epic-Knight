#include<iostream>
using namespace std;

//andiawhoa
long long n;

signed main(){
    cin >> n;
    long long sum = 0;
    for(int i = 1; i <= n; i++) {
        sum += i*i;
    }
    cout << sum;
}