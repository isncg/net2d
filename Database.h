#pragma once
#include "sqlite\sqlite3.h"
#include"message.h"
class Database
{
	sqlite3* sqlite;
	std::string dbname;
public:
	Database(const char *filename);
	~Database();


	std::vector<POINT_MESSAGE>GetData();

};

