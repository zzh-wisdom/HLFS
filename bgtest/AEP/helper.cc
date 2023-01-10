#include <iostream>

using namespace std;

int main() {
    double bandwidth;
    int bs;
    double result;
    while(1) {
        cout << "请输入带宽(MB/s)和访问大小(B):";
        cin >> bandwidth >> bs;
        result = 1000.0*1000 / (bandwidth * 1024 * 1024 / bs) * 1000;
        printf("lat: %0.2lf ns\n", result);
    }
    return 0;
}