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
	//Only constructor that creates the data file if it does not already exist
	//
	//input:
	//	fileName: file name
	Database(const string fileName);
	~Database();

	//Get the names of all the columns
	//
	//input:
	//	tableName: table name
	//output:
	//	out: an array that holds the name of each column
	//return: 0 when success, otherwise fail
	int GetColumnNames(const string tableName, vector<string> *out) const;

	//Get all records from a table
	//
	//input:
	//	tableName: table name
	//	sortField: column name to sort on
	//	bAsc: is asc or dsc
	//output:
	//	out: an array of dictionary that acts as a result table
	//return: 0 when success, otherwise fail
	int GetAllRecords(const string tableName, vector<map <string, string> >* out, string sortField = "ID", bool bAsc = true) const;

	//Search string in fields
	//
	//input:
	//	tableName: table name
	//	fieldValues: an array used to indicate the condition field names
	//	searchStr: string to search
	//	sortField: column name to sort on
	//	bAsc: asc or dsc
	//output:
	//	out: an array of dictionaries that act as a result table
	//return: 0 when success, otherwise fail
	int Database::SearchStringInFields(
		const string tableName,
		vector<string> fieldNames,
		string searchStr,
		vector< map<string, string> >* out,
		string sortField = "ID",
		bool bAsc = true) const;

	//Get all records that match the column name and value from a table
	//
	//input:
	//	tableName: table name
	//	fieldValues: a dictionary used to indicate the condition field name and value
	//	sortField: column name to sort on
	//	bAsc: asc or dsc
	//output:
	//	out: an array of dictionaries that act as a result table
	//return: 0 when success, otherwise fail
	int GetRecordByFields(const string tableName, map<string, string> fieldValues, vector< map<string, string> >* out, string sortField = "ID", bool bAsc = true) const;
	
	//Get the record with the ID matches the ID indicated
	//
	//input:
	//	tableName: table name
	//	ID:	id of the entry
	//	sortField: column name to sort on
	//	bAsc: asc or dsc
	//output:
	//	out: an array of dictionaries that act as a result table
	//return: 0 when success, otherwise fail
	int GetRecordByID(const string tableName, const int id, map<string, string>* out, string sortField = "ID", bool bAsc = true) const;

	//Update the record with the ID matches the ID indicated
	//
	//input:
	//	tableName: table name
	//	ID:	id of the entry
	//	field: field consists of field name and field value to be updated
	//return: 0 when success, otherwise fail
	int UpdateRecordByID(const string tableName, const int id, const pair<string, string> field);

	//Delete the record with the ID matches the ID indicated
	//
	//input:
	//	tableName: table name
	//	ID:	id of the entry
	//return: 0 when success, otherwise fail
	int DeleteRecordByID(const string tableName, const int id);

	//Add a field(column) into existing table
	//
	//input:
	//	tableName: table name
	//	fieldName: field name (added fields are all in TEXT format in database)
	//return: 0 when success, otherwise fail
	int AddField(const string tableName, const string fieldName);

	//Add a record(entry) into existing table
	//
	//input:
	//	tableName: table name
	//	newEntry: a dictionary represents the new entry. It consists of field names and values
	//return: 0 when success, otherwise fail
	int AddRecord(const string tableName, const  map<string, string> newEntry);

	//Add a table into the database if the table is not already exist
	//
	//input:
	//	tableName: table name
	//return: 0 when success, otherwise fail
	int AddTable(const string tableName);

	//Add a table with default columns into the database if the table is not already exist
	//
	//input:
	//	tableName: table name
	//	defaultCols: default column names
	//return: 0 when success, otherwise fail
	int AddTable(const string tableName, const vector<string> defaultCols);

	//Delete an existing table in the database
	//
	//input:
	//	tableName: table name
	//	defaultCols: default column names
	//return: 0 when success, otherwise fail
	int DeleteTable(const string tableName);

	//Check if table with the name indicated already exist in the database
	//
	//input:
	//	tableName: table name
	//return: true if exists, false if not.
	bool IsTableExist(const string tableName) const;

	//Ouput all the table names
	//
	//output:
	//	out: an array of table names
	//return: 0 when success, otherwise fail
	int GetAllTableNames(vector<string> *out) const;

	//Execute a sql statement on the database, all other functions are 
	//implemented using this function
	//
	//input:
	//	sql: sql statment to execute
	//	entryList: an array of dictionary that acts as a result table, if
	//return: 0 when success, otherwise fail
	int Exec(const string sql, vector<map <string,string> >* entryList = NULL) const;

	//Print all the contents in a table
	//
	//input:
	//	tableName: table name
	//return: 0 when success, otherwise fail
	int PrintTable(string tableName);

private:
	sqlite3 *m_pDB;
};



#endif // ! CIS422_DATABASE
