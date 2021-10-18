// Damian Strojek s184407 Informatyka semestr 3 WETI
#include <iostream>
#include <math.h>
#include <iomanip>

void normalFibonacci(const int n);
void formulaFibonacci(const int n);

int main(){
    std::cout << "Which fibonacci number you would like to get? ";
    int number = 0;
    std::cin >> number;
    if(number > 0){
        normalFibonacci(number);
        formulaFibonacci(number);
    }
    else{
        std::cout << "Bad number\n";
    }
    return 0;
};

// Normalna funkcja obliczająca dany numer ciągu fibonacciego
void normalFibonacci(const int n){
    long long c = 0;
    if(n == 1){
        c = 0;
    }
    else if(n == 2){
        c = 1;
    }
    else{
        long long a = 0, b = 1, i = 3;
        c = a + b;
        while(i <= n){
            a = b;
            b = c;
            c = a + b;
            i++;
        }
    }
    std::cout << "Normal fibonacci number : " << c << "\n";
};

// Funkcja korzystająca z uproszczonego wzoru na wyraz ciągu fibonacciego
void formulaFibonacci(const int n){
    long double a, c;
    if(n == 1){
        c = 0;
    }
    else if(n == 2){
        c = 1;
    }
    else{
        a = (1+sqrt(5))/2;
        c = (1/sqrt(5)) * pow(a, n);
    }
    std::cout << std::fixed << std::setprecision(17);
    std::cout << "Formula fibonacci number not rounded : " << c << "\n";
    long long b = std::round(c);
    std::cout << "Formula fibonacci number rounded : " << b << "\n";
};
