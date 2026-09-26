#include <windows.h> // parallel_invoke.cpp 
#include <algorithm> // ѕараллельный алгоритм
#include <iostream> // рекурсивной битонической сортировки
#include <random>
#include <ppl.h>
#include <locale.h>

using namespace concurrency;
using namespace std;

 
template <class Function> // ¬ызов выполн€ющей вычислительную работу функции 
__int64 time_call(Function&& f) //и возврат числа миллисекунд,
{__int64 begin = GetTickCount(); f();
	return GetTickCount() - begin;}// которые затрачены на вычислительную работу.

const bool INCREASING = true;
const bool DECREASING = false;


template <class T> // ‘ункци€ сравнени€ дл€ алгоритма битонической сортировки 
void compare(T* items, int i, int j, bool dir)
{if (dir == (items[i] > items[j]))
	{swap(items[i], items[j]);} }


template <class T>// —ортировка битонической последовательности в заданном пор€дке. 
void bitonic_merge(T* items, int lo, int n, bool dir)
{if (n > 1)
	{int m = n / 2;
	 for (int i = lo; i < lo + m; ++i)
		{compare(items, i, i + m, dir);	}
		bitonic_merge(items, lo, m, dir);
		bitonic_merge(items, lo + m, m, dir);} }


template <class T>//—ортировка заданной последовательности в указанном пор€дке 
void bitonic_sort(T* items, int lo, int n, bool dir)
{if (n > 1)
	{int m = n / 2;// –аздел€ем массив на две части
	 bitonic_sort(items, lo, m, INCREASING);// и сортируем их в разном пор€дке
	 bitonic_sort(items, lo + m, m, DECREASING);
	 bitonic_merge(items, lo, n, dir);}}//—ортируем битоническую последовательность
	



template <class T>// —ортирует заданную последовательность по возрастанию 
void bitonic_sort(T* items, int size)
{bitonic_sort(items, 0, size, INCREASING);}

 
template <class T>// ѕараллельно сортирует битоническую последовательность в указанном пор€дке
void parallel_bitonic_merge(T* items, int lo, int n, bool dir)
{if (n > 500)// ѕараллельно сортируем, если достаточно работы
	{int m = n / 2;
	 for (int i = lo; i < lo + m; ++i)
		{compare(items, i, i + m, dir);}
	 parallel_invoke(//»спользуем parallel_invoke дл€ параллельной обработки битонич. послед.
	        [&items, lo, m, dir] { parallel_bitonic_merge(items, lo, m, dir); },
			[&items, lo, m, dir] { parallel_bitonic_merge(items, lo + m, m, dir); });}
	// ¬ противном случае, последовательно выполн€ем вычисл. работу 
	else if (n > 1)
	{bitonic_merge(items, lo, n, dir);}}

template <class T>// ѕараллельно сортирует заданную последовательность в указанном пор€дке
void parallel_bitonic_sort(T* items, int lo, int n, bool dir)
{if (n > 1) // –аздел€ем массив на две части
	{int m = n / 2;// и сортируем их в разном пор€дке
     parallel_invoke(//—ортируем части массива параллельно
			[&items, lo, m] { parallel_bitonic_sort(items, lo, m, INCREASING); },
			[&items, lo, m] { parallel_bitonic_sort(items, lo + m, m, DECREASING); });
		// ѕараллельно сортируем битоническую последовательность
		parallel_bitonic_merge(items, lo, n, dir);} }


template <class T> //—ортирует заданную последовательность по возрастанию
void parallel_bitonic_sort(T* items, int size)
{parallel_bitonic_sort(items, 0, size, INCREASING);}

int wmain()
{setlocale(LC_ALL, ".ACP");
 const int size = 0x2000000;// size должно быть степенью числа 2. 
 int* a1 = new int[size];// —оздаем два больших массива and fill them with random values.
 int* a2 = new int[size];// и далее заполн€ем их случайными числами
 mt19937 gen(42);
 for (int i = 0; i < size; ++i)
	{a1[i] = a2[i] = gen();}
__int64 elapsed;
// ѕоследовательный алгоритм битонической сортировки
 elapsed = time_call([&] { bitonic_sort(a1, size); });
 wcout << L"врем€ послед. алгоритма в миллисекундах: " << elapsed << endl;
// ѕараллельный алгоритм битонической сортировки
 elapsed = time_call([&] { parallel_bitonic_sort(a2, size); });
 wcout << L"врем€ парал. алгоритма в миллисекундах: " << elapsed << endl;
 delete[] a1;
 delete[] a2;
}

