//parallel_for.cpp
#include <locale.h>
#include <time.h>
#include <ppl.h>
#include <iostream>
#include "My_Task.h"
#define NNN 500

int main(int argc, char *argv[])
{setlocale( LC_ALL, ".ACP" );
 double V[NNN];
 double Time = clock();
 for (int k=0; k<NNN; k++){
     V[k]=My_Task(k);}
 Time = (clock() - Time) / CLOCKS_PER_SEC;
 std::cout<<"Подзадачи завершены"<<std::endl
          << "Время посл. обр.: " << Time << " сек." << std::endl;
 Time=clock();
 Concurrency::parallel_for(0, NNN, [&V](size_t k){V[k]=My_Task(k);});
 Time=(clock()-Time)/CLOCKS_PER_SEC;
 std::cout << "Подзадачи завершены" << std::endl
           << "Время парал. обр.: " << Time << " сек." << std::endl;
 }