//OMP_task.cpp
//Компилировать: icl -DNDEBUG -MD -O3 -Qopenmp -EHsc OMP_task.cpp
#include <locale.h>
#include <time.h>
#include <ppl.h>
#include <iostream>


#include "My_Task.h"

#define NNN 100

int main(int argc, char *argv[])
{setlocale( LC_ALL, ".ACP" );
double V[NNN];
double Time=clock();
for (int k=0; k<NNN; k++){
	 V[k]=My_Task(k);
	 }
 std::cout<<"Подзадачи завершены"<<std::endl;
 Time=(clock()-Time)/CLOCKS_PER_SEC;
 std::cout<<"Время: "<<Time<<std::endl;
 
 Time=clock();
#pragma omp parallel
	 {
#pragma omp single		
		 {
			 for (int k=0; k<NNN; k++){
#pragma omp task
				 V[k]=My_Task(k);
			 }
#pragma omp taskwait
		 }
	 }
 std::cout<<"Подзадачи завершены"<<std::endl;
 Time=(clock()-Time)/CLOCKS_PER_SEC;
 std::cout<<"Время: "<<Time<<std::endl;
}