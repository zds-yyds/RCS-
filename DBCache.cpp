#include "DBCache.h"



bool HY::RCSCache::loadFromDB(const std::string & host, const std::string & user, const std::string & password, const std::string & database)
{
	try {

		std::cout << "host: " << host << "\n"
			<< "user: " << user << "\n"
			<< "password: " << password << "\n"
			<< "database: " << database << "\n";
		// �������ݿ�
		//mysqlx::Session sess(host, 33060, user, password, database);
		mysqlx::Session sess("mysqlx://root:123456@127.0.0.1:33060/zds_test_rcs");
		// ִ�в�ѯ
		mysqlx::SqlResult result = sess.sql("SELECT * FROM rcs_data").execute();

		// �����������
		records.clear();
		name_index.clear();
		//freq_index.clear();

		// Ԥ�����ڴ棨������ܣ�
		records.reserve(100000); // ����Լ10������¼

								 // ���������
		for (mysqlx::Row row : result.fetchAll()) {
			// ��������ȡ����
			int id = row[0].get<int>();
			std::string name = row[1].get<std::string>();
			double freq = row[2].get<double>();
			std::string pol = row[3].get<std::string>();
			int az = row[4].get<int>();
			int el = row[5].get<int>();
			double rcs = row[6].get<double>();

			// ���ӵ����洢
			records.emplace_back(name, freq, pol, az, el, rcs);

			// ���ӵ�����
			const auto& record = &records.back();
			name_index[name].push_back(record);
			//freq_index[freq].push_back(record);
		}

		return true;
	}
	catch (const mysqlx::Error& err) {
		std::cerr << "mysqlx Error: " << err << std::endl;
		return false;
	}
	catch (const std::exception& e) {
		std::cerr << "error: " << e.what() << std::endl;
		return false;
	}
}

const std::vector<const HY::RCSRecord*>& HY::RCSCache::findByAircraft(const std::string & name)
{
	
	static const std::vector<const RCSRecord*> empty; // �ս��

	auto it = name_index.find(name);
	if (it != name_index.end()) {
		return it->second;
	}
	return empty;
	
}

//const std::vector<const HY::RCSRecord*>& HY::RCSCache::findByFrequency(int freq)
//{
//	
//	static const std::vector<const RCSRecord*> empty; // �ս��
//
//	auto it = freq_index.find(freq);
//	if (it != freq_index.end()) {
//		return it->second;
//	}
//	return empty;
//}
