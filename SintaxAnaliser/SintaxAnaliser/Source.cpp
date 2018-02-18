#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
using namespace std;

static ifstream Code("D:\\IT_files\\Compilator\\Files\\Code.txt");
struct Lex{
	int id;
	string s;
};
Lex *lexem = new Lex;

void Get();
void Variable();
void Operators();
void Operator();
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

void Element();
void GotoAndChildren();
void Return();
void Goto();
void Label();

void Block()

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
	if (lexem->id == 2) {
		Get();
		if (lexem->s == "(") {
			Get();
			ListOfParameters();
			Get();
			if (lexem->s != ")") ERROR(")");
		}
	}
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
	Code >> lexem->id;
	Code >> lexem->s;
	return;
}
void ERROR(string s){
	char x;
	Code.get();
	for (x = Code.get(); x != '7'; x = Code.get()) {
		for (; x != '\n'; x = Code.get());
	}
	int strings;
	Code >> strings;
	cout << "Ошибка в строке " << strings << "\nОжидалось: " << s << "\nПолучено: " << lexem->s;
}

void Priority2()
{
	Priority3();
	Get();
	while (lexem->s == "||")
	{
		Sign2();
		Priority3();
		Get();
	}
}
void Priority3()
{
	Priority4();
	Get();
	while (lexem->s == "&&")
	{
		Sign3();
		Priority4();
		Get();
	}
}
void Priority4()
{
	Priority5();
	Get();
	while (lexem->s == "|")
	{
		Sign4();
		Priority5();
		Get();
	}
}
void Priority5()
{
	Priority6();
	Get();
	while (lexem->s == "^")
	{
		Sign5();
		Priority6();
		Get();
	}
}
void Priority6()
{
	Priority7();
	Get();
	while (lexem->s == "&")
	{
		Sign6();
		Priority7();
		Get();
	}
}
void Priority7()
{
	Priority8();
	Get();
	while (lexem->s == "==" ||
		lexem->s == "!=" ||
		lexem->s == "===" ||
		lexem->s == "!==" ||
		lexem->s == "<=>")
	{
		Sign7();
		Priority8();
		Get();
	}
}
void Priority8()
{
	Priority9();
	Get();
	while (lexem->s == "<" ||
		lexem->s == ">" ||
		lexem->s == "<=" ||
		lexem->s == ">=")
	{
		Sign8();
		Priority9();
		Get();
	}
}
void Priority9()
{
	Priority10();
	Get();
	while (lexem->s == "<<" ||
		lexem->s == ">>")
	{
		Sign9();
		Priority10();
		Get();
	}
}
void Priority10()
{
	Priority11();
	Get();
	while (lexem->s == "+" ||
		lexem->s == "-" ||
		lexem->s == ".")
	{
		Sign10();
		Priority11();
		Get();
	}
}
void Priority11()
{
	Priority12();
	Get();
	while (lexem->s == "*" ||
		lexem->s == "/" ||
		lexem->s == "%")
	{
		Sign11();
		Priority12();
		Get();
	}
}
void Priority12()
{
	Sign12();
	Priority13();
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
		Sign13();
	}
}
void Priority14()
{
	Priority15();
	Get();
	while (lexem->s == "**")
	{
		Sign14();
		Priority15();
		Get();
	}
}
void Priority15()
{
	if (lexem->s == "$") {
		Assignable(); Get(); return;
	}
	if (lexem->s == "(") {
		NEExpression();
		Get();
		if (lexem->s != ")") { Get(); return; }
		ERROR(")");
	}
	if (lexem->id == 3)	{
		Get();return;
	}
	FunctionCall();
	return;
}



void Program()
{
	if (lexem->s == "<?php") //!
	{
		Get();
		Operators();
		if (lexem->s == ">"){
			Get();
			return;
		}
		ERROR(">");
	}
	ERROR("<?php");
}

void Variable(){
	if (lexem->s == "$"){
		Get();
		if (lexem->id == 2){
			Get();
			return;
		}
		ERROR("Имя");
	}
	ERROR("$");
}

void Operators(){
	Operator();
	return;
}
void Operator(){
	return;
}

void Expression(){
	NEExpression();//non-empty expression
	Get();
	return;
}
void NEExpression()           ////////////////
{
	Assignable();
	Sign1();
	NEExpression();

	return;
}
void Assaignable()
{
	Variable();
	while (lexem->s == "$")
	{
		Variable();
	}
	if (lexem->s == "[")
	{
		NEExpression();
		if (lexem->s != "]") ERROR("]");
	}
}
void Sign1()
{
	if (lexem->s == "=" ||
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
		lexem->s == ">>="){
		Get();
		return;
	}
	ERROR("= или -= или *= или **= или /= или .= или %= или &= или |= или ^= или <<= или >>=");
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
	ERROR("== или != или === или !== или <=>");
}
void Sign8(){
	if (lexem->s == "<" ||
		lexem->s == ">" ||
		lexem->s == "<=" ||
		lexem->s == ">="){
		Get();
		return;
	}
	ERROR("< или > или <= или >=");
}
void Sign9(){
	if (lexem->s == "<<" ||
		lexem->s == ">>"){
		Get();
		return;
	}
	ERROR("<< или >>");
}
void Sign10(){
	if (lexem->s == "+" ||
		lexem->s == "-" ||
		lexem->s == "."){
		Get();
		return;
	}
	ERROR("+ или - или .");
}
void Sign11(){
	if (lexem->s == "*" ||
		lexem->s == "/" ||
		lexem->s == "%"){
		Get();
		return;
	}
	ERROR("* или / или %");
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
	ERROR("++ или --");
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
	if (lexem->s == "return") { Get(); Return(); return; }
	if (lexem->s == "break") { Get(); return; }
	if (lexem->s == "continue") { Get(); return; }
	if (lexem->s == "goto") { Get(); Goto(); return; }
	if (lexem->id == 2) { Get(); Label(); return; }
	ERROR("return или break или continue или goto или Имя");
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




int main(){
	Get();

	system("pause");
	return 0;
}