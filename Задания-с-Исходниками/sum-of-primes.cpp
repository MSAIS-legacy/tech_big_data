//sum-of-primes.cpp
#include <locale.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ppl.h>
#include <iostream>
#define NNN 200000
//Является ли число простым? 
bool is_prime(int n)
{if (n < 2)
	return false;
for (int i = 2; i < n; ++i)
	{if ((n % i) == 0)
	    return false;
	}
 return true;
}

int main(int argc, char *argv[])
{setlocale(LC_ALL, ".ACP");
 std::vector<int> X(NNN);
 for (int k = 0; k < NNN; k++)
	 X[k] = k;
 std::vector<int> Y(X); //Конструируем копию
 int prime_sum;//Сумма простых чисел
 //Последовательный алгоритм
 double Time = clock();
 std::transform(begin(X), end(X), begin(X),
	           [](int i) {return is_prime(i) ? i: 0;});
 prime_sum = std::accumulate(begin(X), end(X), 0);
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 std::cout << "Число простых чисел: "<< prime_sum << std::endl
	 << "Время посл. алгоритма: " << Time << " сек." << std::endl;
 Time = clock();
 concurrency::parallel_transform(begin(Y), end(Y), begin(Y),
	 [](int i) {return is_prime(i) ? i : 0; });
 prime_sum = concurrency::parallel_reduce(begin(Y), end(Y), 0);
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 std::cout << "Число простых чисел: " << prime_sum << std::endl
	 << "Время парал. алгоритма: " << Time << " сек." << std::endl;
}