//move_semantic.cpp - сравнить Debug и Release c предопределенным макросом MOVE_SEMANT и без него
#include <locale.h>
#include <iostream>
using namespace std;

class Vector{
private:
	int N;
	double *Ptr;
public:
	Vector()
	{N = 0; Ptr = nullptr; }
	
	Vector(int _N, double a=0)
	{N = _N; Ptr = new double[N];
	 for (int k = 0; k < N; k++)
		 Ptr[k] = a;
	}
	
	Vector(Vector const & V)
	{cout << "Конструктор копии" << endl;
	 Ptr = nullptr;
	 N = V.N;
	 if (N > 0){
		Ptr = new double[N];
		for (int k = 0; k < N; k++)
			Ptr[k] = V.Ptr[k];
	   }
	}

#ifdef MOVE_SEMANT
	Vector(Vector && V) 
	{cout << "Передвигающий конструктор" << endl;
	 N = V.N;
	 Ptr = V.Ptr;
	 V.Ptr = nullptr;
	 V.N = 0;
	}
#endif
	
	~Vector(){if (Ptr != nullptr) delete[] Ptr;  }
	
	const Vector & operator=(Vector const & V)
	{cout << "Оператор присваивания" << endl;
	if (this != &V){
		if (Ptr != nullptr) {
			delete[] Ptr;
			Ptr = nullptr;
		}
		N = V.N;
		if (N > 0){
			Ptr = new double[N];
			for (int k = 0; k < N; k++)
				Ptr[k] = V.Ptr[k];
		}
	}
	return *this;
	}
#ifdef MOVE_SEMANT	
	const Vector & operator=(Vector && V)
	{cout << "Передвигающий оператор присваивания" << endl;
	 if (this != &V){
		if (Ptr!= nullptr) delete [] Ptr;
		N = V.N;
		Ptr = V.Ptr;
		V.N = 0;
		V.Ptr = nullptr;
	}
	 return *this;
	}
#endif
	//Переопределение операций
		
	
	Vector operator*(double a) const
	{Vector V(N);
	 for (int k = 0; k < N; k++)
		  V.Ptr[k] = a*Ptr[k];
	 return V;
	}

	friend Vector operator*(double a, Vector const & V);
		
	Vector operator +(Vector const &V) const
	{if (N == V.N){
		Vector Tmp(N);
		for (int k = 0; k < N; k++)
			Tmp.Ptr[k] = Ptr[k] + V.Ptr[k];
		return Tmp;
		}
	else { throw "Несовпадение длин векторов"; }
	}
	
	Vector operator -(Vector const &V) const
	{if (N == V.N){
		Vector Tmp(N);
		for (int k = 0; k < N; k++)
			Tmp.Ptr[k] = Ptr[k] - V.Ptr[k];
		return Tmp;
		}
	else { throw "Несовпадение длин векторов"; }
	}

	double & operator[] (int k)
	{return Ptr[k];}

	double & operator[] (int k) const
	{return Ptr[k];}
};

Vector operator*(double a, Vector const & V)
{Vector Tmp(V.N);
 for (int k = 0; k < V.N; k++)
 Tmp.Ptr[k]= a*V.Ptr[k];
 return Tmp;
}


int main(int argc, char *argv[])
{
	setlocale(LC_ALL, ".ACP");
	try{
		Vector X(3,1);
		Vector Y(3,3);
		
		Vector  Z =X*3+2*Y;
		cout << "Z[1]=" << Z[1] << endl;
		Z = 2 * Z - 3 * X;
		cout << "Z[1]=" << Z[1] << endl;
		
	}
	catch (const char *p)
	{
		cout << "Исключение: " << p << endl;
	}
}