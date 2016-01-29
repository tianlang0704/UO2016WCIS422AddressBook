#ifndef CIS422_ADDRESSBOOKMODEL_H
#define CIS422_ADDRESSBOOKMODEL_H

#include "DB/Database.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>

#define DATA_FILE_NAME "data.db"
#define ADDRESS1_HEADER_NAME "ADDRESS1"
#define ADDRESS2_HEADER_NAME "ADDRESS2"
#define CITY_HEADER_NAME "CITY"
#define STATE_HEADER_NAME "STATE"
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

	static int GetAllAddressBookNames(vector<string> *out);
	static int DeleteAddressbook(string name);

	int Open(const string addresBookName);
	int Search(const string str);
	int Save();

	void AddNewContact(const map<string, string> contactInfo);
	int UpdateContact(const int index, const map<string, string> contactInfo);
	int DeleteContact(const int index);

	int GetAllContacts(vector< map<string, string> > *out) const;
	int GetContact(const int index, map<string, string> *out);

	void SortBy(const string fieldName, bool bASC = true);

	int Import(const string fileName);
	int Export(const string fileName) const;

	void SetHeaders(vector<string> headers);
	vector<string> GetHeaders();
	void AddHeader(string newHeader);
	string GetName() const;
	void SetName(string name);
	vector< map<string, string> > &GetData();
	int GetSize();

	bool IsChanged();
	bool IsDup();

private:
	string m_AddressBookName;
	vector<string> m_Headers;
	vector< map< string, string> > m_ContactInfoList;
	bool m_Changed;
};

class ABSort
{
public:
	ABSort(string fieldName, bool bASC = true){ m_FieldName = fieldName; m_bASC = bASC; };
	bool operator() (const map<string, string> &a, const map<string, string> &b){ 
		bool res = false;
		if (m_bASC){
			if (a.at(m_FieldName) != b.at(m_FieldName)){
				res = a.at(m_FieldName) < b.at(m_FieldName);
			}else{
				bool aFirstnameExist = a.find(FIRSTNAME_HEADER_NAME) != a.end();
				bool bFirstnameExist = b.find(FIRSTNAME_HEADER_NAME) != b.end();
				if (aFirstnameExist && bFirstnameExist){
					res = a.at(FIRSTNAME_HEADER_NAME) < b.at(FIRSTNAME_HEADER_NAME);
				}
			}
		}else{
			if (a.at(m_FieldName) != b.at(m_FieldName)){
				res = a.at(m_FieldName) > b.at(m_FieldName);
			}else{
				bool aFirstnameExist = a.find(FIRSTNAME_HEADER_NAME) != a.end();
				bool bFirstnameExist = b.find(FIRSTNAME_HEADER_NAME) != b.end();
				if (aFirstnameExist && bFirstnameExist){
					res = a.at(FIRSTNAME_HEADER_NAME) < b.at(FIRSTNAME_HEADER_NAME);
				}
			}
		}
		return res; 
	};

private:
	string m_FieldName;
	bool m_bASC;
};

#endif //CIS422_ADDRESSBOOKMODEL_H