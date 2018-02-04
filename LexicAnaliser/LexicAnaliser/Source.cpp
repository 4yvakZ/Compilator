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
	char a;
	for (a = fin.get(); !fin.eof(); fin.get(), a = fin.get()) {
		if (a == x) {
			return true;
		}
	}
	return false;
}

bool IsSign(char x) {
	ifstream fin("D:\\IT_files\\Compilator\\Files\\Signs.txt");
	char a;
	for (a = fin.get(); !fin.eof(); fin.get(), a = fin.get()) {
		if (a == x) {
			return true;
		}
	}
	return false;
}

bool IsWord(string s) {
	ifstream fin("D:\\IT_files\\Compilator\\Files\\Words.txt");
	string a;
	for (fin >> a; !fin.eof(); fin.get(), fin >> a) {
		if (a == s) {
			return true;
		}
	}
	if (a == s) {
		return true;
	}
	return false;
}

int main() {
	ifstream fin("D:\\IT_files\\Compilator\\Files\\Program.txt");
	ofstream fout("D:\\IT_files\\Compilator\\Files\\Code.txt");
	char x, a;
	string s;
	states state = Start;
	for (; !fin.eof();) {
		switch (state) {
		case Start:
			x = fin.get();
			if (x == EOF) return 1;
			if (x == ' ' || x == '\n' || x == '	') {
				state = Start;
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
			else if (x == '"' || x == 39) {
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
			fout << "4 " << x;
			a = fin.peek();
			if (IsOperation(a)) {
				if (a == x && a != '!'&&a != '%'&&a != ',' && a != '~') {
					fout << x;
					fin.get();
				}
				else if (x != ','&&a == '=' || x == '-'&&a == '>') {
					fout << a;
					fin.get();
				}
			}
			fout << "\n";
			state = Start;
			break;
		case B:
			fout << "3 " << x;
			for (x = fin.peek(); isdigit(x); fout << x, fin.get(), x = fin.peek());
			fout << "\n";
			state = Start;
			if (isalpha(x)) {
				cout << "Error" << "\n";
				return 1;
			}
			break;
		case C:
			s = "";
			s += x;
			state = Start;
			for (x = fin.peek(); isalnum(x) || x == '_'; s += x, fin.get(), x = fin.peek());
			if (IsWord(s)) {
				fout << "1 ";
			}
			else {
				fout << "2 ";
			}
			fout << s << "\n";
			break;
		case E:
			fout << "3 " << x;
			a = x;
			for (x = fin.get(); x != a; fout << x, x = fin.get());
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
			break;
		case I:
			for (x = fin.get(); x != EOF;) {
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
}