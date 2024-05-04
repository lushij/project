#pragma once
#include<string>
#include<mysql/mysql.h>
#include<time.h>
using namespace std;
string ip = "127.0.0.1";
unsigned short port=3306;
string user = "root";
string password = "root123.";
string dbname = "cloudDisc";
class Connection
{
public:
	// 初始化数据库连接
	Connection();

	// 释放数据库连接资源
	~Connection();

	// 连接数据库
	bool connect(string ip, unsigned short port, string user, string password,string dbname);

	// 更新操作 insert、delete、update
	bool update(string sql);

	// 查询操作 select
	MYSQL_RES* query(string sql);

	void refreshAliveTime() { _aliveTime = clock(); }

	clock_t getAliveTime() const { return clock() - _aliveTime; }
	
    void insertSQL(string &sql)
    {
        this->connect(ip,port,user,password,dbname);
        this->update(sql);
    }
private:
	MYSQL* _conn; // 表示和MySQL Server的一条连接
	clock_t _aliveTime;//空闲时间即存活的时间
};

// 初始化数据库连接
Connection::Connection()
{
	_conn = mysql_init(nullptr);
}
// 释放数据库连接资源
Connection::~Connection()
{
	if (_conn != nullptr)
		mysql_close(_conn);
}
// 连接数据库
bool Connection::connect(string ip, unsigned short port, string user, string password,
	string dbname)
{
	MYSQL* p = mysql_real_connect(_conn, ip.c_str(), user.c_str(),
		password.c_str(), dbname.c_str(), port, nullptr, 0);
	return p != nullptr;
}
// 更新操作 insert、delete、update
bool Connection::update(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		return false;
	}
	return true;
}
// 查询操作 select
MYSQL_RES* Connection::query(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		return nullptr;
	}
	return mysql_use_result(_conn);
}

