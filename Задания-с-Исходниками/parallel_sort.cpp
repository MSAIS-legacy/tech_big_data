//parallel_sort.cpp
#include <locale.h>
#include <time.h>
#include <vector>
#include <complex>
#include <algorithm>
#include <ppl.h>
#include <iostream>
#define NNN 50000000
using namespace std;
int main(int argc, char *argv[])
{setlocale(LC_ALL, ".ACP");
 vector<complex<double>> Z0(NNN);
 for (int k = 0; k < NNN; k++)
	 Z0[k] =exp(complex<double>(0, k + 0.7));
 vector<complex<double>> Z(Z0);//Конструируем копию

 double Time = clock();
 sort(Z.begin(), Z.end(), 
	 [](const complex<double>& left, const complex<double>& right) {
	 return left.real() < right.real();});
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 cout << "Сортировка завершена" << endl
	 << "Время посл. сортировки: " << Time << " сек." << endl;
 Z = Z0;
 Time = clock();
 concurrency::parallel_sort(Z.begin(), Z.end(),
	 [](const complex<double>& left, const complex<double>& right) {
	 return left.real() < right.real(); });
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 cout << "Сортировка завершена" << endl
	 << "Время парал. сортировки: " << Time << " сек." << endl;
 Z = Z0;
 Time = clock();
 concurrency::parallel_buffered_sort(Z.begin(), Z.end(),
	 [](const complex<double>& left, const complex<double>& right) {
	 return left.real() < right.real(); });
Time = (clock() - Time) / CLOCKS_PER_SEC;
cout << "Сортировка завершена" << endl
	<< "Время парал. сортировки: " << Time << " сек." << endl;
}