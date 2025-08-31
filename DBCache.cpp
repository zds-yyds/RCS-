#include "DBCache.h"

bool HY::RCSCache::loadFromDB(const std::string & host, const std::string & user, const std::string & password, const std::string & database)
{

	MYSQL* conn;
	MYSQL_RES* res;
	MYSQL_ROW row;

	// 初始化
	conn = mysql_init(nullptr);
	if (conn == nullptr) {
		std::cerr << "mysql_init() failed\n";
		return false;
	}

	// 连接数据库
	if (mysql_real_connect(conn,
		host.c_str(),   // 主机
		user.c_str(),        // 用户
		password.c_str(),      // 密码
		database.c_str(),// 数据库
		3306,          // 端口
		nullptr,       // Unix socket/管道
		0) == nullptr) // flags
	{
		std::cerr << "mysql_real_connect() failed: " << mysql_error(conn) << "\n";
		mysql_close(conn);
		return false;
	}

	// 执行查询
	if (mysql_query(conn, "SELECT * FROM rcs_data")) {
		std::cerr << "SELECT failed: " << mysql_error(conn) << "\n";
		mysql_close(conn);
		return false;
	}

	res = mysql_store_result(conn);
	if (res == nullptr) {
		std::cerr << "mysql_store_result() failed: " << mysql_error(conn) << "\n";
		mysql_close(conn);
		return false;
	}

	// 输出结果
	int num_fields = mysql_num_fields(res);
	MYSQL_FIELD* fields = mysql_fetch_fields(res);

	while ((row = mysql_fetch_row(res))) {
		// 按列索引读取每列
		int id = row[0] ? std::stoi(row[0]) : 0;
		std::string name = row[1] ? row[1] : "NULL";
		double freq = row[2] ? std::stod(row[2]) : 0.0;
		std::string pol = row[3] ? row[3] : "NULL";
		double az = row[4] ? std::stoi(row[4]) : 0;
		double el = row[5] ? std::stoi(row[5]) : 0;
		double rcs = row[6] ? std::stod(row[6]) : 0.0;

		// 构造你的记录对象
		records.emplace_back(name, freq, pol, az, el, rcs);

		// 插入索引 map
		const HY::RCSRecord rec = records.back();
		name_index[name].push_back(rec);
	}

	// 释放资源
	mysql_free_result(res);
	mysql_close(conn);
	return true;
}


std::vector<HY::RCSRecord> HY::RCSCache::findByAircraft(const std::string& name)
{
	static std::vector<RCSRecord> empty; //

	auto it = name_index.find(name);
	if (it != name_index.end()) {
		return it->second;
	}
	return empty;
}

