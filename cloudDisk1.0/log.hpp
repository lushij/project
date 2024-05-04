#pragma once
#include "linuxheader.h"
#include <log4cpp/Category.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/FileAppender.hh>

using std::string;
using std::cout;
using std::endl;
 
namespace log
{

class Mylogger
{
public:
	enum Priority
    {
		ERROR = 300,
		WARN,
		INFO,
		DEBUG
	};

	static Mylogger * getInstance();

	static void destroy();

	template <class... Args>
	void warn(const char * msg, Args... args)
	{
		_mycat.warn(msg, args...);
	}

	template <class... Args>
	void error(const char * msg, Args... args)
	{
		_mycat.error(msg, args...);
	}

	template <class... Args>
	void info(const char * msg, Args... args)
	{
		_mycat.info(msg, args...);
	}

	template <class... Args>
	void debug(const char * msg, Args... args)
	{
		_mycat.debug(msg, args...);
	}

	void warn(const char * msg);
	void error(const char * msg);
	void info(const char * msg);
	void debug(const char * msg);

	void setPriority(Priority p);
private:
	Mylogger();
	~Mylogger();
private:
	log4cpp::Category & _mycat;

	static Mylogger * _pInstance;
};

#define prefix(msg) string("[")\
	.append(__FILE__).append(":")\
	.append(__FUNCTION__).append(":")\
	.append(std::to_string(__LINE__)).append("] ")\
	.append(msg).c_str()

//##__VA_ARGS__ 宏前面加上##的作用在于，当可变参数的个数为0时，
//这里的##起到把前面多余的","去掉的作用,否则会编译出错
#define LogError(msg, ...) Mylogger::getInstance()->error(prefix(msg), ##__VA_ARGS__)
#define LogWarn(msg, ...) Mylogger::getInstance()->warn(prefix(msg), ##__VA_ARGS__)
#define LogInfo(msg, ...) Mylogger::getInstance()->info(prefix(msg), ##__VA_ARGS__)
#define LogDebug(msg, ...) Mylogger::getInstance()->debug(prefix(msg), ##__VA_ARGS__)

}//end of namespace log


namespace log
{

Mylogger * Mylogger::_pInstance = nullptr;

Mylogger * Mylogger::getInstance() 
{
	if(nullptr == _pInstance)
    {
		_pInstance = new Mylogger();
	}

	return _pInstance;
}

void Mylogger::destroy()
{
	if(_pInstance) 
    {
		delete _pInstance;
        _pInstance = nullptr;
	}
}

Mylogger::Mylogger()
: _mycat(log4cpp::Category::getRoot().getInstance("MyCategory"))
{
	using namespace log4cpp;

	cout << "Mylogger()" << endl;

	PatternLayout * ptn1 = new PatternLayout();
	ptn1->setConversionPattern("%d %c [%p] %m%n");

	PatternLayout * ptn2 = new PatternLayout();
	ptn2->setConversionPattern("%d %c [%p] %m%n");

	OstreamAppender * ostreamAppender = new OstreamAppender("OstreamAppender", &cout);
	ostreamAppender->setLayout(ptn1);

	FileAppender * fileAppender = new FileAppender("FileAppender", "cloudDisk1.0.log");
	fileAppender->setLayout(ptn2);

	_mycat.addAppender(ostreamAppender);
	_mycat.addAppender(fileAppender);
	_mycat.setPriority(log4cpp::Priority::DEBUG);
}

Mylogger::~Mylogger() 
{
	cout << "~Mylogger()" << endl;
	log4cpp::Category::shutdown();
}


void Mylogger::warn(const char *msg)
{
	_mycat.warn(msg);
}

void Mylogger::error(const char *msg)
{
	_mycat.error(msg);
}

void Mylogger::info(const char *msg)
{
	_mycat.info(msg);
}

void Mylogger::debug(const char *msg)
{
	_mycat.debug(msg);
}

void Mylogger::setPriority(Priority p)
{
	switch(p)
	{
	case WARN:
		_mycat.setPriority(log4cpp::Priority::WARN);
		break;
	case ERROR:
		_mycat.setPriority(log4cpp::Priority::ERROR);
		break;
	case INFO:
		_mycat.setPriority(log4cpp::Priority::INFO);
		break;
	case DEBUG:
		_mycat.setPriority(log4cpp::Priority::DEBUG);
		break;
	}
}

}//end of namespace log
