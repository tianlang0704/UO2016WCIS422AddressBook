#include "Database.h"
#include "sqlite/sqlite3.h"
#include "Util.h"
#include <iostream>
#include <string>
#include <algorithm>

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

int Database::GetColumnNames(const string tableName, vector<string> *out) const
{
	vector< map<string, string> > output;

	string str = "PRAGMA table_info(" + tableName + ");";

	int res = Exec(str, &output);
	if (res)
		return res;

	for (auto &i : output)
		out->push_back(i.at("name"));

	return res;
}

int Database::GetAllRecords(const string tableName, vector< map<string, string> >* out, string sortField, bool bAsc) const
{
	if (!Util::IsLetterNumberDash(tableName) || !Util::IsLetterNumberDash(sortField))
		return 2;

	string order = " ORDER BY " + sortField + (bAsc ? " ASC;" : " DSC;");
	string sql = "SELECT * FROM " + Util::ToUpper(tableName) + order + ";";
	
	return Exec(sql, out);
}

int Database::SearchStringInFields(
	const string tableName,
	vector<string> fieldNames,
	string searchStr,
	vector< map<string, string> >* out,
	string sortField,
	bool bAsc) const
{
	string sql = "";
	if (searchStr == "")
	{
		sql = "SELECT * FROM " + Util::ToUpper(tableName) + ";";
	}
	else
	{ 
		if (!Util::IsLetterNumberDash(tableName) || !Util::IsLetterNumberDash(sortField))
			return 2;

		sql = "SELECT * FROM " + Util::ToUpper(tableName) + " WHERE ";
		string order = " ORDER BY " + sortField + (bAsc ? " ASC;" : " DSC;");

		auto it = fieldNames.cbegin();
		while (it != fieldNames.cend())
		{
			if (!Util::IsLetterNumberDash((*it)))
				return 2;
			sql.append(Util::ToUpper((*it) + " LIKE '"));
			//*TODO: Check value
			sql.append(searchStr + "'");

			++it;
			if (it != fieldNames.cend())
				sql.append(" OR ");
		}

		sql += order;
	}

	return Exec(sql, out);
}

int Database::GetRecordByFields(const string tableName, map<string, string> fieldValues, vector< map<string, string> >* out, string sortField, bool bAsc) const
{
	if (!Util::IsLetterNumberDash(tableName) || !Util::IsLetterNumberDash(sortField))
		return 2;

	string order = " ORDER BY " + sortField + (bAsc ? " ASC;" : " DSC;");
	string sql = "SELECT * FROM " + Util::ToUpper(tableName) + " WHERE ";

	auto it = fieldValues.cbegin();
	while (it != fieldValues.cend())
	{
		if (!Util::IsLetterNumberDash((*it).first))
			return 2;
		sql.append(Util::ToUpper((*it).first + "='"));
		//*TODO: Check value
		sql.append((*it).second + "'");

		++it;
		if (it != fieldValues.cend())
			sql.append(" AND ");
	}

	sql += order;

	return Exec(sql, out);
}

int  Database::GetRecordByID(const string tableName, const int id, map<string, string>* out, string sortField, bool bAsc) const
{
	if (!Util::IsLetterNumberDash(tableName) || !Util::IsLetterNumberDash(sortField))
		return 2;

	vector< map<string, string > > entryList;
	int res;

	string order = " ORDER BY " + sortField + (bAsc ? " ASC;" : " DSC;");
	string sql = "SELECT * FROM " + Util::ToUpper(tableName) + " WHERE  ID=" + to_string(id) + order + ";";

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

int Database::UpdateRecordByID(const string tableName, const int id, const pair<string, string> field)
{
	if (!Util::IsLetterNumberDash(tableName) || !Util::IsLetterNumberDash(field.first))
		return 2;

	string sql = "UPDATE " + Util::ToUpper(tableName) + " SET " + Util::ToUpper(field.first) + "='" + field.second + "' WHERE  ID=" + to_string(id) + ";";

	return Exec(sql);
}


int Database::DeleteRecordByID(const string tableName, const int id)
{
	if (!Util::IsLetterNumberDash(tableName))
		return 2;

	string sql = "DELETE FROM " + Util::ToUpper(tableName) + " WHERE  ID=" + to_string(id) + ";";

	return Exec(sql);

}

int Database::AddField(const string tableName, const string fieldName)
{
	if (!Util::IsLetterNumberDash(tableName) || !Util::IsLetterNumberDash(fieldName))
		return 2;

	string sql = "ALTER TABLE " + Util::ToUpper(tableName) + " ADD " + Util::ToUpper(fieldName) + " TEXT";

	return Exec(sql);
}

int Database::AddRecord(const string tableName, const map<string, string> newEntry)
{
	if (!Util::IsLetterNumberDash(tableName))
		return 2;

	string sql = "INSERT INTO " + Util::ToUpper(tableName) + " (";
	
	auto it = newEntry.cbegin();
	while(it != newEntry.cend())
	{
		if (!Util::IsLetterNumberDash((*it).first))
			return 2;

		if (Util::ToUpper((*it).first).compare("ID") == 0)
			continue;

		sql.append(Util::ToUpper((*it).first));

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
		//*TODO: Check values
		sql.append("'" + (*it).second + "'");

		++it;
		if (it != newEntry.cend())
			sql.append(",");
		else
			sql.append(");");
	}

	return Exec(sql);
}

int Database::AddTable(const string tableName)
{
	return AddTable(Util::ToUpper(tableName), vector<string>());
}

int Database::AddTable(const string tableName, const vector<string> defaultCals)
{
	if (!Util::IsLetterNumberDash(tableName))
		return 2;

	string sql;

	if (defaultCals.empty())
		sql = "CREATE TABLE IF NOT EXISTS " + Util::ToUpper(tableName) + "(ID INTEGER PRIMARY KEY AUTOINCREMENT);";
	else
	{
		sql = "CREATE TABLE IF NOT EXISTS " + Util::ToUpper(tableName) + "(ID INTEGER PRIMARY KEY AUTOINCREMENT";
		for (auto& i : defaultCals)
		{
			if (!Util::IsLetterNumberDash(i))
				return 2;
			sql.append("," + Util::ToUpper(i) + " TEXT");
		}
		sql.append(");");
	}

	return Exec(sql);
}

int Database::DeleteTable(const string tableName)
{
	if (!Util::IsLetterNumberDash(tableName))
		return 2;

	string sql = "DROP TABLE IF EXISTS " + Util::ToUpper(tableName);

	return Exec(sql);
}

bool Database::IsTableExist(const string tableName) const
{	
	if (!Util::IsLetterNumberDash(tableName))
		return false;

	string sql = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='" + Util::ToUpper(tableName) + "';";

	vector< map<string, string> > entryList;
	Exec(sql, &entryList);

	return entryList[0].at("count(*)") == "1";
}

int Database::Exec(const string sql, vector<map <string, string> >* pEntryList) const
{
	sqlite3_stmt *stmt;
	int rs = sqlite3_prepare_v2(m_pDB, sql.c_str(), (int)sql.length() + 1, &stmt, 0);

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
	if (!Util::IsLetterNumberDash(tableName))
		return 2;

	vector<map <string, string> > entryList;
	if (!GetAllRecords(Util::ToUpper(tableName), &entryList))
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

int Database::GetAllTableNames(vector<string> *out) const
{
	vector< map<string, string> > output;

	string str = "SELECT name FROM sqlite_master WHERE type='table' AND name!='sqlite_sequence'";

	int res = Exec(str, &output);
	if (res)
		return res;

	for (auto &i : output)
		out->push_back(i.at("name"));
	
	return res;
}