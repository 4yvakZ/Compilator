#include <iostream>
#include <fstream>
#include <string>
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

void Element();
void GotoAndChildren();
void Return();
void Goto();
void Label();

void Get(){
	Code >> lexem->id;
	Code >> lexem->s;
	return;
}
void ERROR(){
	//а чёрт знает, что тут будет
}






void Program(){
	if (lexem->s == "<?php"){
		Get();
		Operators();
		if (lexem->s == ">"){
			Get();
			return;
		}
		ERROR();
	}
	ERROR();
}

void Variable(){
	if (lexem->s == "$"){
		Get();
		if (lexem->id == 2){
			Get();
			return;
		}
		ERROR();
	}
	ERROR();
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
void NEExpression(){
	Assignable();
	Sign1();
	NEExpression();
	return;
}
void Assignable(){

}

void Sign1(){
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
	ERROR();
}
void Sign2(){
	if (lexem->s == "||"){
		Get();
		return;
	}
	ERROR();
}
void Sign3(){
	if (lexem->s == "&&"){
		Get();
		return;
	}
	ERROR();
}
void Sign4(){
	if (lexem->s == "|"){
		Get();
		return;
	}
	ERROR();
}
void Sign5(){
	if (lexem->s == "^"){
		Get();
		return;
	}
	ERROR();
}
void Sign6(){
	if (lexem->s == "&"){
		Get();
		return;
	}
	ERROR;
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
	ERROR();
}
void Sign8(){
	if (lexem->s == "<" ||
		lexem->s == ">" ||
		lexem->s == "<=" ||
		lexem->s == ">="){
		Get();
		return;
	}
	ERROR;
}
void Sign9(){
	if (lexem->s == "<<" ||
		lexem->s == ">>"){
		Get();
		return;
	}
	ERROR();
}
void Sign10(){
	if (lexem->s == "+" ||
		lexem->s == "-" ||
		lexem->s == "."){
		Get();
		return;
	}
	ERROR();
}
void Sign11(){
	if (lexem->s == "*" ||
		lexem->s == "/" ||
		lexem->s == "%"){
		Get();
		return;
	}
	ERROR();
}
void Sign12(){
	if (lexem->s == "!"){
		Get();
		return;
	}
	ERROR();
}
void Sign13(){
	if (lexem->s == "++" ||
		lexem->s == "--"){
		Get();
		return;
	}
	ERROR();
}
void Sign14(){
	if (lexem->s == "**"){
		Get();
		return;
	}
	ERROR;
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
	ERROR;
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