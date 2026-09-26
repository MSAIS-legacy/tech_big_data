#include <windows.h> // parallel_invoke.cpp 
#include <algorithm> // ѕараллельный алгоритм
#include <iostream> // рекурсивной битонической сортировки
#include <random>
#include <ppl.h>
#include <locale.h>
using namespace concurrency;
using namespace std;
const bool INCREASING = true;
const bool DECREASING = false;

template <class Function>//¬ызов выполн€ющей вычисл. работу ф-ии
__int64 time_call(Function&& f) //и возврат числа миллисекунд,
{
	__int64 begin = GetTickCount(); f();
	return GetTickCount() - begin;
}//затраченных на вычислени€

template <class T> // ‘ункци€ сравнени€ дл€ алгоритма 
void compare(T* items, int i, int j, bool dir) // битонической
{
	if (dir == (items[i] > items[j])) // сортировки
	{
		swap(items[i], items[j]);
	}
}
template <class T>// —ортировка битонической последовательности 
void bitonic_merge(T* items, int lo, int n, bool dir)
{
	if (n > 1) // в заданном пор€дке.
	{
		int m = n / 2;
		for (int i = lo; i < lo + m; ++i)
		{
			compare(items, i, i + m, dir);
		}
		bitonic_merge(items, lo, m, dir);
		bitonic_merge(items, lo + m, m, dir);
	}
}

template <class T>//—ортировка заданной последовательности 
void bitonic_sort(T* items, int lo, int n, bool dir)
{
	if (n > 1) // в указанном пор€дке
	{
		int m = n / 2;// –аздел€ем массив на две части
		// и сортируем их в разном пор€дке
		bitonic_sort(items, lo, m, INCREASING);
		bitonic_sort(items, lo + m, m, DECREASING);
		//—ортируем битоническую последовательность
		bitonic_merge(items, lo, n, dir);
	}
}

template <class T>// —ортирует заданную последовательность 
void bitonic_sort(T* items, int size) // по возрастанию
{
	bitonic_sort(items, 0, size, INCREASING);
}
// ѕараллельно сортирует битонич. послед. в указанном пор€дке
template <class T>
void parallel_bitonic_merge(T* items, int lo, int n, bool dir)
{
	if (n > 500)// ѕараллельно сортируем, если достаточно работы
	{
		int m = n / 2;
		for (int i = lo; i < lo + m; ++i)
		{
			compare(items, i, i + m, dir);
		}
		parallel_invoke(//ѕараллельной обработка битонич. послед.
			[&items, lo, m, dir] {parallel_bitonic_merge(items, lo, m, dir); },
			[&items, lo, m, dir] {parallel_bitonic_merge(items, lo + m,
				m, dir); });
	}
	// ¬ противном случае, последовательно выполн€ем вычисл. работу 
	else if (n > 1)
	{
		bitonic_merge(items, lo, n, dir);
	}
}

// ѕараллельно сортирует заданную послед. в указанном пор€дке
template <class T>
void parallel_bitonic_sort(T* items, int lo, int n, bool dir)
{
	if (n > 1) // –аздел€ем массив на две части
	{
		int m = n / 2;// и сортируем их в разном пор€дке
		parallel_invoke(//—ортируем части массива параллельно
			[&items, lo, m] { parallel_bitonic_sort(items, lo, m,
				INCREASING); },
			[&items, lo, m] { parallel_bitonic_sort(items, lo + m, m,
				DECREASING); });
		// ѕараллельно сортируем битоническую последовательность
		parallel_bitonic_merge(items, lo, n, dir);
	}
}

//—ортирует заданную последовательность по возрастанию
template <class T>
void parallel_bitonic_sort(T* items, int size)
{
	parallel_bitonic_sort(items, 0, size, INCREASING);
}

int wmain()
{
	setlocale(LC_ALL, ".ACP");
	const int size = 0x2000000;//size должно быть степенью числа 2. 
	int* a1 = new int[size];// —оздаем два больших массива 
	int* a2 = new int[size];//и далее заполн€ем их случайн. числами
	mt19937 gen(42);
	for (int i = 0; i < size; ++i)
	{
		a1[i] = a2[i] = gen();
	}
	__int64 elapsed;
	// ѕоследовательный алгоритм битонической сортировки
	elapsed = time_call([&] { bitonic_sort(a1, size); });
	wcout << L"врем€ послед. алгоритма в миллисекундах: " <<
		elapsed << endl;
	// ѕараллельный алгоритм битонической сортировки
	elapsed = time_call([&] { parallel_bitonic_sort(a2, size); });
	wcout << L"врем€ парал. алгоритма в миллисекундах: " << elapsed
		<< endl;
	delete[] a1;
	delete[] a2;
}
