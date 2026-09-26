//parallel_tasks.cpp
#include <locale.h>
#include <ppl.h>
#include <ppltasks.h>
#include <iostream>
using namespace std;
using namespace concurrency;

int main(void){
	setlocale( LC_ALL, ".ACP" );
	task<void> ts1([](){
		cout << "Старт первой задачи" << endl;
		wait(10000); 
		cout << "Завершение первой задачи" << endl; });
	task<void> ts2([](){
		cout << "Старт второй задачи" << endl;
		wait(10000);
		cout << "Завершение второй задачи" << endl; });
		ts1.wait(); ts2.wait();
	task<void> ts3([](){
			cout << "Старт третьей задачи" << endl;
			wait(10000);
			cout << "Завершение третьей задачи" << endl; });
	auto ts4 = ts3.then([](){
			cout << "Старт четвертой задачи" << endl;
			wait(10000);
			cout << "Завершение четвертой задачи" << endl; });
	ts4.wait();
}

