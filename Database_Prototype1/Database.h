#ifndef  CIS422_DATABASE
#define CIS422_DATABASE
#include <vector>
#include <map>
#include <string>

#include "sqlite/sqlite3.h"

using namespace std;

class Database
{
public:
	Database(const string fileName);
	~Database();

	int GetAllRecords(const string tableName, vector<map <string,string> >* pEntryList) const;
	int AddField(const string tableName, const string fieldName);
	int AddRecord(const string tableName, const  map<string, string> newEntry);
	int UpdateRecordByID(const string tableName, const string id, const pair<string, string> field);
	int DeleteRecordByID(const string tableName, const string id);
	int GetRecordByID(const string tableName, const string id, map<string, string>* out) const;
	int AddTable(const string tableName);
	int AddTable(const string tableName, const vector<string> defaultCols);
	int DeleteTable(const string tableName);
	bool IsTableExist(const string tableName) const;

	int Exec(const string sql, vector<map <string,string> >* entryList = NULL) const;

	int PrintTable(string tableName);

private:
	sqlite3 *m_pDB;
};



#endif // ! CIS422_DATABASE
