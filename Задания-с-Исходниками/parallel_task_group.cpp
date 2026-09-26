//parallel_task_group.cpp
#include <locale.h>
#include <time.h>
#include <ppl.h>
#include <iostream>
#include "My_Task.h"
#define NNN 100
using namespace std;
int main(int argc, char *argv[])
{setlocale( LC_ALL, ".ACP" );
 double V[NNN];
 double Time=clock();
 for (int k=0; k<NNN; k++) V[k]=My_Task(k);
 Time=(clock()-Time)/CLOCKS_PER_SEC;
 cout << "Подзадачи завершены. Время: " << Time << " сек." << endl;
 Concurrency::task_group TGr;
 Time=clock();
 for (int k=0; k<NNN; k++) TGr.run([&V,k](){V[k]=My_Task(k);});
 TGr.wait();
 Time=(clock()-Time)/CLOCKS_PER_SEC;
 cout << "Подзадачи завершены. Время: " << Time << " сек." << endl;
}