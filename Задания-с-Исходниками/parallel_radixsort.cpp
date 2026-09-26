//parallel_radixsort.cpp
#include <locale.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <ppl.h>
#include <iostream>
#include <cmath>
#define NNN 100000000
using namespace std;
int main(int argc, char *argv[])
{
	setlocale(LC_ALL, ".ACP");
	vector<size_t> X0(NNN);
	for (int k = 0; k < NNN; k++)
		X0[k] = size_t(4.0e+9*abs(sin(k+0.7)));
	vector<size_t> X(X0);//Конструируем копию

	double Time = clock();
	sort(X.begin(), X.end());
	Time = (clock() - Time) / CLOCKS_PER_SEC;
	cout << "Сортировка завершена" << endl
		<< "Время посл. сортировки: " << Time << " сек." << endl;
	X = X0;
	Time = clock();
	concurrency::parallel_sort(X.begin(), X.end());
		Time = (clock() - Time) / CLOCKS_PER_SEC;
		cout << "Сортировка завершена" << endl
			<< "Время parallel_sort: " << Time << " сек." << endl;
	X = X0;
	Time = clock();
	concurrency::parallel_buffered_sort(X.begin(), X.end());
	Time = (clock() - Time) / CLOCKS_PER_SEC;
	cout << "Сортировка завершена" << endl
	    << "Время parallel_buffered_sort: " << Time << " сек." << endl;
	X = X0;
	Time = clock();
	concurrency::parallel_radixsort(X.begin(), X.end());
	Time = (clock() - Time) / CLOCKS_PER_SEC;
	cout << "Сортировка завершена" << endl
		<< "Время parallel_radix_sort: " << Time << " сек." << endl;

}