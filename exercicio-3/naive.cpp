#include <iostream>
#include <cctype>
#include <cmath>

using namespace std;

int sanitizeInput(int argc, char** argv);
int isnumber(char* c);
int ehPrimo(long long int n);

int main(int argc, char* argv[]) {
    int status = 0;
    if(status = sanitizeInput(argc, argv)) {
        return status;
    }

    long long n = stoll(argv[1]);
    int m = stoi(argv[2]);
    long long s0 = stoll(argv[3]);
    long long ans = 0;

    for(int i = 0; i < n; i++) {
        ans += ehPrimo(s0+i);    
    }

    cout << ans << '\n';
}

int sanitizeInput(int argc, char** argv) {
    if(argc != 4) {
        cout << "Uso: <n> <m> <inteiro inicial>\n";
        return 1;
    }

    long long n = atoll(argv[1]);
    int m = atoi(argv[2]);
    int first = atoll(argv[3]);
    
    if(!isnumber(argv[1]) || n < 0) {
        cout << "ERRO! n deve ser um inteiro maior ou igual a 0.\n";
        return 1;
    }
    
    if(!isnumber(argv[2]) || m <= 0) {
        cout << "ERRO! m deve ser um inteiro maior que 0.\n";
        return 1;
    }
    
    if(!isnumber(argv[3])) {
        cout << "ERRO! O número inicial da sequência deve ser um inteiro\n";
        return 1;
    }

    return 0;
}

int isnumber(char* c) {
    int i = 0;
    if(c[i] == '-') c++;
    while(c[i] != '\0') {
        if(!isdigit(c[i])) return 0;
        i++;
    }
    return i > 0;
}

int ehPrimo(long long int n) {
    int i;
    if (n<=1) return 0;
    if (n==2) return 1;
    if (n%2==0) return 0;
    for (i=3; i<sqrt(n)+1; i+=2)
        if(n%i==0) return 0;
    return 1;
}