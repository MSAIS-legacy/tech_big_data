//task_tree.cpp
#include <locale.h>
#include <ppl.h>
#include <iostream>
using namespace std;
using namespace concurrency;

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, ".ACP");
	//Создать группу задач, являющуюся корнем дерева
 task_group tg1;
 //Создать задачу, которая содержит вложенную группу задач
auto t1=make_task([&](void)->void{
	    cout << "Задача 1" << endl;
    	task_group tg2;
		//Создать дочернюю задачу
		auto t4=make_task([&](void)->void{
			// Выполнить работу
			wait(1000);
			cout << "Задача 4" << endl; 
		});

		// Создать дочернюю задачу
		auto t5=make_task([&](void)->void{
			// Выполнить работу
			wait(2000);
			cout << "Задача 5" << endl;
		});

		// Выполнить дочерние задачи и дождаться их завершения
		tg2.run(t4);
		tg2.run(t5);
		tg2.wait();
	});

	//Создать дочернюю задачу
 auto t2=make_task([&](void)->void{
		// Выполнить работу
		 wait(3000);
		 cout << "Задача 2" << endl;
	});

	// Создать дочернюю задачу
auto t3=make_task([&](void)->void{
		// Выполнить работу
		wait(1000);
		cout << "Задача 3" << endl;
	});

	// Выполнить дочерние задачи и дождаться их завершения
	tg1.run(t1);
	tg1.run(t2);
	tg1.run(t3);
	tg1.wait();

}
