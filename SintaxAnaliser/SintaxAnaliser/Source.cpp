#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <stdio.h>
using namespace std;

static ifstream Code("D:\\IT_files\\Compilator\\Files\\Code.txt");
struct Lex{
	int id;
	string s;
};
static Lex *lexem = new Lex;
static int strings = 1;
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
void Element();
void GotoAndChildren();
void Return();
void Goto();
void Label();

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
void ForCondition(){
	Expression();
	if (lexem->s != ";") ERROR(";");
	Get();
	Expression();
	if (lexem->s != ";") ERROR(";");
	Get();
	Expression();
	return;
}
void For()
{
	if (lexem->s != "for") ERROR("for");
	Get();
	if (lexem->s != "(") ERROR("for");
	Get();
	ForCondition();
	if (lexem->s != ")") ERROR(")");
	Get();
	if (lexem->s == "{") {
		Block();
		return;
	}
	Operator();
	Operators();
	if (lexem->s != "endfor") ERROR("endfor");
	Get();
	if (lexem->s != ";") ERROR(";");
	Get();
}
void DoWhile() {
	if (lexem->s != "do") ERROR("do");
	Get();
	Block();
	if (lexem->s != "while") ERROR("while");
	Get();
	if (lexem->s != "(") ERROR("(");
	Get();
	NEExpression();
	if (lexem->s != ")") ERROR(")");
	Get();
}
void While() {
	if (lexem->s == "while") {
		Get();
		if (lexem->s != "(") ERROR("(");
		Get();
		NEExpression();
		if (lexem->s != ")") ERROR(")");
		Get();
		if (lexem->s == "{") {
			Block();
			return;
		}
		Operator();
		Operators();
		if (lexem->s != "endwhile") ERROR("endwhile");
		Get();
		if (lexem->s != ";") ERROR(";");
		Get();
	}
	ERROR("while");
}
void Else1()
{
	if (lexem->s == "else") {
		Get();
		Operator();
		Operators();
		if (lexem->s != "endif") ERROR("endif");
	}
	ERROR("else");
}
void Else() {
	if (lexem->s != "else")ERROR("else");
	Get();
	Block();
}
void Case() {
	if (lexem->s == "case") {
		Get();
		if (lexem->id == 3) {
			Get();
			if (lexem->s != ":") ERROR(":");
			Get();
			Operators();
			return;
		}
		ERROR("Const");
	} if (lexem->s == "default") {
		Get();
		if (lexem->s != ":") ERROR(":");
		Get();
		Operators();
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
					return;
				}
				do {
					Case();
				} while (lexem->s == "case" ||
					lexem->s == "default");
				if (lexem->s != "endswitch") ERROR("endswitch");
				Get();
				if (lexem->s != ";") ERROR(";");
				Get();
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
	while (lexem->s == ",")
	{
		Get();
		NEExpression();
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
	}
}

void Get(){
	do {
		lexem->s = "";
		lexem->id = 0;
		Code >> lexem->id;
		Code.get();
		for (char x = Code.peek(); x != '\n'; Code.get(), x = Code.peek()) {
			lexem->s += x;
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
	}
}
void Priority3()
{
	Priority4();
	while (lexem->s == "&&")
	{
		Sign3();
		Priority4();
	}
}
void Priority4()
{
	Priority5();
	while (lexem->s == "|")
	{
		Sign4();
		Priority5();
	}
}
void Priority5()
{
	Priority6();
	while (lexem->s == "^")
	{
		Sign5();
		Priority6();
	}
}
void Priority6()
{
	Priority7();
	while (lexem->s == "&")
	{
		Sign6();
		Priority7();
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
	}
}
void Priority12()
{
	if (lexem->s == "!") {
		Sign12();
		Priority13();
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
	}
	else
	{
		Priority14();
		if (lexem->s == "++" ||
			lexem->s == "--") {
			Sign13();
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
	if (lexem->id == 3)	{
		Get();return;
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
	Get();
	Operators();
	if (lexem->s != ">") ERROR(">");
	return;
}

void Variable(){
	if (lexem->s == "$"){
		Get();
		if (lexem->id == 2){
			Get();
			return;
		}
		ERROR("Name");
	}
	ERROR("$");
}

void Operators(){
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
			lexem->s == "++"||
			lexem->s == "--" ||
			lexem->id == 2 ||
			lexem->id == 3) {
			Expression();
			if (lexem->s != ";") ERROR(";");
		}
		else
		if (lexem->s == ";") {
			Get();
		}
		else return;
	}
}
void Operator(){
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
		if (lexem->s != ";") ERROR(";");
		return;
	}
	if (lexem->s == ";") {
		Get();
		return;
	}
	ERROR("Operator");
}

void Expression(){
	if (lexem->s == "$" ||
		lexem->s == "++" ||
		lexem->s == "(" ||
		lexem->s == "--" ||
		lexem->id == 2 ||
		lexem->id == 3) {
		NEExpression();
	}else	Get();
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
	}
	return;
}
void Assignable()
{
	Variable();
	while (lexem->s == "$")	{
		Variable();
	}
	if (lexem->s == "[")	{
		NEExpression();
		if (lexem->s != "]") ERROR("]");
	}
}
void Sign1(){
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
		lexem->s == ">>="){
		Get();
		return;
	}
	ERROR("= or -= or *= or **= or += or /= or .= or %= or &= or |= or ^= or <<= or >>=");
}
void Sign2(){
	if (lexem->s == "||"){
		Get();
		return;
	}
	ERROR("||");
}
void Sign3(){
	if (lexem->s == "&&"){
		Get();
		return;
	}
	ERROR("&&");
}
void Sign4(){
	if (lexem->s == "|"){
		Get();
		return;
	}
	ERROR("|");
}
void Sign5(){
	if (lexem->s == "^"){
		Get();
		return;
	}
	ERROR("^");
}
void Sign6(){
	if (lexem->s == "&"){
		Get();
		return;
	}
	ERROR("&");
}
void Sign7(){
	if (lexem->s == "==" ||
		lexem->s == "!=" ||
		lexem->s == "===" ||
		lexem->s == "!==" ||
		lexem->s == "<=>"){
		Get();
		return;
	}
	ERROR("== or != or === or !== or <=>");
}
void Sign8(){
	if (lexem->s == "<" ||
		lexem->s == ">" ||
		lexem->s == "<=" ||
		lexem->s == ">="){
		Get();
		return;
	}
	ERROR("< or > or <= or >=");
}
void Sign9(){
	if (lexem->s == "<<" ||
		lexem->s == ">>"){
		Get();
		return;
	}
	ERROR("<< or >>");
}
void Sign10(){
	if (lexem->s == "+" ||
		lexem->s == "-" ||
		lexem->s == "."){
		Get();
		return;
	}
	ERROR("+ or - or .");
}
void Sign11(){
	if (lexem->s == "*" ||
		lexem->s == "/" ||
		lexem->s == "%"){
		Get();
		return;
	}
	ERROR("* or / or %");
}
void Sign12(){
	if (lexem->s == "!"){
		Get();
		return;
	}
	ERROR("!");
}
void Sign13(){
	if (lexem->s == "++" ||
		lexem->s == "--"){
		Get();
		return;
	}
	ERROR("++ or --");
}
void Sign14(){
	if (lexem->s == "**"){
		Get();
		return;
	}
	ERROR("**");
}

void Element(){
	Expression();
	Get();
	return;
}

void GotoAndChildren(){
	if (lexem->s == "return") { Return(); if (lexem->s != ";") ERROR(";"); Get(); return; }
	if (lexem->s == "break") { Get(); if (lexem->s != ";") ERROR(";"); Get(); return; }
	if (lexem->s == "continue") { Get(); if (lexem->s != ";") ERROR(";"); Get(); return; }
	if (lexem->s == "goto") { Goto(); if (lexem->s != ";") ERROR(";"); Get(); return; }
	ERROR("return or break or continue or goto");
}
void Return(){
	if (lexem->s == "return"){
		Get();
		Expression();
		return;
	}
}
void Goto(){
	if (lexem->s == "goto"){
		Get();
		if (lexem->id == 2){
			Get();
			return;
		}
	}
}
void Label(){
	if (lexem->id == 2){
		Get();
		if (lexem->s == ":"){
			Get();
			return;
		}
	}
}
int main() {
	Get();
	try {
		Program();
	}
	catch (string s) {
		cout << "Error in string " << strings << "\nExpect: " << s << "\nGet: " << lexem->s << "\n";
		delete lexem;
		Code.close();
		system("pause");
		return 0;
	}
	delete lexem;
	Code.close();
	system("pause");
	return 0;
}