#pragma once //Concurrent_Simps.h
typedef double (*Double_Func_Double)(double);
double Simps(double a, double b, int N, Double_Func_Double);
double Concurrent_Simps(double a, double b, int N, Double_Func_Double);

namespace MethodCall{//без __declspec(thread),
	static void *ObjAddr = nullptr;//т.к. Concurrent_Simps уже распараллелен

	template <class Ty>
	double Sub_Int_Func(double x)
	{return (*((Ty*)ObjAddr))(x);}

	template <class Ty>
	double Simpson(double a, double b, int N, Ty const &Obj)
	{ObjAddr = (void *)&Obj;
	 return Simps(a, b, N, Sub_Int_Func<Ty>);}

	template <class Ty>
	double Concurrent_Simpson(double a, double b, int N, Ty const &Obj)
	{ObjAddr = (void *)&Obj;
	 return Concurrent_Simps(a, b, N, Sub_Int_Func<Ty>);}
};