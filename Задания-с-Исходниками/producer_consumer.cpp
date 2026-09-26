// producer_consumer.cpp 
#include <agents.h>
#include <array>
#include <algorithm>
#include <iostream>
#include <locale.h>

using namespace concurrency;
using namespace std;

// Базовый агент, производящий значение. 
class producer_agent : public agent
{
private:
	// Буфер - приемник информации
	ITarget<double>& _target;
public:
	explicit producer_agent(ITarget<double>& target)
		: _target(target)
	{
	}
protected:
	void run()
	{
		// Например, создаем предопределенный массив данных.  
		// В реальности, их нужно брать из сети или из базы данных 
		array<double, 6> quotes = { 24.44, 24.65, 24.99, 23.76, 22.30, 25.89 };
		// Посылаем каждое значение в буфер.
		for_each(begin(quotes), end(quotes), [&](double quote) {
			send(_target, quote);
			// Ждем, прежде чем послать следующее значение.
			concurrency::wait(20);
		});
		// Посылаем отрицательное значение, чтобы указать завершение обработки.
		send(_target, -1.0);
		// Переводим агента в завершенное состояние
		done();
	}
};

// Базовый агент, принимающий значения 
class consumer_agent : public agent
{
private:
	// Входной буфер для чтения значений
	ISource<double>& _source;
public:
	explicit consumer_agent(ISource<double>& source)
		: _source(source)
	{
	}

protected:
	void run()
	{
		// Читаем значения из входного буфера,
		// пока не получим отрицательное значение 
		double quote;
		while ((quote = receive(_source)) >= 0.0)
		{
			// Печатаем значение
			cout << "Текущее значение " << quote <<  endl;

			// Ожидаем перед вводом следующего значения
			concurrency::wait(10);
		}

		// Переводим агента в завершенное состояние
		done();
	}
};

int wmain()
{
	setlocale(LC_ALL, ".ACP");
	// Буфер сообщений, разделяемый агентами
	overwrite_buffer<double> buffer;

	// Создать и запустить агента-производителя и агента-потребителя
	producer_agent producer(buffer);
	consumer_agent consumer(buffer);
	producer.start();
	consumer.start();

	// Ждать завершения работы агентов
	agent::wait(&producer);
	agent::wait(&consumer);
}

