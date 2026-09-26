//Vectors_vs_Lists.cpp
#include <locale.h>
#include <time.h>
#include <vector>
#include <deque>
#include <list>
#include <algorithm>
#include <ppl.h>
#include <iostream>

#define NNN 10000000

int main(int argc, char *argv[])
{setlocale( LC_ALL, ".ACP" );
 //Обработка векторов
 std::cout << "Обработка векторов" << std::endl;
 std::vector<double> V(NNN);
 for (int k = 0; k < NNN; k++)
	 V[k] = k + 0.7;
 std::vector<double> VP(V);//Конструируем копию
 double Time = clock();
 std::for_each(V.begin(), V.end(), [](double & x){x = 1/(1+x*x); });
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 std::cout << "Подзадачи завершены" << std::endl
	 << "Время посл. обр: " << 1000*Time << " мсек." << std::endl;
 Time = clock();
 concurrency::parallel_for_each(VP.begin(), VP.end(), [](double & x){x = 1 / (1 + x*x); });
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 std::cout << "Подзадачи завершены" << std::endl
	 << "Время парал. обр.: " << 1000 * Time << " мсек." << std::endl;
 //Обработка деков
 std::cout << "Обработка деков" << std::endl;
 std::deque<double> Dq;
 for (int k = 0; k < NNN; k++)
	 Dq.push_back(k + 0.7);
 std::deque<double> DqP(Dq);
 Time = clock();
 std::for_each(Dq.begin(), Dq.end(), [](double & x){x = 1 / (1 + x*x); });
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 std::cout << "Подзадачи завершены" << std::endl
	 << "Время посл. обр: " << 1000 * Time << " мсек." << std::endl;
 Time = clock();
 concurrency::parallel_for_each(DqP.begin(), DqP.end(), [](double & x){x = 1 / (1 + x*x); });
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 std::cout << "Подзадачи завершены" << std::endl
	 << "Время парал. обр.: " << 1000 * Time << " мсек." << std::endl;
 //Обработка списков
 std::cout << "Обработка списков" << std::endl;
 std::list<double> Lst;
 for (int k = 0; k < NNN; k++)
	 Lst.push_back(k + 0.7);
 std::list<double> LstP(Lst);
 Time = clock();
 std::for_each(Lst.begin(), Lst.end(), [](double & x){x = 1 / (1 + x*x); });
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 std::cout << "Подзадачи завершены" << std::endl
	 << "Время посл. обр: " << 1000 * Time << " мсек." << std::endl;
 Time = clock();
 concurrency::parallel_for_each(LstP.begin(), LstP.end(), [](double & x){x = 1 / (1 + x*x); });
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 std::cout << "Подзадачи завершены" << std::endl
	 << "Время парал. обр.: " << 1000 * Time << " мсек." << std::endl;
}