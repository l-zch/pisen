#include <iostream>
#include <queue>
#include <regex>
#include <string.h>
#include <unordered_map>
#include <math.h>
#include <algorithm>


class Number{

public:

    std::vector<long long> digits;
	bool positive;

	Number (std::string str) {
		if(str[0] == '-'){
			positive = false;
			str = str.substr(1, str.size()-1);
		} else {
			positive = true;
		}
		digits.reserve(str.size()/18 + 1);
		for(long i=str.size();i>0;i-=18)
			digits.push_back(
				stoll(str.substr(i>18 ? i-18 : 0, i>18 ? 18: i)));
	}

	Number (long long n) {
		positive = n>0;
		digits = {n};
	}

	Number& operator = (Number other) {
		digits = other.digits;
		return *this;
	}

	Number operator + (Number other) {
		Number tmp = *this;
		tmp += other;
		return tmp;
	}

    Number& operator += (Number other) {
        size_t size = this->digits.size(), sizeOther = other.digits.size();
        std::vector<long long> digits(std::max(size, sizeOther));
        long long tmp, MAX = 1000000000000000000;
        for(size_t i=0; i<size || i<sizeOther; ++i){
            tmp = (i < size ? this->digits[i] : 0) + (i < sizeOther ? other.digits[i] : 0);
            digits[i] += tmp;
            if(digits[i] >= MAX){
                if(i==digits.size())
                    digits.resize(i+1);
                digits[i+1] = 1;
                digits[i] -= MAX;
            }
        }
        this->digits = digits;
        return *this;
    }

 	Number operator - (Number other) {
		Number tmp = *this;
		tmp -= other;
		return tmp;
	}

	Number& operator -= (Number other) {
		size_t size = this->digits.size(), sizeOther = other.digits.size();
		std::vector<long long> digits(std::max(size, sizeOther));
		long long tmp;
		for(size_t i=0; i<size||i<sizeOther; ++i){
			tmp = (i<size ? this->digits[i] : 0) - (i<sizeOther ? other.digits[i] : 0);
			digits[i] += tmp;
			if(digits[i] < 0){
				digits[i+1] = -1;
				digits[i] += 1000000000000000000;
			}
		}
		this->digits = digits;
		return *this;
	}

	Number operator * (Number other) {
		Number tmp = *this;
		tmp *= other;
		return tmp;
	}

	Number& operator *= (Number other) {
		using namespace std;
		string multiplier = other.tostr();
		reverse(multiplier.begin(), multiplier.end());
		Number result = Number("0");
		for(size_t i = 0; i<digits.size(); ++i) {
			for(size_t j = 0; j<multiplier.size(); ++j){
				result += Number(
					to_string(digits[i] * stoll(multiplier.substr(j,1))) + string(i*18 + j, '0')
				);
			}
		}
		digits = result.digits;
		return *this;
	}

	Number operator / (Number other) {
		Number tmp = *this;
		tmp /= other;
		return tmp;
	}

	Number& operator /= (Number other) {
		// TODO;
		return *this;
	}

	Number power(long long other) {
		Number cur = *this;
		Number result = Number("1");
		while (other){
			if(other % 2)
				result *= cur;
			other /= 2;
			cur *= cur;
		}
		return result;
	}

	Number power(Number other) {
		return power(other.digits[0]);
	}

	std::string tostr() {
		using namespace std;
		auto it = digits.rbegin();
		string str;
		while(!*it) ++it;
		for(str = to_string(*it++); it!=digits.rend(); ++it) {
			string s = to_string(*it);
			str += string(18 - s.size(), '0') + s;
		}
		return str;
	}

	friend std::ostream &operator << (std::ostream &out, Number number){
		std::cout << number.tostr();
		return std::cout;
	}

	bool operator == (Number other){
    	return tostr() == other.tostr();
	}

	bool operator != (Number other){ return !(*this == other); }

	bool operator < (Number other){
		std::string s1 = tostr(), s2 = other.tostr();
		int n = s1.size(), m = s2.size();
		if(n != m)
			return n < m;
		while(n--)
			if(s1[n] != s2[n])
				return s1[n] < s2[n];
		return false;
	}

	bool operator > (Number other){ return other < *this; }
	bool operator >= (Number other){ return !(*this < other); }
	bool operator <= (Number other){ return !(*this > other); }
};


/* TOKEN */

typedef enum {
	TK_PLUS,
	TK_MINUS,
	TK_MULTIPLY,
	TK_DIVIDE,
	TK_MODULUS,
	TK_POWER,
	TK_NUMBER,
	TK_LPAREN,
	TK_RPAREN,
	TK_EOF,
} TokenKind;


struct Token {
  operator bool()   { return true; }
  TokenKind kind;
  int idx;
  std::string value;
};

/* LEXER */
std::string input;

std::unordered_map<char, TokenKind> tokenMap = {
	{'+', TK_PLUS},
	{'-', TK_MINUS},
	{'/', TK_DIVIDE},
	{'%', TK_MODULUS},
	{'(', TK_LPAREN},
	{')', TK_RPAREN},
};

void error(std::string err, int pos) {
	using namespace std;
	cout << input << endl << string( pos, ' ' ) + "^ " << err << endl;
	exit(0);
}

std::string match_numebr(std::string s) {
	using namespace std;
	regex reg("\\d+(\\.+\\d+)*");
	smatch match;
	regex_search(s, match, reg);
	return match[0];
}

std::queue<Token> tokenize(std::string s) {
	input = s;
	std::queue<Token> tokens;
	unsigned idx = 0;
	char peek;

	while (idx < input.size()) {
		peek = input[idx];

		if (isspace(peek)) {
			++idx;
			continue;
		} else {
			Token token;
			token.idx = idx;
			if (tokenMap.count(peek)) {
				token.kind = tokenMap[peek];
				tokens.push(token);
				++idx;
				continue;
			}
			if (peek == '*') {
				if(input[++idx]=='*'){
					token.kind = TK_POWER;
					tokens.push(token);
					++idx;
				} else {
					token.kind = TK_MULTIPLY;
					tokens.push(token);
				}
				continue;
			}
			if (isdigit(peek)) {
				std::string value = match_numebr(input.substr(idx));
				token.kind = TK_NUMBER;
				token.value = value;
				tokens.push(token);
				idx += value.size();
				continue;
			}
			error("�аO�ѪR����", idx);
			break;
		}
//
//		Token token;
//		token.kind = TK_EOF;
//		tokens.push(token);
	}
	return tokens;
}

/* PARSER */
std::queue<Token> tokens;

long long term();
long long mul();
long long expr();
long long power();

long long power(){
	long long a = term();
	while(!tokens.empty()) {
		Token token = tokens.front();
		if (token.kind == TK_POWER){
			tokens.pop();
			long long b = term();
			long long n = a;
			while(--b)
				a *= n;
		}
		else break;
	}
	return a;
}

long long term() {
	if(tokens.empty())
        error("�⦡������", input.size());
	Token token = tokens.front();
	if (token.kind == TK_LPAREN) {
		tokens.pop();
		long long result = expr();
		if (tokens.front().kind == TK_RPAREN)
			tokens.pop();
		else
			error("�A��������", token.idx);
		return result;
	}
	if (token.kind == TK_NUMBER){
		tokens.pop();
		return std::stoll(token.value);
	} else
	    error("���O�ƭ�", token.idx);
	return 0;
}


long long mul() {
	long long a = power();
	while(!tokens.empty()) {
		Token token = tokens.front();
		if (token.kind == TK_MULTIPLY){
			tokens.pop();
			long long b = power();
			a *= b;
		}
		else if (token.kind == TK_DIVIDE){
			tokens.pop();
			long long b = power();
			a /= b;
		}
        else if (token.kind == TK_MODULUS){
			tokens.pop();
			long long b = power();
			a %= b;
		}
		else break;	
	}
	return a;
}

long long expr() {
	long long a = mul();
	while(!tokens.empty()) {
		Token token = tokens.front();
		if (token.kind == TK_PLUS) {
			tokens.pop();
			long long b = mul();
			a += b;
		}
		else if (token.kind == TK_MINUS) {
			tokens.pop();
			long long b = mul();
			a -= b;
		}
		else if (token.kind == TK_LPAREN || token.kind == TK_RPAREN)
			error("�A��������", token.idx);
	}
	return a;
}


long long parse() {
	/*
	expr  = mul ( "+" | "-" ) mul
	mul   = power ( "*" | "/" | "% ) power
	power = term "**" term
	term  = num | "(" expr ")"
	*/
	return expr();
}

// Number term();
// Number mul();
// Number expr();
// Number power();

// Number power(){
// 	Number a = term();
// 	while(!tokens.empty()) {
// 		Token token = tokens.front();
// 		if (token.kind == TK_POWER){
// 			tokens.pop();
// 			Number b = term();
// 			a = a.power(b);
// 		}
// 		else break;
// 	}
// 	return a;
// }

// Number term() {
// 	Token token = tokens.front();
// 	if(tokens.empty())
// 		error("�⦡������", input.size());
// 	if (token.kind == TK_LPAREN) {
// 		tokens.pop();
// 		Number result = expr();
// 		if (tokens.front().kind == TK_RLPAREN)
// 			tokens.pop();
// 		else
// 			error("�A��������", token.idx);
// 		return result;
// 	}
// 	if (token.kind == TK_NUMBER){
// 		tokens.pop();
// 		return Number(token.value);
// 	}
// 	error("���O�ƭ�", token.idx);
// 	return 0;
// }


// Number mul() {
// 	Number a = power();
// 	while(!tokens.empty()) {
// 		Token token = tokens.front();
// 		if (token.kind == TK_MULTIPLY){
// 			tokens.pop();
// 			Number b = power();
// 			a *= b;
// 		}
// 		else if (token.kind == TK_DIVIDE){
// 			tokens.pop();
// 			Number b = power();
// 			a /= b;
// 		}
// 		else break;
			
// 	}
// 	return a;
// }

// Number expr() {
// 	Number a = mul();
// 	while(!tokens.empty()) {
// 		Token token = tokens.front();
// 		if (token.kind == TK_PLUS){
// 			tokens.pop();
// 			Number b = mul();
// 			a += b;
// 		}
// 		else if (token.kind == TK_MINUS){
// 			tokens.pop();
// 			Number b = mul();
// 			a -= b;
// 		}
// 		else break;
// 	}
// 	return a;
	
// }


// Number parse() {
// 	return expr();
// }


/* MAIN */
int main() {
	using namespace std;
	string content;
	while(true){
		cout << "��J�⦡>> ";
		getline(cin, content);
		tokens = tokenize(content);
		cout << parse() << std::endl;
	}
	return 0;
}