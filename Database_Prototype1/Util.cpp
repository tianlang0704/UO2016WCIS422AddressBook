#include "Util.h"
#include <algorithm>

using namespace std;

struct match_rule {
	bool operator()(char c) {
		return !(isalnum(c) || (c == '_') || (c == '-') || (c == ' '));
	}
};

string Util::ToUpper(const string str)
{
	string cStr = str;
	for_each(cStr.begin(), cStr.end(), ::toupper);
	return cStr;
}

bool Util::IsLetterNumberDash(const string str)
{
	return find_if(str.begin(), str.end(), match_rule()) == str.end();
}