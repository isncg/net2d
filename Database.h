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


	void GetData(std::vector<POINT_MESSAGE>& result_buffer);
	void GetData(std::list<POINT_MESSAGE>& result_list);
	void PutData(UINT point_type, float x, float y, UINT time_stamp, UINT frame_id, UINT frame_index);
};

