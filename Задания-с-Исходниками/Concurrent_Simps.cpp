
#include "Concurrent_Simps.h"//Concurrent_Simps.cpp
#include <ppl.h>
using namespace concurrency;

double Simps(double a, double b, int N, Double_Func_Double Func)
{double h=(b-a)/(2*N);
 double S1=0, S2=0;
 for (int k=1; k<N; k++){       
	  double Tmp=a+(2*k-1)*h; S1+=Func(Tmp);  S2+=Func(Tmp+h);}
 S1+=Func(b-h);
 return h*(Func(a) + Func(b) + 4.0*S1 + 2.0*S2)/3.0;}

double Concurrent_Simps(double a, double b, int N, Double_Func_Double Func)
{double h = (b - a) / (2 * N);
 combinable<double> CS1([](){return 0.0;}), CS2([](){return 0.0;});
 parallel_for(1, N, [a, h, Func, &CS1, &CS2](int k)
   {double Tmp = a + (2 * k - 1)*h;
		 CS1.local() += Func(Tmp); CS2.local() += Func(Tmp + h);});
	double S1 = CS1.combine([](double x, double y){return x + y; });
	double S2 = CS2.combine([](double x, double y){return x + y; });
	S1 += Func(b - h);
	return h*(Func(a) + Func(b)	+ 4.0*S1 + 2.0*S2) / 3.0;}
