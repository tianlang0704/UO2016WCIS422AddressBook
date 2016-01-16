#include <iostream>
#include "Database.h"

#define ADDRESSBOOK_NAME "ADDRESSBOOK5"

using namespace std;

int main()
{
	Database db("test.db");

	vector<string> colNames = { "FIRSTNAME", "LASTNAME", "ADDRESS", "ZIPCODE", "PHONENUMBER" };
	db.AddTable(ADDRESSBOOK_NAME, colNames);
	
	map<string, string> entry = { {"FIRSTNAME", "JOHN"},
								   {"LASTNAME", "JOHN2"},
								   { "ADDRESS", "JJJJJ 1111 2312" },
								   { "ZIPCODE", "97401" },
								   { "PHONENUMBER", "123-456-789" } };

	db.AddRecord(ADDRESSBOOK_NAME, entry);
	db.AddRecord(ADDRESSBOOK_NAME, entry);
	db.PrintTable(ADDRESSBOOK_NAME);

	db.AddField(ADDRESSBOOK_NAME, "MORECOLUMN");
	db.PrintTable(ADDRESSBOOK_NAME);

	db.UpdateRecordByID(ADDRESSBOOK_NAME, "1", pair<string, string>("ADDRESS", "ADDRESS changed"));
	db.PrintTable(ADDRESSBOOK_NAME);

	db.DeleteRecordByID(ADDRESSBOOK_NAME, "1");
	db.PrintTable(ADDRESSBOOK_NAME);

	map<string, string> returnedEntry;
	db.GetRecordByID(ADDRESSBOOK_NAME, "2", &returnedEntry);
	cout << "retrieved entry:" << endl;
	for (auto& i : returnedEntry)
		cout << i.first << " = " << i.second << endl;
	cout << endl;

	if (db.IsTableExist(ADDRESSBOOK_NAME))
		cout << "Table " << ADDRESSBOOK_NAME << " exists" << endl;
	else
		cout << "Table " << ADDRESSBOOK_NAME << " does not exist" << endl;

	db.DeleteTable(ADDRESSBOOK_NAME);

	if (db.IsTableExist(ADDRESSBOOK_NAME))
		cout << "Table " << ADDRESSBOOK_NAME << " exists" << endl;
	else
		cout << "Table " << ADDRESSBOOK_NAME << " does not exist" << endl;

	return 0;
}
