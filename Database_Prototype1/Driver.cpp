#include <iostream>
#include "Database.h"

using namespace std;

int main()
{
	cout << "hello world" << endl;

	Database db("test.db");
	db.AddTable("ADDRESSBOOK");
	//(FIRSTNAME,LASTNAME,ADDRESS,ZIPCODE,PHONENUMBER
	map<string, string> entry1 = { {"FIRSTNAME", "JOHN"},
								   {"LASTNAME", "JOHN2"},
								   { "ADDRESS", "JJJJJ 1111 2312" },
								   { "ZIPCODE", "97401" },
								   { "PHONENUMBER", "123-456-789" } };

	db.AddRecord("ADDRESSBOOK", entry1);
	db.PrintTable("ADDRESSBOOK");
}
