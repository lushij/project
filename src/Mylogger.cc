#include "../include/Mylogger.h"
/* #include <log4cpp/PatternLayout.hh> */
/* #include <log4cpp/Priority.hh> */
/* #include <log4cpp/OstreamAppender.hh> */
/* #include <log4cpp/FileAppender.hh> */

#include <iostream>
using std::cout;
using std::endl;
 
namespace mylog
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

	// cout << "Mylogger()" << endl;

	PatternLayout * ptn1 = new PatternLayout();
	ptn1->setConversionPattern("%d %c [%p] %m%n");

	PatternLayout * ptn2 = new PatternLayout();
	ptn2->setConversionPattern("%d %c [%p] %m%n");

	//设置log屏幕版
	// OstreamAppender * ostreamAppender = new OstreamAppender("OstreamAppender", &cout);
	// ostreamAppender->setLayout(ptn1);


	//设置log文件版
	FileAppender * fileAppender = new FileAppender("FileAppender", "../log/SearchEngine.log");
	fileAppender->setLayout(ptn2);

	// _mycat.addAppender(ostreamAppender);
	_mycat.addAppender(fileAppender);
	_mycat.setPriority(log4cpp::Priority::DEBUG);
}

Mylogger::~Mylogger() 
{
	// cout << "~Mylogger()" << endl;
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
