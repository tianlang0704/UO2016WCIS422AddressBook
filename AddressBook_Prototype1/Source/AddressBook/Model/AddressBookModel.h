#include "DB/Database.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#define DATA_FILE_NAME "data.db"
#define ADDRESS_HEADER_NAME "ADDRESS"
#define FIRSTNAME_HEADER_NAME "FIRSTNAME"
#define LASTNAME_HEADER_NAME "LASTNAME"
#define ZIPCODE_HEADER_NAME "ZIP"
#define PHONE_HEADER_NAME "PHONE"

using namespace std;

class AddressBookModel
{
public:
	AddressBookModel(const AddressBookModel& abm);
	AddressBookModel(vector<string> headers);
	AddressBookModel();
	~AddressBookModel();

	int Open(const string addresBookName);
	int Search(const string str);
	int Save();

	void AddNewContact(const map<string, string> contactInfo);
	int UpdateContact(const int index, const map<string, string> contactInfo);

	int GetAllContacts(vector< map<string, string> > *out) const;
	int GetContact(const int index, map<string, string> *out);

	void SortBy(const string fieldName, bool bASC = true);

	int Import(const string fileName);
	int Export(const string fileName) const;


	void SetHeaders(vector<string> headers);
	void GetHeaders(vector<string> headers);
	string GetName() const;
	void SetName(string name);
	vector< map<string, string> > &GetData();
	int GetSize();

	bool IsDup();

private:
	string m_AddressBookName;
	vector<string> m_Headers;
	vector< map< string, string> > m_ContactInfoList;
};

class ABSort
{
public:
	ABSort(string fieldName, bool bASC = true){ m_FieldName = fieldName; m_bASC = bASC; };
	bool operator() (const map<string, string> &a, const map<string, string> &b){ return (m_bASC) ? (a.at(m_FieldName) < b.at(m_FieldName)) : (a.at(m_FieldName) > b.at(m_FieldName)); };

private:
	string m_FieldName;
	bool m_bASC;
};