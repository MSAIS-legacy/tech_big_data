// task_aggregation.cpp 
#include <ppl.h>
#include <ppltasks.h>
#include <array>
#include <iostream>
#include <locale.h>

using namespace concurrency;
using namespace std;

int main()
{
	setlocale(LC_ALL, ".ACP");
	// Запуск нескольких задач 
	array<task<void>, 3> tasks =
	{
		create_task([] { cout << "Задача 1." << endl; wait(5000); }),
		create_task([] { cout << "Задача 2." << endl; wait(3000); }),
		create_task([] { cout << "Задача 3." << endl; wait(2000); })
	};

	auto joinTask = when_all(begin(tasks), end(tasks));

	// Распечатка сообщения из потока, объединившего задачи
	cout << "Сообщение из потока, объединившего задачи" << endl;

	// Ожидание завершения задач
	joinTask.wait();
	cout << "Задачи завершены" << endl;
}
