//My_Task_A.cpp
#include "My_Task.h"

double My_Task(int k)
{
 int KK=((k+11) % 11) + ((k+3) % 4);
 double S=0;
 for (int j1=1; j1<=1000*KK; j1++){
	 double Tmp1=3.14*j1;
	 for (int j2=1; j2<=1000*KK; j2++){
		  double Tmp2=2.78*j2;
	 S+=1.0/(Tmp1*Tmp1*Tmp1+Tmp2*Tmp2*Tmp2);
	 }
 }
  return S;
}

