//parallel_transform.cpp
#include <locale.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <ppl.h>
#include <iostream>
#include <cmath>
#define N1 1000000
#define N2 50

class My_Class{//Функтор
private:
	double a, b;
public:
My_Class(double a_ = 0, double b_ = 0){ a = a_; b = b_;}
double operator()(double x, double y) const
{double Tmp = 0;
for (int k = 1; k <= N2; k++)
    for (int j = 1; j <= N2; j++)
        Tmp += cos(k*x)*sin(j*y) / ((k*k + j*j)*sqrt(a*a+b*b+k+j));
return Tmp;
}
};

int main(int argc, char *argv[])
{setlocale(LC_ALL, ".ACP");
 std::vector<double> X0(N1);
 for (int k = 0; k < N1; k++)
	 X0[k] = k + 0.7;
 std::vector<double> Y0(N1);
 for (int k = 0; k < N1; k++)
	 Y0[k] =N1- k + 0.3;
 std::vector<double> Y(Y0);//Конструируем копию

 double Time = clock();
 std::vector<double>::iterator It=std::transform(X0.begin(), X0.end(), Y.begin(), Y.begin(), My_Class(1,2));
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 std::cout << "Преобразование завершено" << std::endl
	 << "Время посл. обр: " << Time << " сек." << std::endl;
 
 Y = Y0;
 Time = clock();
 It = concurrency::parallel_transform(X0.begin(), X0.end(), Y.begin(), Y.begin(), My_Class(1, 2));
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 std::cout << "Преобразование завершено" << std::endl
	 << "Время парал. обр.: " << Time << " сек." << std::endl;
}