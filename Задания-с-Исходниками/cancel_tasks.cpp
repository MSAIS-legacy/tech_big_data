// cancel_tasks.cpp 
// compile with: /EHsc
#include <ppl.h>
#include <ppltasks.h>
#include <iostream>
#include <sstream>
#include <locale.h>

using namespace concurrency;
using namespace std;

bool do_work()
{// Имитация работы
 cout << "Работаем..." << endl;
 wait(1000);
 return true;
}

int main()
{
	setlocale(LC_ALL, ".ACP");
	cancellation_token_source cts;
	auto token = cts.get_token();
	cout << "Создание задачи..." << endl;
	// Создание задачи, которая выполняет работу, пока не будет прекращена
	auto t = create_task([token]
	{
		bool moreToDo = true;
		while (moreToDo)
		{
			// Проверка необходимости прекращения 
			if (token.is_canceled())
			{	// TODO: При необходимости выполнить очистку... 

				// Завершить текущую задачу
				cancel_current_task();
			}
			else
			{
				// Выполнять работу
				moreToDo = do_work();
			}
		}
	}, token);

	// Ждать 10 секунд и потом прекратить работу.
	wait(10000);

	cout << "Прекратить задачу..." << endl;
	cts.cancel();

	// Ожидание прекращения.
	cout << "Ожидание прекращения завершено..." << endl;
	t.wait();

	cout << "Готово." << endl;
}
