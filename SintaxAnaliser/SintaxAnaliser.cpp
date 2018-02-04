#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Lex
{
	int id;
	string s;
};

int main()
{
	ifstream Code("Code.txt");
	
	Lex *lexem = new Lex;
	if (!Code.eof())
	{
		Code >> lexem->id;
		getline(Code, lexem->s);

	}
	return 0;
}