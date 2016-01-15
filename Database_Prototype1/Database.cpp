#include "Database.h"
#include "sqlite/sqlite3.h"
#include <iostream>
#include <string>

using namespace std;


Database::Database(const string fileName)
{
	m_pDB = NULL;
	int rs = sqlite3_open(fileName.c_str(), &m_pDB);

	if (rs)
		cout << "Error opening file: " << sqlite3_errmsg(m_pDB);
}

Database::~Database()
{
	if(m_pDB)
		sqlite3_close(m_pDB);
}

int Database::GetAllRecords(const string tableName, vector< map<string, string> >* pEntryList) const
{
	string sql = "SELECT * FROM " + tableName;
	
	return Exec(sql, pEntryList);
}

//int Database::AddField(const string tableName, const string fieldName)
//{
//
//}

int Database::AddRecord(const string tableName, const map<string, string> newEntry)
{
	string sql = "INSERT INTO " + tableName + " (FIRSTNAME,LASTNAME,ADDRESS,ZIPCODE,PHONENUMBER) "\
		"VALUES ('" + newEntry.at("FIRSTNAME").c_str() + "', '" + 
		newEntry.at("LASTNAME").c_str() + "', '" + 
		newEntry.at("ADDRESS").c_str() + "', '" + 
		newEntry.at("ZIPCODE").c_str() + "', '" + 
		newEntry.at("PHONENUMBER").c_str() + "' ); ";

	return Exec(sql);
}

//int Database::UpdateRecordByID(const string tableName, const pair<string, string> field)
//{
//
//
//
//
//}
//
//int Database::DeleteRecordByID(const string tableName, const string ID)
//{
//
//
//}
//
//const Entry Database::GetRecordByID(const string tableName, const string ID) const
//{
//
//
//}
//
int Database::AddTable(const string tableName)
{
	string sql = "CREATE TABLE IF NOT EXISTS " + tableName + "(\
		ID INTEGER PRIMARY KEY AUTOINCREMENT,\
		FIRSTNAME TEXT NOT NULL,\
		LASTNAME TEXT NOT NULL,\
		ADDRESS TEXT NOT NULL,\
		ZIPCODE TEXT NOT NULL,\
		PHONENUMBER TEXT);";
	
	return Exec(sql);
}

//int Database::DeleteTable(const string tableName)
//{
//
//
//}
//
//bool Database::IsTableExist(const string tableName) const
//{
//
//
//}

int Database::Exec(const string sql, vector<map <string, string> >* pEntryList) const
{
	sqlite3_stmt *stmt;
	int rs = sqlite3_prepare_v2(m_pDB, sql.c_str(), sql.length() + 1, &stmt, 0);

	if (rs)
	{
		cout << "Error preparing statement: " << sqlite3_errmsg(m_pDB) << endl;
		return 1;
	}


	while (true)
	{
		rs = sqlite3_step(stmt);

		if (rs == SQLITE_ROW)
		{
			map<string, string> entry;
			int colCount = sqlite3_column_count(stmt);
			for (int i = 0; i < colCount; i++)
			{
				if (sqlite3_column_type(stmt, i) == SQLITE_TEXT)
					entry.insert(pair<string, string>(sqlite3_column_name(stmt, i), (char *)sqlite3_column_text(stmt, i)));
				else if (sqlite3_column_type(stmt, i) == SQLITE_INTEGER)
					entry.insert(pair<string, string>(sqlite3_column_name(stmt, i), to_string(sqlite3_column_int(stmt, i))));
				else
					cout << "Field value type not recognized: " << sqlite3_column_type(stmt, i) << endl;
			}

			if(pEntryList != NULL)
				pEntryList->push_back(entry);
		}
		else if (rs == SQLITE_DONE)
		{
			break;
		}
		else if (rs == SQLITE_BUSY)
		{
			cout << "Database busy";
			return 3;
		}
		else
		{
			cout << "Database unknown error: " << sqlite3_errmsg(m_pDB);
			return 4;
		}
	}

	sqlite3_finalize(stmt);

	return 0;
}


int Database::PrintTable(string tableName)
{
	vector<map <string, string> > entryList;
	if (!GetAllRecords(tableName, &entryList))
	{
		for (auto& i : entryList)
		{
			for (auto& j : i)
				cout << j.first << " = " << j.second << endl;
			cout << endl;
		}
	}
	else
		return 1;
	return 0;
}
