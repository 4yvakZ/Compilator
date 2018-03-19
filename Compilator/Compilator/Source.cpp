#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <stdio.h>

using namespace std;


static ifstream Code("D:\\IT_files\\Compilator\\Files\\Code.txt");
struct Lex {
	int id;
	string s;
};
static Lex *lexem = new Lex;
static int strings = 1;
//TID
enum Type {
	Int,
	Double,
	//Char,
	Bool,
	String,
	Operation
};
struct Tid {
	Type type;
	Tid *next;
	string name;
	string value;
};

static Tid *L = new Tid;
static Tid *L1 = nullptr;

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

//END OF TID
//FUNCTIN TID
/*struct FunctionTid {
	Type type;
	Tid *L;
	FunctionTid *next;
};
void AddIDF(string name, Type type, FunctionTid *&LF) {
	FunctionTid *p;
	for (p = LF; p != nullptr && p != LF1; p = p->next) {
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
}

Type CheckID(string name, Tid *&L) {
	for (Tid *p = L; p != nullptr; p = p->next) {
		if (p->name == name) {
			return p->type;
		}
	}
	throw(2);
}
//END OF FUNCTION TID
*///STEK
struct STEK{
	Type type;
	string op = "";
	STEK *next;
};
static STEK *stek = new STEK;
void push1(STEK *&stek, Type type) {
	STEK *p = new STEK;
	p->next = stek;
	stek = p;
	p->type = type;
}
void push2(STEK *&stek, string s) {
	STEK *p = new STEK;
	p->next = stek;
	stek = p;
	p->op = s;
	p->type = Operation;
}
STEK *pop(STEK *&stek) {
	if (stek != nullptr) {
		STEK *q;
		q = stek;
		stek = stek->next;
		q->next = nullptr;
		return q;
	}
}
void check_op(STEK *&stek) {
	Type type1, type2;
	string op;
	STEK *p;
	p = pop(stek);
	type2 = p->type;
	delete p;
	p = pop(stek);
	op = p->op;
	delete p;
	p = pop(stek); 
	type1 = p->type;
	delete p;
	if (op == "**" || op == "**=") {
		if ((type1 == Int || type1 == Double) && type2 == Int) {
			push1(stek, type1);
			return;
		}
	}
	else if (op == "*" || op == "+" || op == "-"||op == "/") {
		if ((type1 == Int || type1 == Double) && (type2 == Int || type2 == Double)) {
			if (type1 == Double || type2 == Double) {
				push1(stek, Double);
				return;
			} else {
				push1(stek, Int);
				return;
			}
		}
	}
	else if (op == "%" || op == "<<" || op == ">>" || op == "&" || op == "^" || op == "|" ||
		op == "%=" || op == ">>=" || op == "<<=" || op == "&=" || op == "^=" || op == "|=") {
		if (type1 == Int && type2 == Int) {
			push1(stek, Int);
			return;
		}
	}
	else if (op == "."||op==".=") {
		if (type1 == String && type2 == String) {
			push1(stek, String);
			return;
		}
	}
	else if (op == "<" || op == ">" || op == ">=" || op == "<=") {
		if ((type1 == Int || type1 == Double) && (type2 == Int || type2 == Double)) {
			push1(stek, Bool);
			return;
		}
	}
	else if (op == "&&" || op == "||") {
		if (type1 == Bool && type2 == Bool) {
			push1(stek, Bool);
			return;
		}
	}
	else if (op == "=") {
		if (type1 == type2) {
			push1(stek, type2);
			return;
		}
		else if ((type1 == Int || type1 == Double) && (type2 == Int || type2 == Double)) {
			push1(stek, type2);
			return;
		}
	}
	else if (op == "-=" || op == "+=" || op == "*=" || op == "/=") {
		if ((type1 == Int || type1 == Double) && (type2 == Int || type2 == Double)) {
			push1(stek, type1);
			return;
		}
	}
	else if (op == "==" || op == "===" || op == "<=>" || op == "<>" || op == "!="
		|| op == "!==") {
		if (type1 == type2) {
			push1(stek, Bool);
			return;
		}
		else if ((type1 == Int || type1 == Double) && (type2 == Int || type2 == Double)) {
			push1(stek, Bool);
			return;
		}
	}
	throw (3);
}
void check_not(STEK *&stek) {
	Type type;
	string op;
	STEK *p;
	p = pop(stek);
	if (p->op == "++" || "--") {
		delete p;
		p = pop(stek);
		if (p->type == Int) {
			delete p;
			push1(stek, Int);
			return;
		}
	}
	else{
		type = p->type;
		delete p;
		p = pop(stek);
		op = p->op;
		delete p;
		if (op == "++" || op == "--") {
			if (type == Int) {
				push1(stek, Int);
				return;
			}
		}
		else if (op == "!") {
			if (type == Bool) {
				push1(stek, Bool);
				return;
			}
		}
	}

	throw (3);
}
void delete_elem(STEK *&stek) {
	if (stek == nullptr)return;
	STEK *p = stek;
	stek = stek->next;
	p->next = nullptr;
	delete p;
}
void delete_stek(STEK *&stek) {
	for (; stek != nullptr; delete_elem(stek));
}
//END OF STEK

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
	if (lexem->s != "$") ERROR("$");
	Get();
	if (lexem->id != 2) ERROR("Name");
	Get();
	while (lexem->s == ",") {
		Get();
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
void ForCondition() {
	Expression();
	if (stek != nullptr)
		if (stek->next != nullptr)throw(3);
	delete_stek(stek);
	if (lexem->s != ";") ERROR(";");
	Get();
	Expression();
	if (stek != nullptr)
		if (stek->next != nullptr || stek->type != Bool)throw(3);
	delete_stek(stek);
	if (lexem->s != ";") ERROR(";");
	Get();
	Expression();
	if (stek != nullptr)
		if (stek->next != nullptr)throw(3);
	delete_stek(stek);
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
	ForCondition();
	if (lexem->s != ")") ERROR(")");
	Get();
	if (lexem->s == "{") {
		Block();
		DeleteUntil(L, L1); 
		L1 = p;
		return;
	}
	Operator();
	Operators();
	if (lexem->s != "endfor") ERROR("endfor");
	Get();
	if (lexem->s != ";") ERROR(";");
	Get();
	DeleteUntil(L, L1);
	L1 = p;
}
void DoWhile() {
	Tid *p = L1;
	L1 = L;
	if (lexem->s != "do") ERROR("do");
	Get();
	Block();
	if (lexem->s != "while") ERROR("while");
	Get();
	if (lexem->s != "(") ERROR("(");
	Get();
	NEExpression();
	if (stek != nullptr)
	if (stek->next != nullptr || stek->type != Bool)throw(3);
	delete_stek(stek);
	if (lexem->s != ")") ERROR(")");
	Get();
	DeleteUntil(L, L1);
	L1 = p;
}
void While() {
	Tid *p = L1;
	L1 = L;
	if (lexem->s == "while") {
		Get();
		if (lexem->s != "(") ERROR("(");
		Get();
		NEExpression();
		if (stek != nullptr)
			if (stek->next != nullptr || stek->type != Bool)throw(3);
		delete_stek(stek);
		if (lexem->s != ")") ERROR(")");
		Get();
		if (lexem->s == "{") {
			Block();
			DeleteUntil(L, L1);
			L1 = p;
			return;
		}
		Operator();
		Operators();
		if (lexem->s != "endwhile") ERROR("endwhile");
		Get();
		if (lexem->s != ";") ERROR(";");
		Get();
		DeleteUntil(L, L1);
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
}
void Case(Type type1) {
	Tid *p = L1;
	Type type2;
	L1 = L;
	if (lexem->s == "case") {	
		Get();
		if (lexem->id == 3) {
			if (lexem->s[0] == '"') {
				type2 = String;
			}
			else if (lexem->s == "true" || lexem->s == "false") {
				type2 = Bool;
			}
			else {
				for (int i = 0; i < lexem->s.length(); i++) {
					if (lexem->s[i] == '.') {
						type2 = Double;
						Get();
						return;
					}
				}
				type2 = Int;
			}
			if (type1 == type2 || type1 == Double && type2 == Int) {
				Get();
				if (lexem->s != ":") ERROR(":");
				Get();
				Operators();
				DeleteUntil(L, L1);
				L1 = p;
				return;
			}
			else {
				throw(3);
			}
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
			NEExpression();
			if (stek != nullptr)
			if (stek->next != nullptr)throw(3);
			Type type = stek->type;
			delete_stek(stek);
			if (lexem->s == ")") {
				Get();
				if (lexem->s == "{") {
					Get();
					Case(type);
					while (lexem->s == "case" ||
						lexem->s == "default") {
						Case(type);
					}
					if (lexem->s != "}") ERROR("}");
					Get();
					return;
				}
				do {
					Case(type);
				} while (lexem->s == "case" ||
					lexem->s == "default");
				if (lexem->s != "endswitch") ERROR("endswitch");
				Get();
				if (lexem->s != ";") ERROR(";");
				Get();
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
		NEExpression();
		if (stek != nullptr)
		if (stek->next != nullptr || stek->type != Bool)throw(3);
		delete_stek(stek);
		if (lexem->s != ")") ERROR(")");
		Get();
		if (lexem->s == "{") {
			Block();
			Else();
			return;
		}
		Operator();
		Operators();
		Else1();
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
		Get();
		Operator();
		Operators();
		if (lexem->s != "}") ERROR("}");
		Get();
	}
	else {
		Operator();
		Get();
	}
}

void ListOfParameters()
{
	NEExpression();
	if (stek != nullptr)
	if (stek->next != nullptr)throw(3);
	delete_stek(stek);
	while (lexem->s == ",")
	{
		Get();
		NEExpression();
		if (stek != nullptr)
		if (stek->next != nullptr)throw(3);
		delete_stek(stek);
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
		if (stek != nullptr)
		if (stek->next != nullptr)throw(3);
		delete_stek(stek);
	}
}

void Get() {
	do {
		char x;
		lexem->s = "";
		lexem->id = 0;
		Code >> lexem->id;
		Code.get();
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
		if (lexem->id == 7)strings++;
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
		check_op(stek);
	}
}
void Priority3()
{
	Priority4();
	while (lexem->s == "&&")
	{
		Sign3();
		Priority4();
		check_op(stek);
	}
}
void Priority4()
{
	Priority5();
	while (lexem->s == "|")
	{
		Sign4();
		Priority5();
		check_op(stek);
	}
}
void Priority5()
{
	Priority6();
	while (lexem->s == "^")
	{
		Sign5();
		Priority6();
		check_op(stek);
	}
}
void Priority6()
{
	Priority7();
	while (lexem->s == "&")
	{
		Sign6();
		Priority7();
		check_op(stek);
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
		Sign7();
		Priority8();
		check_op(stek);
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
		Sign8();
		Priority9();
		check_op(stek);
	}
}
void Priority9()
{
	Priority10();
	while (lexem->s == "<<" ||
		lexem->s == ">>")
	{
		Sign9();
		Priority10();
		check_op(stek);
	}
}
void Priority10()
{
	Priority11();
	while (lexem->s == "+" ||
		lexem->s == "-" ||
		lexem->s == ".")
	{
		Sign10();
		Priority11();
		check_op(stek);
	}
}
void Priority11()
{
	Priority12();
	while (lexem->s == "*" ||
		lexem->s == "/" ||
		lexem->s == "%")
	{
		Sign11();
		Priority12();
		check_op(stek);
	}
}
void Priority12()
{
	if (lexem->s == "!") {
		Sign12();
		Priority13();
		check_not(stek);
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
		Sign13();
		Priority14();
		check_not(stek);
	}
	else
	{
		Priority14();
		if (lexem->s == "++" ||
			lexem->s == "--") {
			Sign13();
			check_not(stek);
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
		check_op(stek);
	}
}
void Priority15()
{
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
			push1(stek, String);
		}
		else if (lexem->s == "true" || lexem->s == "false") {
			push1(stek, Bool);
		}
		else {
			for (int i = 0; i < lexem->s.length(); i++) {
				if (lexem->s[i] == '.') {
					push1(stek, Double);
					Get();
					return;
				}
			}
			push1(stek, Int);
		}
		Get(); return;
	}
	if (lexem->id == 2) {
		Get();
		if (lexem->s == "(") {
			Get();
			FunctionCall();
			return;
		}
		if (lexem->s == ":") {
			Get();
			throw(1.0);
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
	Get();
	Operators();
	if (lexem->s != ">"|| Code.eof()) ERROR(">");
	return;
}

void Variable() {
	if (lexem->s == "$") {
		Get();
		if (lexem->id == 2) {
			push1(stek, CheckID(lexem->s, L));
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
					try {
						Expression();
						if (stek != nullptr)
						if (stek->next != nullptr)throw(3);
						delete_stek(stek);
						if (lexem->s != ";") ERROR(";");
					}
					catch (double a) {};
				}
				else
					if (lexem->s == "int" ||
						lexem->s == "double" ||
						lexem->s == "bool" ||
						//lexem->s == "char"||
						lexem->s == "string") {
						Type type;
						if (lexem->s == "int") {
							type = Int;
							push1(stek, Int);
						} 
						else if (lexem->s == "double") {
							type = Double;
							push1(stek, Double);
						}
						else if (lexem->s == "bool") {
							type = Bool;
							push1(stek, Bool);
						}
						else if (lexem->s == "string") {
							type = String;
							push1(stek, String);
						}
						/*else {
							type = Char;
						}*/
						do {
							Get();
							if (lexem->s != "$") ERROR("$");
							Get();
							if (lexem->id != 2) ERROR("name");
							AddID(lexem->s, "", type, L);
							Get();
							if (lexem->s == "=") {
								NEExpression();
							}
						} while (lexem->s == ",");
						delete_elem(stek);
						if (lexem->s != ";") ERROR(";");
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
		try {
			Expression();
			if (stek != nullptr)
			if (stek->next != nullptr)throw(3);
			delete_stek(stek);
			if (lexem->s != ";") ERROR(";");
		}
		catch (int a) {};
		return;
	}
	if (lexem->s == ";") {
		Get();
		return;
	}
	if (lexem->s == "int" ||
		lexem->s == "double" ||
		lexem->s == "bool" ||
		//lexem->s == "char" ||
		lexem->s == "string") {
		Type type;
		if (lexem->s == "int") {
			type = Int;
			push1(stek, Int);
		}
		else if (lexem->s == "double") {
			type = Double;
			push1(stek, Double);
		}
		else if (lexem->s == "bool") {
			type = Bool;
			push1(stek, Bool);
		}
		else if (lexem->s == "string") {
			type = String;
			push1(stek, String);
		}
		/*else {
		type = Char;
		}*/
		do {
			Get();
			if (lexem->s != "$") ERROR("$");
			Get();
			if (lexem->id != 2) ERROR("name");
			AddID(lexem->s, "", type, L);
			Get();
			if (lexem->s == "=") {
				push2(stek, "=");
				NEExpression();
				check_op(stek);
				if (stek != nullptr)
				if (stek->next != nullptr)throw(3);
				delete_stek(stek);
			}
		} while (lexem->s == ",");
		delete_elem(stek);
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
		Sign1();
		NEExpression();
		check_op(stek);
	}
	return;
}
void Assignable()
{
	Variable();
	/*if (lexem->s == "[") {
		NEExpression();
		if (lexem->s != "]") ERROR("]");
	}*/
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
		push2(stek, lexem->s);
		Get();
		return;
	}
	ERROR("= or -= or *= or **= or += or /= or .= or %= or &= or |= or ^= or <<= or >>=");
}
void Sign2() {
	if (lexem->s == "||") {
		push2(stek, lexem->s);
		Get();
		return;
	}
	ERROR("||");
}
void Sign3() {
	if (lexem->s == "&&") {
		push2(stek, lexem->s);
		Get();
		return;
	}
	ERROR("&&");
}
void Sign4() {
	if (lexem->s == "|") {
		push2(stek, lexem->s);
		Get();
		return;
	}
	ERROR("|");
}
void Sign5() {
	if (lexem->s == "^") {
		push2(stek, lexem->s);
		Get();
		return;
	}
	ERROR("^");
}
void Sign6() {
	if (lexem->s == "&") {
		push2(stek, lexem->s);
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
		push2(stek, lexem->s);
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
		push2(stek, lexem->s);
		Get();
		return;
	}
	ERROR("< or > or <= or >=");
}
void Sign9() {
	if (lexem->s == "<<" ||
		lexem->s == ">>") {
		push2(stek, lexem->s);
		Get();
		return;
	}
	ERROR("<< or >>");
}
void Sign10() {
	if (lexem->s == "+" ||
		lexem->s == "-" ||
		lexem->s == ".") {
		push2(stek, lexem->s);
		Get();
		return;
	}
	ERROR("+ or - or .");
}
void Sign11() {
	if (lexem->s == "*" ||
		lexem->s == "/" ||
		lexem->s == "%") {
		push2(stek, lexem->s);
		Get();
		return;
	}
	ERROR("* or / or %");
}
void Sign12() {
	if (lexem->s == "!") {
		push2(stek, lexem->s);
		Get();
		return;
	}
	ERROR("!");
}
void Sign13() {
	if (lexem->s == "++" ||
		lexem->s == "--") {
		push2(stek, lexem->s);
		Get();
		return;
	}
	ERROR("++ or --");
}
void Sign14() {
	if (lexem->s == "**") {
		push2(stek, lexem->s);
		Get();
		return;
	}
	ERROR("**");
}

void GotoAndChildren() {
	if (lexem->s == "return") { Return(); if (lexem->s != ";") ERROR(";"); Get(); return; }
	if (lexem->s == "break") { Get(); if (lexem->s != ";") ERROR(";"); Get(); return; }
	if (lexem->s == "continue") { Get(); if (lexem->s != ";") ERROR(";"); Get(); return; }
	if (lexem->s == "goto") { Goto(); if (lexem->s != ";") ERROR(";"); Get(); return; }
	ERROR("return or break or continue or goto");
}
void Return() {
	if (lexem->s == "return") {
		Get();
		Expression();
		if (stek != nullptr)
		if (stek->next != nullptr)throw(3);
		delete_stek(stek);
		return;
	}
}
void Goto() {
	if (lexem->s == "goto") {
		Get();
		if (lexem->id == 2) {
			Get();
			return;
		}
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
/*void out_stek() {
	ofstream fout("D:\\IT_files\\Compilator\\Files\\TID.txt");
	for (STEK *p = stek; p != nullptr; p = p->next) {
		switch (p->type)
		{
		case Int:
			fout << "int";
			break;
		case Double:
			fout << "double";
			break;
		case Bool:
			fout << "bool";
			break;
		case String:
			fout << "string";
			break;
		}
		fout<< "\n";
	}
	fout.close();
}*/
int main() {
	ifstream fin("D:\\IT_files\\Compilator\\Files\\Program.txt");
	ofstream fout("D:\\IT_files\\Compilator\\Files\\Code.txt");
	char x, a;
	string s;
	int strings = 0;
	states state = Start;
	stek = nullptr;
	for (;;) {
		switch (state) {
		case Start:
			x = fin.get();
			if (x == EOF) {
				strings++;
				fout << "7 " << strings << "\n";
				goto sintax;
			}
			else if (x == ' ' || x == '	') {
				state = Start;
			}
			else if (x == '\n') {
				state = Start;
				strings++;
				fout << "7 " << strings << "\n";
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
				cout << "Lexical ERROR in string " << strings + 1 << "\n";
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
			strings++;
			fout << "7 " << strings << "\n";
			break;
		case I:
			for (x = fin.get(); ;) {
				if (x == '\n') {
					strings++;
					fout << "7 " << strings << "\n";
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
	try {
		Program();
	}
	catch (string s) {
		cout << "Error in string " << strings << "\nExpect: " << s << "\nGet: " << lexem->s << "\n";
		//OutTID();
		delete_stek(stek);
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
			cout << "Error in string " << strings << endl << lexem->s << " is initialized saveral times";
			break;
		}
		case 2: {
			cout << "Error in string " << strings << endl << lexem->s << " isn't initialized";
			break;
		}
		case 3: {
			cout << "TYPE ERROR";
			break;
		}
		}	
		//OutTID();
		delete lexem;
		delete_stek(stek);
		DeleteAll(L);
		Code.close();
		system("pause");
		return 0;
	}
	delete lexem;
	//OutTID();
	delete_stek(stek);
	DeleteAll(L);
	Code.close();
	system("pause");
	return 0;
}