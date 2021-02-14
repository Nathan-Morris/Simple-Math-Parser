#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "LinkedList.h"

#define SCOPE_EXP 1
#define SCOPE_MULT_DIV 2
#define SCOPE_ADD_SUB 3
#define DNAN ((double)NAN)

using namespace std;

typedef double(*OperatorCallback)(double, double);

double addOp(double a, double b) { return a + b; }
double subOp(double a, double b) { return a - b; }
double multOp(double a, double b) { return a * b; }
double divOp(double a, double b) { return a / b; }
double powOp(double a, double b) { return pow(a,b); } // yes it's redundant, argument overloading's annoying though

struct OperatorPosition {
	double value				= DNAN;
	OperatorCallback callback	= NULL;
	short equScope				= 0;
};

inline bool isInScope(OperatorCallback cb, char scope) {
	switch (scope)
	{
	case SCOPE_EXP:			return (cb == powOp);
	case SCOPE_MULT_DIV:	return (cb == multOp || cb == divOp);
	case SCOPE_ADD_SUB:		return (cb == addOp || cb == subOp);
	default:				return false;
	}
}

void printOperations(LinkedList<OperatorPosition> places) {
	for (unsigned int i = 0; i < places.length(); i++) {
		OperatorPosition pos = places[i];

		if (pos.callback == NULL) {
			cout << pos.value << " | " << pos.equScope << endl;
		}
		else {
			if (pos.callback == addOp) {
				cout << "+ | " << pos.equScope << endl;
			}
			else if (pos.callback == subOp) {
				cout << "- | " << pos.equScope << endl;
			}
			else if (pos.callback == multOp) {
				cout << "* | " << pos.equScope << endl;
			}
			else if (pos.callback == divOp) {
				cout << "/ | " << pos.equScope << endl;
			}
			else {
				cout << "^ | " << pos.equScope << endl;
			}
		}
	}
}
void performOperation(LinkedList<OperatorPosition>* places, unsigned int offsetOper) {
	double a = places->getAt(offsetOper - 1).value;
	double b = places->getAt(offsetOper + 1).value;
	places->replace({ places->getAt(offsetOper).callback(a, b), NULL }, offsetOper - 1);
	places->remove(offsetOper);
	places->remove(offsetOper);
}
double evaluateBase(LinkedList<OperatorPosition> places) {

	char opScope = SCOPE_EXP;
	short maxEquScope = 0;
	for (unsigned int i = 0; i < places.length(); i++) {
		OperatorPosition pos = places[i];
		if (maxEquScope < pos.equScope)
			maxEquScope = pos.equScope;
	}

	while (places.length() > 1) {
		for (unsigned short equS = maxEquScope; equS != USHRT_MAX; equS--) {
			for (char opS = SCOPE_EXP; opS < 4; opS++) {
				for (unsigned int i = 0; i < places.length(); i++) {
					OperatorPosition pos = places[i];
					if (pos.callback == NULL)
						continue;
					if (!isInScope(pos.callback, opS))
						continue;
					if (pos.equScope >= equS) {
						performOperation(&places, i);
					}
				}
			}

		}

	}

	return places.getAt(0).value;
}

LinkedList<OperatorPosition> parseBase(string input) {
	LinkedList<OperatorPosition> places;

	string inputBuffer = "";
	bool numberFlag = false;
	bool pointFlag = false;
	short equScopeC = 0;
	for (unsigned int i = 0; i < input.length(); i++) {
		char c = input[i];

		if (numberFlag) {
			if (isdigit(c) || (c == '.')) {
				inputBuffer += c;
				continue;
			}
			else {
				places.append({ stod(inputBuffer), NULL, equScopeC });
				inputBuffer.clear();
				numberFlag = false;
			}
		}

		switch (c)
		{
		case '(':
			equScopeC++;
			continue;

		case ')':
			equScopeC--;
			continue;

		case '+':
			places.append({ DNAN, addOp, equScopeC });
			continue;

		case '-':
			if (i != input.length() - 1) {
				if (isdigit(input[i + 1]) || input[i + 1] == '.') {
					inputBuffer += '-';
					continue;
				}
			}
			places.append({ DNAN, subOp, equScopeC });
			continue;

		case '*':
			places.append({ DNAN, multOp, equScopeC });
			continue;

		case '/':
			places.append({ DNAN, divOp, equScopeC });
			continue;

		case '^':
			places.append({ DNAN, powOp, equScopeC });
			continue;
		}

		if (isdigit(c) || (pointFlag = (c == '.'))) {
			numberFlag = true;
			inputBuffer += c;
		}
	}

	if (!inputBuffer.empty())
		places.append({ stod(inputBuffer), NULL, equScopeC });

	return places;
}

int main( int argc, char** argv, char** env ) {
	string inputBuf;
	while (1) {
		cout << "[Enter Equation]>>> ";
		getline(cin, inputBuf);
		LinkedList<OperatorPosition> pos = parseBase(inputBuf);
		cout << "Result: " << evaluateBase(pos) << endl;
		pos.clear();
	}
}