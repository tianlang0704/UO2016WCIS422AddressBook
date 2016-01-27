#include "AddressBookModel.h"

AddressBookModel::AddressBookModel(const AddressBookModel& abm)
{
	m_AddressBookName = abm.GetName();

	vector< map<string, string> > buff;
	abm.GetAllContacts(&buff);
	m_ContactInfoList = buff;
}

AddressBookModel::AddressBookModel()
{
	
}

AddressBookModel::AddressBookModel(vector<string> headers)
{
	m_Headers = headers;
}

AddressBookModel::~AddressBookModel()
{

}

int AddressBookModel::GetAllAddressBookNames(vector<string> *out)
{
	Database db(DATA_FILE_NAME);

	return db.GetAllTableNames(out);
}

int AddressBookModel::Open(const string addresBookName)
{
	Database db(DATA_FILE_NAME);

	if (!db.IsTableExist(addresBookName))
		return 1;

	m_AddressBookName = addresBookName;

	vector<string> conOut;
	db.GetColumnNames(m_AddressBookName, &conOut);
	SetHeaders(conOut);

	if (db.GetAllRecords(m_AddressBookName, &m_ContactInfoList))
		return 1;

	return 0;
}

int AddressBookModel::Search(const string str)
{
	Database db(DATA_FILE_NAME);

	if (!db.IsTableExist(m_AddressBookName))
		return 1;

	if (db.SearchStringInFields(m_AddressBookName, m_Headers, str, &m_ContactInfoList))
		return 1;

	return 0;
}

int AddressBookModel::Save()
{
	Database db(DATA_FILE_NAME);

	if (db.IsTableExist(m_AddressBookName))
		db.DeleteTable(m_AddressBookName);

	if (db.AddTable(m_AddressBookName, m_Headers))
		return 1;

	for (auto &i : m_ContactInfoList)
		db.AddRecord(m_AddressBookName, i);

	return 0;
}

void AddressBookModel::AddNewContact(const map<string, string> contactInfo)
{
	//*TODO: Check fields
	map<string, string> newContact;
	for (auto &i : m_Headers)
	{
		auto res = contactInfo.find(i);
		if (res == contactInfo.end())
			newContact[i] = "";
		else
			newContact[i] = (*res).second;
	}

	m_ContactInfoList.push_back(newContact);
}

int AddressBookModel::UpdateContact(const int index, const map<string, string> contactInfo)
{
	if ((int)m_ContactInfoList.size() <= index)
		return 1;

	for (auto &j : m_Headers)
		if(contactInfo.find(j) != contactInfo.end())
			m_ContactInfoList[index][j] = contactInfo.at(j);
		

	return 0;
}

int AddressBookModel::GetAllContacts(vector< map<string, string> > *out) const
{
	(*out) = m_ContactInfoList;

	return 0;
}


int AddressBookModel::GetContact(const int index, map<string, string> *out)
{
	if ((int)m_ContactInfoList.size() <= index)
		return 1;

	(*out) = m_ContactInfoList.at(index);
	return 0;
}

void AddressBookModel::SortBy(const string fieldName, bool bASC)
{
	ABSort objSort(fieldName, bASC);

	sort(m_ContactInfoList.begin(), m_ContactInfoList.end(), objSort);
}

//int AddressBookModel::Import(string fileName)
//{
//
//
//
//}
//

int AddressBookModel::Export(string fileName) const
{
	ofstream of(fileName);

	if (!of.is_open())
		return 1;

	for (auto &i : m_Headers)
		of << i << "/t";
	of << endl;

	for (auto &i : m_ContactInfoList)
	{
		for (auto &j : i)
		{
			of << j.second << "/t";
		}

		of << endl;
	}

	return 0;
}

void AddressBookModel::SetName(string name)
{
	m_AddressBookName = name;
}

string AddressBookModel::GetName() const
{
	return m_AddressBookName;
}

vector< map<string, string> > &AddressBookModel::GetData()
{
	return m_ContactInfoList;
}

void AddressBookModel::SetHeaders(vector<string> headers)
{
	m_Headers = headers;
}

void AddressBookModel::GetHeaders(vector<string> headers)
{
	headers = m_Headers;
}

int AddressBookModel::GetSize()
{
	return m_ContactInfoList.size();
}

bool AddressBookModel::IsDup()
{
	Database db(DATA_FILE_NAME);

	return db.IsTableExist(m_AddressBookName);
}