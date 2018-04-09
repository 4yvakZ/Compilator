#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>
#include <stdio.h>
using namespace std;

static ifstream Code("D:\\IT_files\\Compilator\\Files\\Code.txt");
static ofstream POLIZ("D:\\IT_files\\Compilator\\Files\\POLIZ.txt");

struct Lex {
	int id;
	string s;
};
static Lex *lexem = new Lex;
static int strings = 1;
int LineNum;

//TID
enum Type {
	Int,
	Double,
	Bool,
	String,
	Operation
};
struct Tid {
	Type type;
	Tid *next;
	string name;
	string value = "nullptr";
};
static Tid *L = new Tid;
static Tid *L1 = nullptr;
static Tid *SupportTid = new Tid;
static Tid *CurrentSupport = SupportTid;
void AddID(string name, string value, Type type, Tid *&L) {
	Tid *p;
	for (p = L; p != nullptr && p != L1; p = p->next) {
		if (p->name == name) {
			throw(1);
		}
	}
	p = new Tid;
	p->type = type;
	p->name = name;
	p->value = value;
	p->next = L;
	L = p;
	p = CurrentSupport;
	p = new Tid;
	p->type = type;
	p->name = name;
	p->value = value;
	p->next = CurrentSupport;
	CurrentSupport = p;
	return;
}
Type CheckID(string name, Tid *&L) {
	for (Tid *p = L; p != nullptr; p = p->next) {
		if (p->name == name) {
			return p->type;
		}
	}
	throw(2);
}
void DeleteElem(Tid *&L) {
	if (L == nullptr)return;
	Tid *p = L;
	L = L->next;
	p->next = nullptr;
	delete p;
}
void DeleteAll(Tid *&L) {
	for (; L != nullptr; DeleteElem(L));
}
void DeleteUntil(Tid *&L, Tid *&L1) {
	Tid *q = L;
	if (L == L1)return;
	for (; q->next != L1; q = q->next);
	q->next = nullptr;
	q = L;
	L = L1;
	DeleteAll(q);
}
/*
void CopyUntil(Tid *L, Tid *L1) {
	Tid *p = L;
	while (p != L1) {
		CurrentSupport->name = p->name;
		CurrentSupport->type = p->type;
		CurrentSupport->value = p->value;
		CurrentSupport->next = new Tid;
		CurrentSupport = CurrentSupport->next;
		
		p = p->next;
	}
	CurrentSupport->next = nullptr;
}*/

//POLIZ part
bool Break = false;
bool ShiftPOLIZ = false;
bool ShiftSwitch = false;
string GoTo = "NAN";
int ShiftUntilPOLIZ;
void ChangeVar(string VarName, string value) {
	for (Tid *p = L; p != nullptr; p = p->next)
		if (p->name == VarName) {
			p->value = value;
			return;
		}
	for (Tid *p = SupportTid; p != nullptr; p = p->next)
		if (p->name == VarName) {
			p->value = value;
			return;
		}
	cout << "Cannot found " + VarName;
	return;
}
string GetValue(string VarName) {
	for (Tid *p = L; p != nullptr; p = p->next)
		if (p->name == VarName) return p->value;
	for (Tid *p = SupportTid; p != nullptr; p = p->next)
		if (p->name == VarName) return p->value;
	return "GetValue crashs";
}
Type GetType(string VarName) {
	for (Tid *p = L; p != nullptr; p = p->next)
		if (p->name == VarName) return p->type;
	return Operation;
}
void Shift(string &a) {
	for (int i = 1; i < a.length(); i++) a[i - 1] = a[i];
	a = a.erase(a.length() - 1,1);
	return;
}
struct RESULT {
	string s;
	int ID;
	Type type;
	int LineNum;
	RESULT *next = nullptr;
};

static RESULT *MainPOLIZ = new RESULT;
static RESULT *CurrentPOLIZ = MainPOLIZ;
static RESULT *LResult = new RESULT;

Type TypeCheck(Type type2, Type type1, string sign) {
	if (sign == "=") {
		if (type1 == type2) return type1;
		if (type1 == Double || type2 == Double)
			if (type1 != Int && type2 != Int) throw(3);
		throw (3);
		return Bool;
	}
	if (sign == "+=" || sign == "-=" || sign == "*=" || sign == "/=" || sign == "+" || sign == "-" || sign == "*" || sign == "/") {
		if (type1 != Int || type1 != Double) throw(3);
		if (type2 != Int || type2 != Double) throw(3);
		if (type1 == Double || type2 == Double) return Double;
		return Int;
	}
	if (sign == "**=" || sign == "**") {
		if (type1 != Int) throw(3);
		if (type2 != Int && type2 != Double) throw(3);
		if (type2 == Double) return Double;
		return Int;
	}
	if (sign == ".=" || sign == ".") {
		if (type1 != String && type2 != String) throw(3);
		return String;
	}
	if (sign == "%=" || sign == "%" || sign == "&=" || sign == "|=" || sign == "^=" || sign == "<<=" || sign == ">>=" || sign == "|" || sign == "^" || sign == "&" || sign == "<<" || sign == ">>") {
		if (type1 != Int || type2 != Int) throw(3);
		return Int;
	}
	if (sign == "||" || sign == "&&" || sign == "==" || sign == "!=" || sign == "===" || sign == "!==" || sign =="<=>" || sign == "!") {
		return Bool;
	}//expended true
	if (sign == "<" || sign == "<=" || sign == ">" || sign == ">=") {
		if (type2 != Int && type2 != Double) throw(3);
		if (type1 != Int && type1 != Double) throw(3);
		return Bool;
	}
	if (sign == "++" || sign == "--" || sign == "++Prev" || sign == "--Prev") {
		if (type1 != Int && type1 != Double) throw(3);
		return type1;
	}
	cout << "WTF? Can't determine op.sign: " << sign << endl;
	return Bool;
}
void DelRESULT() {
	RESULT *p = LResult;
	LResult = LResult->next;
	delete p;
}
void ClearRESULT() { //clear all stek
	while (LResult != nullptr) {
		RESULT*p = LResult->next;
		delete LResult;
		LResult = p;
	}
	return;
}
double GetNum(string a) {
	if (a == "nullptr") return 0;
	double sum = 0;
	int NumAfterComa = 0;
	for (int i = 0; i < a.length(); i++) {
		if (a[i] == '-') i++;
		if (NumAfterComa) NumAfterComa++;
		if (a[i] == '.') { NumAfterComa++; i++; }
		sum *= 10;
		sum += a[i] - '0';
	}
	while (NumAfterComa-- > 0) sum /= 10;
	if (a[0] == '-') return -sum;
	return sum;
}
void RunOperation() {
	if (LResult->s == "=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		ChangeVar(Operand2->s, Operand1->s);
		LResult->s = Operand1->s;
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "=");
		//cout << Operand2->s << " = " << Operand1->s << endl;
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "+=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		string Operand2Name = Operand2->s;
		Operand2->s = GetValue(Operand2->s);
		double sum = GetNum(Operand2->s) + GetNum(Operand1->s);
		ostringstream ostr;
		ostr << sum;
		ChangeVar(Operand2Name, ostr.str());
		LResult->s = ostr.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "+=");
		//cout << Operand2Name << " = " << ostr.str() << endl;
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "-=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s), Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		string Operand2Name = Operand2->s;
		double sum = GetNum(GetValue(Operand2->s)) - GetNum(Operand1->s);
		ostringstream strsum;
		strsum << sum;
		ChangeVar(Operand2Name, strsum.str());
		//cout << Operand2Name << " = " << strsum.str() << endl;
		LResult->s = strsum.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "-=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "*=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s), Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		string Operand2Name = Operand2->s;
		double sum = GetNum(GetValue(Operand2->s)) * GetNum(Operand1->s);
		ostringstream strsum;
		strsum << sum;
		ChangeVar(Operand2Name, strsum.str());
		//cout << Operand2Name << " = " << strsum.str() << endl;
		LResult->s = strsum.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "*=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "**=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s), Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		string Operand2Name = Operand2->s;
		double sum = 1.0;
		for (double i = GetNum(Operand1->s); i > 0; i--) {
			sum *= GetNum(GetValue(Operand2->s));
		}
		ostringstream strsum;
		strsum << sum;
		ChangeVar(Operand2Name, strsum.str());
		//cout << Operand2Name << " = " << strsum.str() << endl;
		LResult->s = strsum.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "**=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "/=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s), Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		string Operand2Name = Operand2->s;
		if (GetNum(Operand1->s) == 0) throw(0);
		double sum = GetNum(GetValue(Operand2->s)) / GetNum(Operand1->s);
		ostringstream strsum;
		strsum << sum;
		ChangeVar(Operand2Name, strsum.str());
		//cout << Operand2Name << " = " << strsum.str() << endl;
		LResult->s = strsum.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "/=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == ".="){
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s), Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		string Operand2Name = Operand2->s;
		Operand2->s = GetValue(Operand2->s);
		string sum = Operand2->s.erase(Operand2->s.length()-1,1) + Operand1->s.erase(0,1);
		ChangeVar(Operand2Name, sum);
		//cout << Operand2Name << " = " << sum << endl;
		LResult->s = sum;
		LResult->type = TypeCheck(Operand2->type, Operand1->type, ".=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "%=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s), Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		string Operand2Name = Operand2->s;
		if (GetNum(Operand1->s) == 0) throw(0);
		double sum = int(GetNum(GetValue(Operand2->s))) %  int(GetNum(Operand1->s));
		ostringstream strsum;
		strsum << sum;
		ChangeVar(Operand2Name, strsum.str());
		//cout << Operand2Name << " = " << strsum.str() << endl;
		LResult->s = strsum.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "%=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "&="){
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s), Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		string Operand2Name = Operand2->s;
		double sum = int(GetNum(GetValue(Operand2->s))) & int(GetNum(Operand1->s));
		ostringstream strsum;
		strsum << sum;
		ChangeVar(Operand2Name, strsum.str());
		//cout << Operand2Name << " = " << strsum.str() << endl;
		LResult->s = strsum.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "&=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "|=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s), Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		string Operand2Name = Operand2->s;
		double sum = int(GetNum(GetValue(Operand2->s))) | int(GetNum(Operand1->s));
		ostringstream strsum;
		strsum << sum;
		ChangeVar(Operand2Name, strsum.str());
		//cout << Operand2Name << " = " << strsum.str() << endl;
		LResult->s = strsum.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "|=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "^=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s), Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		string Operand2Name = Operand2->s;
		double sum = int(GetNum(GetValue(Operand2->s))) ^ int(GetNum(Operand1->s));
		ostringstream strsum;
		strsum << sum;
		ChangeVar(Operand2Name, strsum.str());
		//cout << Operand2Name << " = " << strsum.str() << endl;
		LResult->s = strsum.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "^=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "<<=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s), Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		string Operand2Name = Operand2->s;
		double sum = int(GetNum(GetValue(Operand2->s))) << int(GetNum(Operand1->s));
		ostringstream strsum;
		strsum << sum;
		ChangeVar(Operand2Name, strsum.str());
		//cout << Operand2Name << " = " << strsum.str() << endl;
		LResult->s = strsum.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "<<=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == ">>=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s), Operand1->s = GetValue(Operand1->s); }
		Shift(Operand2->s);
		string Operand2Name = Operand2->s;
		double sum = int(GetNum(GetValue(Operand2->s))) >> int(GetNum(Operand1->s));
		ostringstream strsum;
		strsum << sum;
		ChangeVar(Operand2Name, strsum.str());
		//cout << Operand2Name << " = " << strsum.str() << endl;
		LResult->s = strsum.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, ">>=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "||") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (Operand1->s == "true" || Operand2->s == "true")	LResult->s = "true";
		else LResult->s = "false";
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "||");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "&&") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (Operand1->s == "true" && Operand2->s == "true")	LResult->s = "true";
		else LResult->s = "false";
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "&&");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "|") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		int sum = int(GetNum(Operand1->s)) | int(GetNum(Operand2->s));
		ostringstream sumstr;
		sumstr << sum;
		LResult->s = sumstr.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "|");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "^") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		int sum = int(GetNum(Operand1->s)) ^ int(GetNum(Operand2->s));
		ostringstream sumstr;
		sumstr << sum;
		LResult->s = sumstr.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "^");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "&") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		int sum = int(GetNum(Operand1->s)) & int(GetNum(Operand2->s));
		ostringstream sumstr;
		sumstr << sum;
		LResult->s = sumstr.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "&");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "==") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (Operand1->s == Operand2->s) LResult->s = "true";
		else LResult->s = "false";
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "==");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "!=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (Operand1->s != Operand2->s) LResult->s = "true";
		else LResult->s = "false";
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "!=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "===") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (Operand1->s == Operand2->s && Operand1->type == Operand2->type) LResult->s = "true";
		else LResult->s = "false";
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "===");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "!==") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (Operand1->s != Operand2->s && Operand1->type == Operand2->type) LResult->s = "true";
		else LResult->s = "false";
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "!==");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "<=>") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (Operand1->s != Operand2->s && Operand1->type == Operand2->type) LResult->s = "true";
		else LResult->s = "false";
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "<=>");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "<") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (GetNum(Operand2->s) < GetNum(Operand1->s)) LResult->s = "true";
		else LResult->s = "false";
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "<");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == ">") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (GetNum(Operand1->s) > GetNum(Operand2->s)) LResult->s = "true";
		else LResult->s = "false";
		LResult->type = TypeCheck(Operand2->type, Operand1->type, ">");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "<=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (GetNum(Operand1->s) <= GetNum(Operand2->s)) LResult->s = "true";
		else LResult->s = "false";
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "<=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == ">=") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (GetNum(Operand1->s) >= GetNum(Operand2->s)) LResult->s = "true";
		else LResult->s = "false";
		LResult->type = TypeCheck(Operand2->type, Operand1->type, ">=");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "<<") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		int sum = int(GetNum(Operand1->s)) << int(GetNum(Operand2->s));
		ostringstream sumstr;
		sumstr << sum;
		LResult->s = sumstr.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "<<");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == ">>") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		int sum = int(GetNum(Operand1->s)) >> int(GetNum(Operand2->s));
		ostringstream sumstr;
		sumstr << sum;
		LResult->s = sumstr.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, ">>");
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "+"){
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		double sum = GetNum(Operand1->s) + GetNum(Operand2->s);
		ostringstream ostr;
		ostr << sum;
		LResult->s = ostr.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "+");
		//cout << Operand2->s << "+" << Operand1->s << "=" << ostr.str() << endl;
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "-") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		double sum = GetNum(Operand2->s) - GetNum(Operand1->s);
		ostringstream ostr;
		ostr << sum;
		LResult->s = ostr.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "-");
		//cout << Operand2->s << "-" << Operand1->s << "=" << ostr.str() << endl;
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == ".") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		LResult->s = Operand2->s.erase(Operand2->s.length()-1,1) + Operand1->s.erase(0,1);
		LResult->type = TypeCheck(Operand2->type, Operand1->type, ".");
		//cout << Operand2->s << "+" << Operand1->s << "=" << LResult->s << endl;
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "*") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		double sum = GetNum(Operand1->s) * GetNum(Operand2->s);
		ostringstream ostr;
		ostr << sum;
		LResult->s = ostr.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "*");
		//cout << Operand2->s << "*" << Operand1->s << "=" << ostr.str() << endl;
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "/") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (GetNum(Operand1->s) == 0) throw(0);
		double sum = GetNum(Operand1->s) / GetNum(Operand2->s);
		ostringstream ostr;
		ostr << sum;
		LResult->s = ostr.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "/");
		//cout << Operand2->s << "/" << Operand1->s << "=" << ostr.str() << endl;
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "%") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (GetNum(Operand1->s) == 0) throw(0);
		double sum = int(GetNum(Operand1->s)) % int(GetNum(Operand2->s));
		ostringstream ostr;
		ostr << sum;
		LResult->s = ostr.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "%");
		//cout << Operand2->s << "%" << Operand1->s << "=" << ostr.str() << endl;
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "!") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand = LResult;
		LResult = new RESULT;
		LResult->next = Operand->next;
		if (Operand->s[0] == '$') { Shift(Operand->s); Operand->s = GetValue(Operand->s); }
		if (Operand->s != "false" && Operand->s != "0") LResult->s = "false";
		else LResult->s = "true";
		LResult->type = TypeCheck(Bool, Operand->type, "!");
		delete Operand;
		return;
	}
	if (LResult->s == "++") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand = LResult;
		LResult = new RESULT;
		LResult->next = Operand->next;
		Shift(Operand->s);
		string OperandName = Operand->s;
		Operand->s = GetValue(Operand->s);
		double sum = GetNum(Operand->s) + 1;
		ostringstream sumstr;
		sumstr << sum;
		ChangeVar(OperandName, sumstr.str());
		LResult->s = Operand->s;
		LResult->type = TypeCheck(Bool, Operand->type, "++");
		//cout << OperandName << " = " << sumstr.str() << endl;
		delete Operand;
		return;
	}
	if (LResult->s == "--") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand = LResult;
		LResult = new RESULT;
		LResult->next = Operand->next;
		Shift(Operand->s);
		string OperandName = Operand->s;
		Operand->s = GetValue(Operand->s);
		double sum = GetNum(Operand->s) - 1;
		ostringstream sumstr;
		sumstr << sum;
		ChangeVar(OperandName, sumstr.str());
		LResult->s = Operand->s;
		LResult->type = TypeCheck(Bool, Operand->type, "--");
		//cout << OperandName << " = " << sumstr.str() << endl;
		delete Operand;
		return;
	}
	if (LResult->s == "++Prev") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand = LResult;
		LResult = new RESULT;
		LResult->next = Operand->next;
		Shift(Operand->s);
		string OperandName = Operand->s;
		Operand->s = GetValue(Operand->s);
		double sum = GetNum(Operand->s) + 1;
		ostringstream sumstr;
		sumstr << sum;
		ChangeVar(OperandName, sumstr.str());
		LResult->s = sumstr.str();
		LResult->type = TypeCheck(Bool, Operand->type, "++Prev");
		//cout << OperandName << " = " << LResult->s << endl;
		delete Operand;
		return;
	}
	if (LResult->s == "--Prev") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand = LResult;
		LResult = new RESULT;
		LResult->next = Operand->next;
		Shift(Operand->s);
		string OperandName = Operand->s;
		Operand->s = GetValue(Operand->s);
		double sum = GetNum(Operand->s) - 1;
		ostringstream sumstr;
		sumstr << sum;
		ChangeVar(OperandName, sumstr.str());
		LResult->s = sumstr.str();
		LResult->type = TypeCheck(Bool, Operand->type, "--Prev");
		//cout << OperandName << " = " << LResult->s << endl;
		delete Operand;
		return;
	}
	if (LResult->s == "**") {
		LineNum = LResult->LineNum;
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult->next = Operand2->next;
		if (Operand1->s[0] == '$') { Shift(Operand1->s); Operand1->s = GetValue(Operand1->s); }
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		double sum = 1.0;
		for (double i = GetNum(Operand1->s); i > 0; i--) sum *= GetNum(Operand2->s);
		ostringstream sumstr;
		sumstr << sum;
		LResult->s = sumstr.str();
		LResult->type = TypeCheck(Operand2->type, Operand1->type, "**");
		//cout << Operand2->s << "**" << Operand1->s << "=" << sumstr.str() << endl;
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "!F") {
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult = Operand2->next;
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (Operand2->s != "0" && Operand2->s != "false") {
			delete Operand1;
			delete Operand2;
			return;
		}
		ShiftPOLIZ = true;
		ShiftUntilPOLIZ = int(GetNum(Operand1->s));
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "!T") {
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult = Operand2->next;
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (Operand2->s == "0" || Operand2->s == "false") {
			delete Operand1;
			delete Operand2;
			return;
		}
		ShiftPOLIZ = true;
		ShiftUntilPOLIZ = int(GetNum(Operand1->s));
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "!F_switch") {
		DelRESULT();
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		LResult = new RESULT;
		LResult = Operand2->next;
		if (!ShiftSwitch) {
			delete Operand1;
			delete Operand2;
			return;
		}
		if (Operand2->s != "0" && Operand2->s != "false") {
			ShiftSwitch = false;
			delete Operand1;
			delete Operand2;
			return;
		}
		ShiftPOLIZ = true;
		ShiftUntilPOLIZ = int(GetNum(Operand1->s));
		delete Operand1;
		delete Operand2;
		return;
	}
	if (LResult->s == "!WC") {
		DelRESULT();
		RESULT *Operand = LResult;
		LResult = new RESULT;
		LResult = Operand->next;
		ShiftPOLIZ = true;
		ShiftUntilPOLIZ = int(GetNum(Operand->s));
		return;
	}
	if (LResult->s == "switch==") {
		DelRESULT();
		if (ShiftSwitch == false) {
			LResult->s = "true";
			LResult->type = Bool;
			return;
		}
		RESULT *Operand1 = LResult;
		RESULT *Operand2 = LResult->next;
		if (Operand2->s[0] == '$') { Shift(Operand2->s); Operand2->s = GetValue(Operand2->s); }
		if (Operand1->s == Operand2->s) {
			ShiftSwitch = false;
			LResult = Operand2;
			LResult->s = "true";
			LResult->type = Bool;
			delete Operand1;
			return;
		}
		LResult = Operand1;
		LResult->s = "false";
		LResult->type = Bool;
		return;
	}
	if (LResult->s == "default") {
		ShiftSwitch = false;
		DelRESULT();
		DelRESULT();//clear Switch Expression
		return;
	}
	if (LResult->s == ";") {
		DelRESULT();
		return;
	}
	if (LResult->s == "break") {
		DelRESULT();
		ShiftUntilPOLIZ = -1;
		Break = true;
		return;
	}
	if (LResult->s == "continue") {
		DelRESULT();
		ShiftUntilPOLIZ = -1;
		return;
	}
	if (LResult->s == "goto") {
		DelRESULT();
		GoTo = LResult->s;
		DelRESULT();
		return;
	}
	if (LResult->s == "begin" || LResult->s == "end") {
		DelRESULT();
		return;
	}
	if (LResult->s == "echo") {
		DelRESULT();
		if (LResult->s[0] == '$') {
			Shift(LResult->s); 
			LResult->s = GetValue(LResult->s); 
		}
		string str = LResult->s;
		if (str[0] = '"') {
			for (int i = 1; i < str.length() - 1; i++) {
				cout << str[i];
			}
		}
		else {
			cout << LResult->s;
		}
		DelRESULT();
		return;
	}
	cout << "WTF? What is it ->" << LResult->s << "!!!" << endl;
	return;
}
void RunPOLIZ_Special(RESULT *CurrentPosition) {
	RESULT *BegOfExpression = CurrentPosition;
	while (CurrentPosition != nullptr) {
		LResult->s = CurrentPosition->s;
		LResult->type = CurrentPosition->type;
		LResult->LineNum = CurrentPosition->LineNum;
		if (LResult->type == 4) {
			RunOperation();
			CurrentPosition = CurrentPosition->next;
			if (ShiftPOLIZ) CurrentPosition = BegOfExpression;
			while (ShiftPOLIZ && CurrentPosition != nullptr) {
				if (CurrentPosition->ID == ShiftUntilPOLIZ) ShiftPOLIZ = false;
				else CurrentPosition = CurrentPosition->next;
			}
			int depth_in = 0;
			while (ShiftUntilPOLIZ == -1) {
				if (CurrentPosition->s == "begin" || CurrentPosition->next == nullptr) depth_in++;
				if (CurrentPosition->s == "end") {
					depth_in--;
					if (depth_in == -1) ShiftUntilPOLIZ = 0;
				}
				if (ShiftUntilPOLIZ == -1)  CurrentPosition = CurrentPosition->next;
			}
			if (Break && CurrentPosition->next != nullptr) {
				Break = false;
				CurrentPosition = CurrentPosition->next;
				if (CurrentPosition != nullptr) {
					CurrentPosition = CurrentPosition->next;
					CurrentPosition = CurrentPosition->next;
				}
			}
			while (GoTo != "NAN" && CurrentPosition != nullptr) {
				if (CurrentPosition->s == GoTo + ":") GoTo = "NAN";
				else CurrentPosition = CurrentPosition->next;
			}
			RESULT *R = new RESULT;
			R->next = LResult;
			LResult = R;
		}
		else {
			RESULT *R = new RESULT;
			R->next = LResult;
			LResult = R;
			CurrentPosition = CurrentPosition->next;
		}
	}
	return;
}

int position = 1;//adress
void pushPOLIZ(string s, Type type) { //add to POLIZ.txt operand/operation and its adress
	CurrentPOLIZ->ID = position++;
	CurrentPOLIZ->s = s;
	CurrentPOLIZ->type = type;
	CurrentPOLIZ->LineNum = strings;
	CurrentPOLIZ->next = new RESULT;
	CurrentPOLIZ = CurrentPOLIZ->next;

	POLIZ << s << " ";
	POLIZ << type << endl;

	return;
}
//END of POLIZ part
//END OF TID

//LEXIC
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
using namespace std;

enum states {
	Start,
	A,
	B,
	C,
	E,
	F,
	G,
	H,
	I
};
bool IsOperation(char x) {
	ifstream fin("D:\\IT_files\\Compilator\\Files\\Operations.txt");
	string a;
	for (fin >> a; a.length() < 2; fin >> a) {
		if (a[0] == x) {
			fin.close();
			return true;
		}
	}
	fin.close();
	return false;
}
bool IsSign(char x) {
	ifstream fin("D:\\IT_files\\Compilator\\Files\\Signs.txt");
	int a;
	for (a = fin.get(); !fin.eof(); fin.get(), a = fin.get()) {
		if (a == x) {
			fin.close();
			return true;
		}
	}
	fin.close();
	return false;
}
bool IsFinalOperation(string s) {
	ifstream fin("D:\\IT_files\\Compilator\\Files\\Operations.txt");
	string a;
	for (fin >> a; !fin.eof(); fin.get(), fin >> a) {
		if (a == s) {
			fin.close();
			return false;
		}
	}
	if (a == s) {
		fin.close();
		return false;
	}
	fin.close();
	return true;
}
bool IsWord(string s) {
	ifstream fin("D:\\IT_files\\Compilator\\Files\\Words.txt");
	string a;
	for (fin >> a; !fin.eof(); fin.get(), fin >> a) {
		if (a == s) {
			fin.close();
			return false;
		}
	}
	if (a == s) {
		fin.close();
		return false;
	}
	fin.close();
	return true;
}
//END OF LEXIC

//SINTAX
bool GoToPoint = false;
void Program();
void ERROR(string s);
void Get();
void Variable();
void Operators();
void Operator();
void OutPutOperator();
void Expression();
void NEExpression();
void Assignable();
void Sign1();
void Sign2();
void Sign3();
void Sign4();
void Sign5();
void Sign6();
void Sign7();
void Sign8();
void Sign9();
void Sign10();
void Sign11();
void Sign12();
void Sign13();
void Sign14();

void Priority2();
void Priority3();
void Priority4();
void Priority5();
void Priority6();
void Priority7();
void Priority8();
void Priority9();
void Priority10();
void Priority11();
void Priority12();
void Priority13();
void Priority14();
void Priority15();

void Block();
void GotoAndChildren();
void Return();
void Goto();

void ListOfArguments() {
	if (lexem->s == "int" ||
		lexem->s == "double" ||
		lexem->s == "bool" ||
		lexem->s == "string") {
		Type type;
		if (lexem->s == "int") {
			type = Int;
		}
		else if (lexem->s == "double") {
			type = Double;
		}
		else if (lexem->s == "bool") {
			type = Bool;
		}
		else if (lexem->s == "string") {
			type = String;
		}
		Get();
	}
	if (lexem->s != "$") ERROR("$");
	Get();
	if (lexem->id != 2) ERROR("Name");
	Get();
	while (lexem->s == ",") {
		Get();
		if (lexem->s == "int" ||
			lexem->s == "double" ||
			lexem->s == "bool" ||
			lexem->s == "string") {
			Type type;
			if (lexem->s == "int") {
				type = Int;
			}
			else if (lexem->s == "double") {
				type = Double;
			}
			else if (lexem->s == "bool") {
				type = Bool;
			}
			else if (lexem->s == "string") {
				type = String;
			}
			Get();
		}
		if (lexem->s != "$") ERROR("$");
		Get();
		if (lexem->id != 2) ERROR("Name");
		Get();
	}
	return;
}
void FuncDescription() {
	if (lexem->s != "function") ERROR("function");
	Get();
	if (lexem->id != 2) ERROR("Name");
	Get();
	if (lexem->s != "(") ERROR("(");
	Get();
	ListOfArguments();
	if (lexem->s != ")") ERROR(")");
	Get();
	Block();
	return;
}
void Description() {
	FuncDescription();
	return;
}
void For()
{
	Tid *p = L1;
	L1 = L;
	if (lexem->s != "for") ERROR("for");
	Get();
	if (lexem->s != "(") ERROR("for");
	Get();
	//ForConditional_Begining
	Expression();                          //1st Expression
	if (lexem->s != ";") ERROR(";");
	Get();
	RESULT *BegOfFor = CurrentPOLIZ;
	Expression();                        //2nd Expression
	RESULT *P1 = CurrentPOLIZ;//points address A1
	pushPOLIZ("Address_A1", Int);
	pushPOLIZ("!F", Operation);
	if (lexem->s != ";") ERROR(";");
	Get();
	RESULT *P2 = CurrentPOLIZ;//points address A2
	pushPOLIZ("Address_A2", Int);
	pushPOLIZ("!WC", Operation);
	ostringstream ID3rd;
	ID3rd << position;
	Expression();                     //3rd Expression
	ostringstream ID;
	ID << BegOfFor->ID;
	pushPOLIZ(ID.str(), Int);
	pushPOLIZ("!WC", Operation);
	ID.str(""); //cleaning
	ID << position;//because CurrentPOLIZ hasn't been filled yet
	P2->s = ID.str();
	ID.str("");//cleaning
	ID << BegOfFor->ID;
	//ForConditional_End
	if (lexem->s != ")") ERROR(")");
	Get();
	if (lexem->s == "{") {
		Block();
		pushPOLIZ(ID3rd.str(), Int);
		pushPOLIZ("!WC", Operation);
		ID.str("");//cleaning
		ID << position;//because CurrentPOLIZ hasn't been filled yet
		P1->s = ID.str();
		DeleteUntil(L, L1);
		L1 = p;
		return;
	}
	Operator();
	Operators();
	pushPOLIZ(ID3rd.str(), Int);
	pushPOLIZ("!WC", Operation);
	ID.str("");//cleaning
	ID << position;//because CurrentPOLIZ hasn't been filled yet
	P1->s = ID.str();
	if (lexem->s != "endfor") ERROR("endfor");
	Get();
	if (lexem->s != ";") ERROR(";");
	Get();
	DeleteUntil(L, L1);
	L1 = p;
	return;
}
void DoWhile() {
	Tid *p = L1;
	L1 = L;
	if (lexem->s != "do") ERROR("do");
	RESULT *BegOFDoWhile = CurrentPOLIZ;
	Get();
	Block();
	if (lexem->s != "while") ERROR("while");
	Get();
	if (lexem->s != "(") ERROR("(");
	Get();
	NEExpression();

	ostringstream ID;
	ID << BegOFDoWhile->ID;
	pushPOLIZ(ID.str(), Int);
	pushPOLIZ("!T", Operation);
	if (lexem->s != ")") ERROR(")");
	Get();

	DeleteUntil(L, L1);
	L1 = p;
	return;
}
void While() {
	Tid *p = L1;
	L1 = L;
	if (lexem->s == "while") {
		Get();
		if (lexem->s != "(") ERROR("(");
		Get();
		RESULT *WhileBeginning_before = CurrentPOLIZ;
		NEExpression();
		RESULT *P1 = CurrentPOLIZ; //P1 points to Address_A1
		pushPOLIZ("Address_A1", Int);
		pushPOLIZ("!F", Operation);

		if (lexem->s != ")") ERROR(")");
		Get();
		if (lexem->s == "{") {
			ostringstream ID;
			Block();

			ID << WhileBeginning_before->ID;
			pushPOLIZ(ID.str(), Int);
			pushPOLIZ("!WC", Operation);

			ID.str("");//cleaning ID
			ID << position;//because CurrentPOLIZ->ID have not been filled yet
			P1->s = ID.str();
			DeleteUntil(L, L1);
			L1 = p;
			return;
		}
		ostringstream ID;
		Operator();
		Operators();
		ID << position; //beginning of initialisation P1
		P1->s = ID.str(); // end of initialisation
		ID.str(""); //cleaning ID
		ID << WhileBeginning_before->ID;
		pushPOLIZ(ID.str(), Int);
		pushPOLIZ("!WC", Operation);

		if (lexem->s != "endwhile") ERROR("endwhile");
		Get();
		if (lexem->s != ";") ERROR(";");
		Get();
		L1 = p;
		return;
	}
	ERROR("while");
}
void Else1()
{
	Tid *p = L1;
	L1 = L;
	if (lexem->s == "else") {
		Get();
		Operator();
		Operators();
		if (lexem->s != "endif") ERROR("endif");
		Get();
		DeleteUntil(L, L1);
		L1 = p;
		return;
	}
	ERROR("else");
}
void Else() {
	Tid *p = L1;
	L1 = L;
	if (lexem->s != "else")ERROR("else");
	Get();
	Block();
	DeleteUntil(L, L1);
	L1 = p;
	return;
}
void Case() {
	Tid *p = L1;
	Type CaseType;
	L1 = L;
	if (lexem->s == "case") {	
		Get();
		if (lexem->id == 3) {
			//Beginning of determinating a type of CASE constant
			if (lexem->s[0] == '"') CaseType = String;
			else if (lexem->s == "true" || lexem->s == "false") CaseType = Bool;
			else {
				CaseType = Int;
				for (int i = 0; i < lexem->s.length(); i++) {
					if (lexem->s[i] == '.') CaseType = Double;
				}
			}//End of determinating
			pushPOLIZ(lexem->s, CaseType);
			Get();
			pushPOLIZ("switch==", Operation);
			RESULT *P = CurrentPOLIZ;//points Address
			pushPOLIZ("Address", Int);
			pushPOLIZ("!F_switch", Operation);
			if (lexem->s != ":") ERROR(":");
			Get();
			Operators();
			ostringstream ID;
			ID << position;//because CurrentPOLIZ->ID is empty
			P->s = ID.str();
			DeleteUntil(L, L1);
			L1 = p;
			return;
		}
		ERROR("Const");
	} if (lexem->s == "default") {
		Get();
		if (lexem->s != ":") ERROR(":");
		Get();
		Operators();
		DeleteUntil(L, L1);
		L1 = p;
		return;
	}
	ERROR("Case or default");
}
void SelectOperator()
{
	if (lexem->s == "switch") {
		Get();
		if (lexem->s == "(") {
			Get();
			RESULT *BegOfSwitch = CurrentPOLIZ;
			ShiftSwitch = true;
			NEExpression();
			if (lexem->s == ")") {
				Get();
				if (lexem->s == "{") {
					Get();
					Case();
					while (lexem->s == "case" ||
						lexem->s == "default") {
						Case();
					}
					if (lexem->s != "}") ERROR("}");
					Get();
					ShiftSwitch = false;
					DeleteAll(SupportTid);
					SupportTid = new Tid;
					CurrentSupport = SupportTid;
					return;
				}
				do {
					Case();
				} while (lexem->s == "case" || lexem->s == "default");
				if (lexem->s != "endswitch") ERROR("endswitch");
				Get();
				if (lexem->s != ";") ERROR(";");
				Get();
				ShiftSwitch = false;
				return;
			}
			ERROR(")");
		}
		ERROR("(");
	}
	ERROR("switch");
}
void ConditionalOperator() {
	if (lexem->s == "if") {
		Get();
		if (lexem->s != "(") ERROR("(");
		Get();
		RESULT *BegOfIf = CurrentPOLIZ;
		NEExpression();
		RESULT *P1 = CurrentPOLIZ; //P1 points to Address_A1
		pushPOLIZ("Address_A1", Int);
		pushPOLIZ("!F", Operation);

		if (lexem->s != ")") ERROR(")");
		Get();
		if (lexem->s == "{") {
			Tid *p = L1;
			L1 = L;
			Block();
			RESULT *P2 = CurrentPOLIZ; //P2 points to Address_A2
			pushPOLIZ("Address_A2", Int);
			pushPOLIZ("!WC", Operation);

			ostringstream ID; //beginning of initialization P1
			ID << position; //
			P1->s = ID.str(); //end of initialisation P1
			DeleteUntil(L, L1);
			L1 = p;
			Else();
			ID.str(""); //cleaning of ID
			ID << position; //beginning of initialization P2
			P2->s = ID.str(); //end of initialisation P2
			CurrentPOLIZ->ID = position; //because CurrentPOLIZ have not been filled yet
			return;
		}
		Tid *p = L1;
		L1 = L;
		Operator();
		Operators();
		DeleteUntil(L, L1);
		L1 = p;
		
		RESULT *P2 = CurrentPOLIZ; //P2 points to Address_A2
		pushPOLIZ("Address_A2", Int);
		pushPOLIZ("!WC", Operation); //WC <=> without conditional

		ostringstream ID; //beginning of initialization P1
		ID << position; //
		P1->s = ID.str(); //end of initialisation P1

		Else1();
		ID.str(""); //cleaning of ID
		ID << position; //beginning of initialization P2
		P2->s = ID.str(); //end of initialisation
		CurrentPOLIZ->ID = position; //because CurrentPOLIZ have not been filled yet
		return;
	}
	ERROR("if");
}
void CycleOperator() {
	if (lexem->s == "while") {
		While();
		return;
	}
	if (lexem->s == "do") {
		DoWhile();
		return;
	}
	if (lexem->s == "for") {
		For();
		return;
	}
	ERROR("while or do while or for");
}
void SpecOperator() {
	if (lexem->s == "echo") {
		OutPutOperator();
		if (lexem->s != ";")ERROR(";");
		Get();
		return;

	}
	if (lexem->s == "if") {
		ConditionalOperator();
		return;
	}
	if (lexem->s == "while" ||
		lexem->s == "do" ||
		lexem->s == "for") {
		CycleOperator();
		return;
	}
	if (lexem->s == "return" ||
		lexem->s == "break" ||
		lexem->s == "continue" ||
		lexem->s == "goto" ||
		lexem->id == 2) {
		GotoAndChildren();
		return;
	}
	SelectOperator();
}
void Block() {
	if (lexem->s == "{") {
		pushPOLIZ("begin", Operation);
		Get();
		Operator();
		if (lexem->s == ";") Get();
		if (lexem->s == "}") {
			Get();
			return;
		}
		Operators();
		pushPOLIZ("end", Operation);
		if (lexem->s != "}") ERROR("}");
		Get();
		return;
	}
	else {
		Operator();
		Get();
	}
}

void ListOfParameters()
{
	NEExpression();
	while (lexem->s == ",")
	{
		Get();
		NEExpression();
		pushPOLIZ(",", Operation);
	}
}
void FunctionCall()
{
	ListOfParameters();
	if (lexem->s != ")") ERROR(")");
	Get();
}
void OutPutOperator()
{
	if (lexem->s == "echo")
	{
		Get();
		Expression();
		pushPOLIZ("echo", Operation);
	}
}

void Get() {
	do {
		char x;
		lexem->s = "";
		lexem->id = 0;
		Code >> lexem->id;
		char g = Code.get();
		x = Code.peek();
		if (x != '"') {
			for (; x != '\n' && x != EOF; Code.get(), x = Code.peek()) {
				lexem->s += x;
			}
		} else {
			lexem->s += x;
			for (Code.get(), x = Code.peek(); x != '"' && x != EOF; Code.get(), x = Code.peek()) {
				lexem->s += x;
			}
			lexem->s += x;
			Code.get();
		}
		if (lexem->id == 7) {
			strings++;
		}
	} while (lexem->id == 7);
	return;
}
void ERROR(string s) {
	throw(s);
}

void Priority2()
{
	Priority3();
	while (lexem->s == "||")
	{
		Sign2();
		Priority3();
		pushPOLIZ("||", Operation);
	}
}
void Priority3()
{
	Priority4();
	while (lexem->s == "&&")
	{
		Sign3();
		Priority4();
		pushPOLIZ("&&", Operation);
	}
}
void Priority4()
{
	Priority5();
	while (lexem->s == "|")
	{
		Sign4();
		Priority5();
		pushPOLIZ("|", Operation);
	}
}
void Priority5()
{
	Priority6();
	while (lexem->s == "^")
	{
		Sign5();
		Priority6();
		pushPOLIZ("^", Operation);
	}
}
void Priority6()
{
	Priority7();
	while (lexem->s == "&")
	{
		Sign6();
		Priority7();
		pushPOLIZ("&", Operation);
	}
}
void Priority7()
{
	Priority8();
	while (lexem->s == "==" ||
		lexem->s == "!=" ||
		lexem->s == "===" ||
		lexem->s == "!==" ||
		lexem->s == "<=>")
	{
		string OpSign = lexem->s; //sign of operation
		Sign7();
		Priority8();
		pushPOLIZ(OpSign, Operation);
	}
}
void Priority8()
{
	Priority9();
	while (lexem->s == "<" ||
		lexem->s == ">" ||
		lexem->s == "<=" ||
		lexem->s == ">=")
	{
		string OpSign = lexem->s; //sign of operation
		Sign8();
		Priority9();
		pushPOLIZ(OpSign, Operation);
	}
}
void Priority9()
{
	Priority10();
	while (lexem->s == "<<" ||
		lexem->s == ">>")
	{
		string OpSign = lexem->s; //sign of operation
		Sign9();
		Priority10();
		pushPOLIZ(OpSign, Operation);
	}
}
void Priority10()
{
	Priority11();
	while (lexem->s == "+" ||
		lexem->s == "-" ||
		lexem->s == ".")
	{
		string OpSign = lexem->s; //sign of operation
		Sign10();
		Priority11();
		pushPOLIZ(OpSign, Operation);
	}
}
void Priority11()
{
	Priority12();
	while (lexem->s == "*" ||
		lexem->s == "/" ||
		lexem->s == "%")
	{
		string OpSign = lexem->s; //sign of operation
		Sign11();
		Priority12();
		pushPOLIZ(OpSign, Operation);
	}
}
void Priority12()
{
	if (lexem->s == "!") {
		Sign12();
		Priority13();
		pushPOLIZ("!", Operation);
		return;
	}
	Priority13();
	return;
}
void Priority13()
{
	if (lexem->s == "++" ||
		lexem->s == "--")
	{	
		string OpSign = lexem->s; //sign of operation
		Sign13();
		Priority14();
		if (OpSign == "++") OpSign = "++Prev";//That helps us to understand which kind of operator is used (prefix or postfix)
		else OpSign = "--Prev";               //
		pushPOLIZ(OpSign, Operation);
	}
	else
	{
		Priority14();
		if (lexem->s == "++" ||
			lexem->s == "--") {
			string OpSign = lexem->s; //sign of operation
			Sign13();
			pushPOLIZ(OpSign, Operation);
		}
		return;
	}
	return;
}
void Priority14()
{
	Priority15();
	while (lexem->s == "**")
	{
		Sign14();
		Priority15();
		pushPOLIZ("**", Operation);
	}
}
void Priority15(){
	if (lexem->s == "$") {
		Assignable(); return;
	}
	if (lexem->s == "(") {
		Get();
		NEExpression();
		if (lexem->s != ")") ERROR(")");
		Get();
		return;
	}
	if (lexem->id == 3) {
		if (lexem->s[0] == '"') {
			pushPOLIZ(lexem->s, String);
		}
		else if (lexem->s == "true" || lexem->s == "false") {
			pushPOLIZ(lexem->s, Bool);
		}
		else {
			for (int i = 0; i < lexem->s.length(); i++) {
				if (lexem->s[i] == '.') {
					pushPOLIZ(lexem->s, Double);
					Get();
					return;
				}
			}
			pushPOLIZ(lexem->s, Int);
		}
		Get(); return;
	}
	if (lexem->id == 2) {
		string Name = lexem->s;
		Get();
		if (lexem->s == "(") {
			Get();
			FunctionCall();
			return;
		}
		if (lexem->s == ":") {
			pushPOLIZ(Name + ":", String);
			GoToPoint = true;
			Get();
			return;
		}
		ERROR("( or :");
	}
	return;
}

void Program()
{
	if (lexem->s != "<") ERROR("<");
	Get();
	if (lexem->s != "?") ERROR("?");
	Get();
	if (lexem->s != "php") ERROR("php");
	pushPOLIZ("begin", Operation);
	Get();
	Operators();
	pushPOLIZ("end", Operation);
	if (lexem->s != ">" || Code.eof()) ERROR(">");
	return;
}
void Variable() {
	if (lexem->s == "$") {
		Get();
		if (lexem->id == 2) {
			CheckID(lexem->s, L);
			pushPOLIZ("$" + lexem->s, GetType(lexem->s));
			Get();
			return;
		}
		ERROR("Name");
	}
	ERROR("$");
}

void Operators() {
	while (true) {
		if (lexem->s == "function") {
			FuncDescription();
		}
		else
			if (lexem->s == "switch" ||
				lexem->s == "if" ||
				lexem->s == "while" ||
				lexem->s == "do" ||
				lexem->s == "for" ||
				lexem->s == "return" ||
				lexem->s == "break" ||
				lexem->s == "continue" ||
				lexem->s == "echo" ||
				lexem->s == "goto"
				) {
				SpecOperator();
			}
			else
				if (lexem->s == "$" ||
					lexem->s == "(" ||
					lexem->s == "++" ||
					lexem->s == "--" ||
					lexem->id == 2 ||
					lexem->id == 3) {
					Expression();
					if (lexem->s != ";" && !GoToPoint) ERROR(";");
					if (GoToPoint) GoToPoint = false;
					else pushPOLIZ(";", Operation);
				}
				else
					if (lexem->s == "int" ||
						lexem->s == "double" ||
						lexem->s == "bool" ||
						lexem->s == "string") {
						Type type;
						if (lexem->s == "int") {
							type = Int;
						} 
						else if (lexem->s == "double") {
							type = Double;
						}
						else if (lexem->s == "bool") {
							type = Bool;
						}
						else if (lexem->s == "string") {
							type = String;
						}
						do {
							bool is_coma = false;
							if (lexem->s == ",") is_coma = true;
							Get();
							if (lexem->s != "$") ERROR("$");
							Get();
							if (lexem->id != 2) ERROR("name");
							AddID(lexem->s, "", type, L);
							pushPOLIZ("$" + lexem->s, type);
							Get();
							if (lexem->s == "=") {
								Get();
								NEExpression();
								pushPOLIZ("=", Operation);
							}
							if(is_coma)  pushPOLIZ(",", Operation);
						} while (lexem->s == ",");
						if (lexem->s != ";") ERROR(";");
						pushPOLIZ(";", Operation);
						Get();
					}
					else
						if (lexem->s == ";") {
							Get();
						}
						else return;
	}
}
void Operator() {
	if (lexem->s == "function") {
		FuncDescription();
		return;
	}
	if (lexem->s == "switch" ||
		lexem->s == "if" ||
		lexem->s == "while" ||
		lexem->s == "do" ||
		lexem->s == "for" ||
		lexem->s == "return" ||
		lexem->s == "break" ||
		lexem->s == "continue" ||
		lexem->s == "goto" ||
		lexem->s == "echo" ||
		lexem->id == 2) {
		SpecOperator();
		return;
	}
	if (lexem->s == "$" ||
		lexem->s == "--" ||
		lexem->s == "(" ||
		lexem->s == "++" ||
		lexem->id == 2 ||
		lexem->id == 3) {
			Expression();
			if (lexem->s != ";" && !GoToPoint) ERROR(";");
			if (GoToPoint) GoToPoint = false;
			pushPOLIZ(";", Operation);
		return;
	}
	if (lexem->s == ";") {
		Get();
		pushPOLIZ(";", Operation);
		return;
	}
	if (lexem->s == "int" ||
		lexem->s == "double" ||
		lexem->s == "bool" ||
		lexem->s == "string") {
		Type type;
		if (lexem->s == "int") {
			type = Int;
		}
		else if (lexem->s == "double") {
			type = Double;
		}
		else if (lexem->s == "bool") {
			type = Bool;
		}
		else if (lexem->s == "string") {
			type = String;
		}
		do {
			bool is_coma = false;
			if (lexem->s == ",") is_coma = true;
			Get();
			if (lexem->s != "$") ERROR("$");
			Get();
			if (lexem->id != 2) ERROR("name");
			AddID(lexem->s, "", type, L);
			pushPOLIZ("$" + lexem->s, type);
			Get();
			if (lexem->s == "=") {
				Get();
				NEExpression();
				pushPOLIZ("=", Operation);
			}
			if(is_coma) pushPOLIZ(",", Operation);
		} while (lexem->s == ",");
		if (lexem->s != ";") ERROR(";");
		Get();
		return;
	}
	ERROR("Operator");
}
void Expression() {
	if (lexem->s == "$" ||
		lexem->s == "++" ||
		lexem->s == "(" ||
		lexem->s == "--" ||
		lexem->id == 2 ||
		lexem->id == 3) {
		NEExpression();
	}
	return;
}
void NEExpression()
{
	Priority2();
	if (lexem->s == "=" ||
		lexem->s == "+=" ||
		lexem->s == "-=" ||
		lexem->s == "*=" ||
		lexem->s == "**=" ||
		lexem->s == "/=" ||
		lexem->s == ".=" ||
		lexem->s == "%=" ||
		lexem->s == "&=" ||
		lexem->s == "|=" ||
		lexem->s == "^=" ||
		lexem->s == "<<=" ||
		lexem->s == ">>=") {
		string OpSign = lexem->s;
		Sign1();
		NEExpression();
		pushPOLIZ(OpSign, Operation);
	}
	return;
}
void Assignable()
{
	Variable();
	return;
}

void Sign1() {
	if (lexem->s == "=" ||
		lexem->s == "-=" ||
		lexem->s == "*=" ||
		lexem->s == "**=" ||
		lexem->s == "/=" ||
		lexem->s == ".=" ||
		lexem->s == "+=" ||
		lexem->s == "%=" ||
		lexem->s == "&=" ||
		lexem->s == "|=" ||
		lexem->s == "^=" ||
		lexem->s == "<<=" ||
		lexem->s == ">>=") {
		Get();
		return;
	}
	ERROR("= or -= or *= or **= or += or /= or .= or %= or &= or |= or ^= or <<= or >>=");
}
void Sign2() {
	if (lexem->s == "||") {
		Get();
		return;
	}
	ERROR("||");
}
void Sign3() {
	if (lexem->s == "&&") {
		Get();
		return;
	}
	ERROR("&&");
}
void Sign4() {
	if (lexem->s == "|") {
		Get();
		return;
	}
	ERROR("|");
}
void Sign5() {
	if (lexem->s == "^") {
		Get();
		return;
	}
	ERROR("^");
}
void Sign6() {
	if (lexem->s == "&") {
		Get();
		return;
	}
	ERROR("&");
}
void Sign7() {
	if (lexem->s == "==" ||
		lexem->s == "!=" ||
		lexem->s == "===" ||
		lexem->s == "!==" ||
		lexem->s == "<=>") {
		Get();
		return;
	}
	ERROR("== or != or === or !== or <=>");
}
void Sign8() {
	if (lexem->s == "<" ||
		lexem->s == ">" ||
		lexem->s == "<=" ||
		lexem->s == ">=") {
		Get();
		return;
	}
	ERROR("< or > or <= or >=");
}
void Sign9() {
	if (lexem->s == "<<" ||
		lexem->s == ">>") {
		Get();
		return;
	}
	ERROR("<< or >>");
}
void Sign10() {
	if (lexem->s == "+" ||
		lexem->s == "-" ||
		lexem->s == ".") {
		Get();
		return;
	}
	ERROR("+ or - or .");
}
void Sign11() {
	if (lexem->s == "*" ||
		lexem->s == "/" ||
		lexem->s == "%") {
		Get();
		return;
	}
	ERROR("* or / or %");
}
void Sign12() {
	if (lexem->s == "!") {
		Get();
		return;
	}
	ERROR("!");
}
void Sign13() {
	if (lexem->s == "++" ||
		lexem->s == "--") {
		Get();
		return;
	}
	ERROR("++ or --");
}
void Sign14() {
	if (lexem->s == "**") {
		Get();
		return;
	}
	ERROR("**");
}

void GotoAndChildren() {
	if (lexem->s == "return") { Return(); if (lexem->s != ";") ERROR(";"); Get(); return; }
	if (lexem->s == "break") { pushPOLIZ("break", Operation); Get(); if (lexem->s != ";") ERROR(";"); Get(); return; }
	if (lexem->s == "continue") { pushPOLIZ("continue", Operation); Get(); if (lexem->s != ";") ERROR(";"); Get(); return; }
	if (lexem->s == "goto") { Goto(); if (lexem->s != ";") ERROR(";"); Get(); return; }
	ERROR("return or break or continue or goto");
}
void Return() {
	if (lexem->s == "return") {
		Get();
		Expression();
		return;
	}
}
void Goto() {
	if (lexem->s == "goto") {
		Get();
		if (lexem->id == 2) {
			pushPOLIZ(lexem->s, String);
			pushPOLIZ("goto", Operation);
			Get();
			return;
		}
		ERROR("name in goto");
	}
}
//END OF SINTAX

void OutTID() {
	ofstream fout("D:\\IT_files\\Compilator\\Files\\TID.txt");
	for (Tid *p = L; p != nullptr; p = p->next) {
		fout << p->type << " | " << p->value << " | " << p->name<<"\n";
	}
	fout.close();
}
int main() {
	ifstream fin("D:\\IT_files\\Compilator\\Files\\Program.txt");
	ofstream fout("D:\\IT_files\\Compilator\\Files\\Code.txt");
	char x;// , a;
	string s;
	int str = 0;
	states state = Start;
	for (;;) {
		switch (state) {
		case Start:
			x = fin.get();
			if (x == EOF) {
				str++;
				fout << "7 " << str << "\n";
				goto sintax;
			}
			else if (x == ' ' || x == '	') {
				state = Start;
			}
			else if (x == '\n') {
				state = Start;
				str++;
				fout << "7 " << str << "\n";
			}
			else if (x == '/') {
				state = G;
			}
			else if (isalpha(x) || x == '_') {
				state = C;
			}
			else if (isdigit(x)) {
				state = B;
			}
			else if (x == '"') {
				state = E;
			}
			else if (IsOperation(x)) {
				state = A;
			}
			else if (IsSign(x)) {
				state = Start;
				fout << "5 " << x << "\n";
			}
			else {
				state = Start;
				fout << "6 " << x << "\n";
			}
			break;
		case A:
			fout << "4 ";
			s = "";
			s += x;
			for (x = fin.peek(), s += x; !IsFinalOperation(s);) {
				fin.get();
				x = fin.peek();
				s += x;
			}
			for (int i = 0; i < s.length() - 1; fout << s[i], i++);
			fout << "\n";
			state = Start;
			break;
		case B:
			fout << "3 " << x;
			for (x = fin.peek(); isdigit(x); fout << x, fin.get(), x = fin.peek());
			if (x == '.') {
				fin.get();
				fout << x;
				x = fin.peek();
				for (x = fin.peek(); isdigit(x); fout << x, fin.get(), x = fin.peek());
			}
			fout << "\n";
			state = Start;
			if (isalpha(x)) {
				cout << "Lexical ERROR in string " << str + 1 << "\n";
				system("pause");
				return 0;
			}
			break;
		case C:
			s = "";
			s += x;
			state = Start;
			for (x = fin.peek(); isalnum(x) || x == '_'; s += x, fin.get(), x = fin.peek());
			if (IsWord(s)) {
				if (s == "true" || s == "false") {
					fout << "3 ";
				}
				else {
					fout << "2 ";
				}
			}
			else {
				fout << "1 ";
			}
			fout << s << "\n";
			break;
		case E:
			fout << "3 " << x;
			for (x = fin.get(); x != '"'; fout << x, x = fin.get());
			fout << x << "\n";
			state = Start;
			break;
		case G:
			x = fin.peek();
			if (x == '/') {
				state = H;
				fin.get();
			}
			else if (x == '*') {
				fin.get();
				state = I;
			}
			else if (x == '=') {
				fin.get();
				fout << "4 /=\n";
				state = Start;
			}
			else {
				fout << "4 /\n";
				state = Start;
			}
			break;
		case H:
			for (x = fin.get(); x != '\n' && x != EOF; x = fin.get());
			state = Start;
			str++;
			fout << "7 " << str << "\n";
			break;
		case I:
			for (x = fin.get(); ;) {
				if (x == '\n') {
					str++;
					fout << "7 " << str << "\n";
				}
				if (x == '*') {
					x = fin.get();
					if (x == '/') {
						break;
					}
				}
				else {
					x = fin.get();
				}
			}
			state = Start;
			break;
		default:
			break;
		}
	}

sintax:fout.close();
	fin.close();
	Get();
	L = nullptr;

	//Beginning of SyntaxAnalyser
	try {
		Program();
		POLIZ.close();
		RunPOLIZ_Special(MainPOLIZ);
	}
	catch (string s) {
		cout << "Error in string " << strings << "\nExpect: " << s << "\nGet: " << lexem->s << endl;
		DeleteAll(L);
		delete lexem;
		Code.close();
		system("pause");
		return 0;
	}
	catch (int x) {
		switch (x)
		{
		case 1: {
			cout << "Error in string " << strings << endl << lexem->s << " is initialized saveral times" << endl;
			break;
		}
		case 2: {
			cout << "Error in string " << strings << endl << lexem->s << " isn't initialized" << endl;
			break;
		}
		case 3: {
			cout << "Error in string " << LineNum << endl << "TYPE ERROR" << endl;
			break;
		}
		case 0: {
			cout << "Error in string" << LineNum << endl << "Devision by zero" << endl;
		}
		}	
		delete lexem;
		DeleteAll(L);
		Code.close();
		system("pause");
		return 0;
	}
	delete lexem;
	ClearRESULT();
	DeleteAll(L);
	Code.close();
	system("pause");
	return 0;
}