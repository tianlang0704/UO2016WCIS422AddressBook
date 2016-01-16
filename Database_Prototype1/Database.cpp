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

int Database::AddField(const string tableName, const string fieldName)
{
	string sql = "ALTER TABLE " + tableName + "\nADD " + fieldName + " TEXT";

	return Exec(sql);
}

int Database::AddRecord(const string tableName, const map<string, string> newEntry)
{
	string sql = "INSERT INTO " + tableName + " (";
	
	auto it = newEntry.cbegin();
	while(it != newEntry.cend())
	{
		sql.append((*it).first);

		++it;
		if (it != newEntry.cend())
			sql.append(",");
		else
			sql.append(") ");
	}

	sql.append("VALUES(");
	it = newEntry.cbegin();
	while (it != newEntry.cend())
	{
		sql.append("'" + (*it).second + "'");

		++it;
		if (it != newEntry.cend())
			sql.append(",");
		else
			sql.append(");");
	}

	return Exec(sql);
}

int Database::UpdateRecordByID(const string tableName, const string id, const pair<string, string> field)
{
	string sql = "UPDATE " + tableName + " SET " + field.first + "='" + field.second + "' WHERE  ID=" + id + ";";

	return Exec(sql);
}


int Database::DeleteRecordByID(const string tableName, const string id)
{
	string sql = "DELETE FROM " + tableName + " WHERE  ID=" + id + ";";

	return Exec(sql);

}

int  Database::GetRecordByID(const string tableName, const string id, map<string, string>* out) const
{
	vector< map<string, string > > entryList;
	int res;
	
	string sql = "SELECT * FROM " + tableName + " WHERE  ID=" + id + ";";
	
	res = Exec(sql, &entryList);

	if (!res)
	{
		if (!entryList.empty())
		{
			(*out) = entryList[0];
			return 0;
		}
		else
			return 1;
	}
	else
		return 2;
}


int Database::AddTable(const string tableName)
{
	return AddTable(tableName, vector<string>());
}

int Database::AddTable(const string tableName, const vector<string> defaultCals)
{
	string sql;

	if (defaultCals.empty())
		sql = "CREATE TABLE IF NOT EXISTS " + tableName + "(ID INTEGER PRIMARY KEY AUTOINCREMENT);";
	else
	{
		sql = "CREATE TABLE IF NOT EXISTS " + tableName + "(ID INTEGER PRIMARY KEY AUTOINCREMENT";
		for (auto& i : defaultCals)
			sql.append("," + i + " TEXT");
		sql.append(");");
	}

	return Exec(sql);
}

int Database::DeleteTable(const string tableName)
{
	string sql = "DROP TABLE IF EXISTS " + tableName;

	return Exec(sql);
}

bool Database::IsTableExist(const string tableName) const
{
	vector< map<string, string> > entryList;
	
	string sql = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='" + tableName + "';";

	Exec(sql, &entryList);

	return entryList[0].at("count(*)") == "1";
}

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
				else if (sqlite3_column_type(stmt, i) == SQLITE_NULL)
					entry.insert(pair<string, string>(sqlite3_column_name(stmt, i), ""));
				else
					cout << "Field type not recognized: " << sqlite3_column_type(stmt, i) << endl;
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
