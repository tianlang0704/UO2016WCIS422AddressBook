#include <iostream>
#include "Database.h"
#include "Util.h"

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

	cout << "=============================ADD RECORD============================" << endl;
	db.AddRecord(ADDRESSBOOK_NAME, entry);
	db.AddRecord(ADDRESSBOOK_NAME, entry);
	db.PrintTable(ADDRESSBOOK_NAME);

	cout << "===================ADD COLUMN IN EXISTING TABLE====================" << endl;
	db.AddField(ADDRESSBOOK_NAME, "MORECOLUMN");
	db.PrintTable(ADDRESSBOOK_NAME);

	cout << "===============UPDATING EXISTING ENTRY IN THE TABLE================" << endl;
	db.UpdateRecordByID(ADDRESSBOOK_NAME, "1", pair<string, string>("ADDRESS", "ADDRESS changed"));
	db.PrintTable(ADDRESSBOOK_NAME);

	cout << "===============DELETE EXISTING ENTRY IN THE TABLE==================" << endl;
	db.DeleteRecordByID(ADDRESSBOOK_NAME, "1");
	db.PrintTable(ADDRESSBOOK_NAME);

	cout << "=================GETTING AN EXISTING ENTRY BY ID===================" << endl;
	map<string, string> returnedEntry;
	db.GetRecordByID(ADDRESSBOOK_NAME, 2, &returnedEntry);
	cout << "retrieved entry:" << endl;
	for (auto& i : returnedEntry)
		cout << i.first << " = " << i.second << endl;
	cout << endl;

	cout << "======GETTING AN EXISTING ENTRY BY FIELD VALUE CONDITION===========" << endl;
	db.AddRecord(ADDRESSBOOK_NAME, entry);
	vector< map<string, string> > entryList;
	map<string, string> condFields = { { "FIRSTNAME", "JOHN" },
									   { "LASTNAME", "JOHN2" } };
	db.GetRecordByFields(ADDRESSBOOK_NAME, condFields, &entryList);
	cout << "retrieved entry id:" << endl;
	for (auto& i : entryList)
		cout << "ID = " << i.at("ID") << endl;
	cout << endl;

	cout << "=================CHECK IF TABLE EXISTS=============================" << endl;
	if (db.IsTableExist(ADDRESSBOOK_NAME))
		cout << "Table " << ADDRESSBOOK_NAME << " exists" << endl;
	else
		cout << "Table " << ADDRESSBOOK_NAME << " does not exist" << endl;

	cout << "=================DELETE EXISTING TABLE=============================" << endl;
	db.DeleteTable(ADDRESSBOOK_NAME);
	if (db.IsTableExist(ADDRESSBOOK_NAME))
		cout << "Table " << ADDRESSBOOK_NAME << " exists" << endl;
	else
		cout << "Table " << ADDRESSBOOK_NAME << " does not exist" << endl;

	system("pause");
	return 0;
}
