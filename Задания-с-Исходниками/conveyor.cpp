// conveyor.cpp 
// compile with: /EHsc
#include <agents.h>
#include <cmath>
#include <iostream>
#include <locale.h>
#include <time.h>

using namespace concurrency;
using namespace std;

int const N = 100; //Размерность рядов
int const NNN = 1000; //Длина массива на входе конвейера 
int const Chunk = 100; //Порция обрабатываемых элементов

double Func1(double x)//Первая ступень конвейера
{
	double Tmp = 0, Tmpa=abs(x);
	for (int n = 0; n <= N; n++){
		double Tmpxn = x + n, Tmpn3 = n*n*n;
		for (int k = 0; k <= N; k++){
			double Tmpk = k*k;
			for (int j = 0; j <= N; j++)
				Tmp += Tmpxn / (1 + Tmpa + Tmpn3 + k*k + j*j);
		}
	}
	return Tmp;
}

double Func2(double x)//Вторая ступень конвейера
{
	double Tmp = 0, Tmpa=abs(x);
	for (int n = 0; n <= N; n++){
		double Tmpn = n*n; 
		for (int k = 0; k <= N; k++){
			double Tmpk = k*k*k;
			for (int j = 0; j <= N; j++)
				Tmp += 1.0 / (1 + Tmpa + Tmpn + Tmpk + j*j);
		}
	}
	return x*Tmp;
}

double Func3(double x)//Третья ступень конвейера
{
	double Tmp = 0, Tmp2 = x*x;
	for (int n = 0; n <= N; n++){
		double Tmpn = n*n;
		for (int k = 0; k <= N; k++){
			double Tmpk = k*k;
			for (int j = 0; j <= N; j++)
				Tmp += (Tmp2 + j) / (1 + Tmp2 + Tmpn + Tmpk + j*j*j);
		}
	}
	return Tmp;
}

double Func4(double x)//Четвертая ступень конвейера
{
	double Tmp = 0, Tmpa=abs(x);
	for (int n = 0; n <= N; n++){
		double Tmpn = n*n*n;
		for (int k = 0; k <= N; k++){
			double Tmpk = Tmpa - k, 
				Tmpk4 = k*k; Tmpk4 *= Tmpk4;
			for (int j = 0; j <= N; j++)
				Tmp += Tmpk / (1 + Tmpa + Tmpn + Tmpk4 + j*j*j);
		}
	}
	return Tmp;
}

class Stage_Agent : public agent
{
private:
	// Буфер - источник информации
	ISource<double>& _source;
	// Буфер - приемник информации
	ITarget<double>& _target;
	//Функция для обработки
	double(*_func) (double);
public:
	Stage_Agent(ISource<double>& source, ITarget<double>& target, double func(double))
		: _target(target), _source(source)
	{
		_func = func;
	}
protected:
	void run()
	{
		while (true){
		       asend(_target, _func(receive(_source)));
		}

	}

};

int main()
{
	setlocale(LC_ALL, ".ACP");
	vector<double> X(NNN), Y(NNN), Z(NNN);
	//Заполнение входного вектора
	for (int k = 0; k < X.size(); k++)
		X[k] = 10 * sin((double)k);
	double Tms = clock();
	//Последовательные вычисления
	for (int k = 0; k < X.size(); k++){
		Y[k] = Func1(X[k]);
		Y[k] = Func2(Y[k]);
		Y[k] = Func3(Y[k]);
		Y[k] = Func4(Y[k]);
	}
	Tms = (clock() - Tms) / CLOCKS_PER_SEC;
	cout << "Время последовательного алгоритма: " << Tms << " c." << endl;
	unbounded_buffer<double> buf0, buf1, buf2, buf3, buf4;
	//Первая ступень конвейера	
	Stage_Agent Stage_Agent1(buf0, buf1, Func1);
	//Вторая ступень конвейера
	Stage_Agent Stage_Agent2(buf1, buf2, Func2);
	//Третья ступень конвейера
	Stage_Agent Stage_Agent3(buf2, buf3, Func3);
	//Четвертая ступень конвейера
	Stage_Agent Stage_Agent4(buf3, buf4, Func4);
	Stage_Agent1.start();
	Stage_Agent2.start();
	Stage_Agent3.start();
	Stage_Agent4.start();
	
	Tms = clock();
	//Параллельные вычисления
	for (int k = 0; k < Chunk; k++)
		send(buf0, X[k]);
	
	for (int k = Chunk; k < X.size(); k++){
        Z[k-Chunk] = receive(buf4);
		send(buf0, X[k]);
    }
	for (int k = X.size()-Chunk; k < X.size(); k++){
		Z[k] = receive(buf4);
	}
	Tms = (clock() - Tms) / CLOCKS_PER_SEC;
	cout << "Время параллельного алгоритма: " << Tms << " c." << endl;
	
	
	
}

