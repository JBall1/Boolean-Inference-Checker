//By Joshua Ball


#include <locale>  
#include <sstream>
#include <cmath>
#include <string>
#include <list>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iostream>

using namespace std;


typedef struct AST *pNODE;
struct AST {
	string info;
	pNODE children[2];
};
struct inference {
	vector<AST> premises;
	AST conclusion;
};

void Insert(string s, list<string> *L);
list<string> vars(AST T);
list<string> vars(vector<AST> Ts);
list<bool> bits(int i, int N);
AST substitute(list<bool> vals, list<string> vars, AST Exp);
bool witnessInvalid(list<bool> vals, list<string> vars, inference I);
bool valid(inference I);
string validInference(string s);
//Probably not needed..
list<string> vectToList(vector<string> V);


//********* From Project 1 *********
struct tokRslt { bool success; vector<string> syms; };
struct parseRslt { bool success; inference I; };
tokRslt tokenize(string s);
parseRslt parse(vector<string> V);
struct TPERslt { bool val; string msg; };
//********* Helpers for parse *********
//pNODE Inference(vector<string> V, int i, int j);
pNODE Expression(vector<string> V, int i, int j);
pNODE Implcation(vector<string> V, int i, int j);
pNODE Disjunction(vector<string> V, int i, int j);
pNODE Conjunction(vector<string> V, int i, int j);
pNODE Negation(vector<string> V, int i, int j);
pNODE Constant(vector<string> V, int i, int j);
pNODE Unbreakable(vector<string> V, int i, int j);
pNODE LVAR(vector<string> V, int i, int j);
pNODE Ps(vector<string> V, int i, int j);
pNODE Inf(vector<string> V, int i, int j);
inference inf;
//********* End of helpers *********
void AssembleInference(AST T) {

	if (T.info == ":.")
	{
		inf.conclusion = *T.children[1];
		if (T.children[0]->info == ",")
		{
			AssembleInference(*T.children[0]);
		}
		else
		{
			inf.premises.push_back(*T.children[0]);
			return;
		}
	}
	if (T.info == ",")
	{

		if (T.children[0]->info == ",")
		{
			AssembleInference(*T.children[0]);
		}
		else
		{
			inf.premises.push_back(*T.children[0]);
		}

		if (T.children[1]->info == ",")
		{
			AssembleInference(*T.children[1]);
		}
		else
		{
			inf.premises.push_back(*T.children[1]);
		}
	}
	return;
}
string ASTtoString(AST T) //converts an AST to String
{
	string s;
	// If both children are NULL, just print the symbol
	if (T.children[0] == NULL) {
		s = s + T.info;
		return s;
	}

	// print an opening paren, followed by the symbol of T, followed
	// by a space, and then the first child.
	s = s + "(";
	s = s + T.info;
	s = s + " ";
	s += ASTtoString(*(T.children[0]));
	s = s + " ";

	// print the second child if there is one, and then a right paren.
	if (T.children[1] != NULL) {
		s += ASTtoString(*(T.children[1]));
	}
	s = s + ")";
	return s;
}
pNODE cons(string s, pNODE c1, pNODE c2) {
	pNODE ret = new AST;
	ret->info = s;  // same as (*ret).info = s
	ret->children[0] = c1;
	ret->children[1] = c2;
	return ret;
}
void prinTree(AST T)
{

	// If both children are NULL, just print the symbol
	if (T.children[0] == NULL) {
		cout << T.info;
		return;
	}

	// print an opening paren, followed by the symbol of T, followed
	// by a space, and then the first child.
	cout << "(" << T.info << " ";
	prinTree(*(T.children[0]));
	cout << " ";

	// print the second child if there is one, and then a right paren.
	if (T.children[1] != NULL)
		prinTree(*(T.children[1]));
	cout << ")";
}
void printList(list<string> R) {
	list<string>::const_iterator iterator;
	for (iterator = R.begin(); iterator != R.end(); iterator++) {
		cout << *iterator << ",";
	}
	cout << endl;
}
list<string> vectToList(vector<string> V) {
	list<string> L;
	for (size_t i = 0; i < V.size(); i++) {
		L.push_back(V[i]);
	}
	return L;
}
bool eval(AST T);

pNODE ASTpushAST(AST T) {
	pNODE hold = NULL, holder0 = NULL, holder1 = NULL;

	if (T.info != "\0") {
		if (T.children[0] != NULL) {
			holder0 = ASTpushAST(*T.children[0]);
		}

		if (T.children[1] != NULL) {
			holder1 = ASTpushAST(*T.children[1]);
		}
	}
	return hold = cons(T.info, holder0, holder1);
}

//**********************************
bool isSymbol(string v);
void Insert(string s, list<string> *L);
bool isSymbol(string v) {
	return (v == "T" || v == "F" || v == ":." || v == "(" || v == ")" || v == "=>" || v == "<=>" || v == "~" || v == "^" || v == "v" || v == "," || v == "<" || v == ">" || v == "=");
}
bool isOP(string v) {
	return ( v == "=>" || v == "<=>" || v == "^" || v == "v");
}
bool isSymbolChar(char v) {
	return (v == 'T' || v == 'F' || v == ':.' || v == '(' || v == ')' || v == '=>' || v == '<=>' || v == '~' || v == '^' || v == 'v' || v == ',' || v == '<' || v == '>' || v == '=');
}
//using ascii values!!!
bool isOPbool(string s) {
	if ((s.front() != 'v') && (s.front() >= 'a' && s.front() <= 'z')) {
		return true;
	}
	else {
		return false;
	}
}
tokRslt tokenize(string s) {
	tokRslt lex;
	string n;
	string c;
	int strLength = s.length();

	if (strLength == 1) {
		if (s == " ") {
			lex.syms.push_back("");
			lex.success = false;
			return lex;
		}
	}
	else if (s == "") {
		lex.syms.push_back("");
		lex.success = false;
		return lex;
	}

	for (int i = 0; i < strLength; i++) {
		n = "";
		n = n + s[i];
		c = s.substr(i, 1);
		//:.
		if (s[i] == ',') {
			lex.syms.push_back(",");
			lex.success = true;
			//i++;
		}

		else if (s[i] == ':') {
			if (s[i + 1] == '.') {
				i++;
				lex.syms.push_back(":.");
				lex.success = true;
			}
			else {
				lex.syms.push_back(n);
				lex.success = false;
				break;
			}
		}
		//=>
		else if (s[i] == '=') {
			//checking next element
			if (s[i + 1] == '>') {
				i++;
				lex.syms.push_back("=>");
				lex.success = true;
			}
			else {
				lex.syms.push_back(n);
				lex.success = false;
				break;
			}
		}
		//<=>
		else if (s[i] == '<') {
			if (s[i + 1] == '='&&s[i + 2] == '>') {
				i = i + 2;
				lex.syms.push_back("<=>");
				lex.success = true;
			}
			else {
				lex.syms.push_back(n);
				lex.success = false;
				break;
			}
		}
		//T
		else if (s[i] == 'T') {
			lex.syms.push_back("T");
			lex.success = true;
			//i++;
		}
		//F
		else if (s[i] == 'F') {
			lex.syms.push_back("F");
			lex.success = true;
			//	i++;
		}
		//~
		else if (s[i] == '~') {
			lex.syms.push_back("~");
			lex.success = true;
			//i++;
		}
		//^
		else if (s[i] == '^') {
			lex.syms.push_back("^");
			lex.success = true;
			//i++;
		}
		//v
		else if (s[i] == 'v') {
			lex.syms.push_back("v");
			lex.success = true;
			//	i++;
		}
		//(
		else if (s[i] == '(') {
			lex.syms.push_back("(");
			lex.success = true;
			//	i++;
		}
		//)
		else if (s[i] == ')') {
			lex.syms.push_back(")");
			lex.success = true;
			//	i++;
		}
		else if (s[i] == ' ') {
			//do nothing
		}
		else if (islower(s[i]) && s[i] != 'v') {
			int j = i;
			string t;
			while (j < s.length() && islower(s[j]) && !isSymbol(&s[j])) {
				j = j + 1;
			}
			for (int p = i; p < j; p++) {
				t.push_back(s[p]);
			}

			i = i + t.length() - 1;
			lex.syms.push_back(t);
			lex.success = true;
		}
		else if (isupper(s[i])) {
			lex.syms.push_back(c);
			lex.success = false;
			break;
		}
	}//end of main for loop

	return lex;
}

parseRslt parse(vector<string> V) {
	parseRslt rslt;
	int i = 0;//start of V
	int j = V.size();//size of V
	int commaCount = 0;
	int locpos = 0;
	int infCount = 0;
	bool isInf = false;
	rslt.success = true;
	int parenCount = 0;

	for (int g = 0; g < V.size()-1; g++) {
	
		if (V[g] == "(") {
			parenCount = parenCount + 1;
		}
		else if (V[g] == ")") {
			parenCount = parenCount - 1;
		}

	}
	if (parenCount != 0) {
		rslt.success = false;
		return rslt;
	}


	for (int k = 0; k < j; k++) {
		if (V[k] == ":.") {
			locpos = k;
			infCount++;
			if (infCount > 1) {
				rslt.success = false;
				return rslt;
			}
			isInf = true;
		}
	}

	if (isInf) {
		for (int h = 0; h < locpos; h++) {
			if (V[h] == ",") {
				commaCount++;
				//inf = true;
			}
		}
	}
	else if (!isInf) {
		for (int h = 0; h < j; h++) {
			if (V[h] == ",") {
				commaCount++;
				//inf = true;
			}
		}
	}
	

	if (commaCount > 0 && !isInf) {
		rslt.success = false;
		return rslt;
	}

	if (locpos + 1 == j) {
		rslt.success = false;
		return rslt;
	}


	pNODE pop = Inf(V, 0, V.size());
	
	if (pop == NULL) {
		rslt.success = false;
		return rslt;
	}


	AssembleInference(*pop);
	rslt.I = inf;
	
	for (int k = 0; k < rslt.I.premises.size(); k++) {
		AST temp = rslt.I.premises[k];
		string t1 = "";
		string t2 = "";

		if (temp.children[0] != NULL) {
			t1 = ASTtoString(*temp.children[0]);
		}
		if (temp.children[1] != NULL) {
			t2 = ASTtoString(*temp.children[1]);
		}
		if (t1 == "" || t2 == "") {
			if (isOP(temp.info)) {
				rslt.success = false;
				return rslt;
			}
		}
	}
	
	rslt.success = true;
	return rslt;

}

//The following are Parse helper functions:
//Checks for the following parameneters: if it is a bool expression
pNODE Inf(vector<string> V, int i, int j) {
	for (int c = i; c < j; c++) {
		if (V[c] == ":.") {
			pNODE ps = Ps(V, i, c);
			if (c + 1 == j) {
				return NULL;
			}
			for (int p1 = c + 1; p1 < j; p1++) {
				if (V[p1] == ",") {
					return NULL;
				}
			}
			pNODE infer = Expression(V, c + 1, j);
			if (ps != NULL && infer != NULL) {
				return cons(":.", ps, infer);
			}
		}
	}
	/*pNODE bexp = Expression(V, i, j);
	if (bexp != NULL) {
		return bexp;
	}*/
	return NULL;
}
pNODE Ps(vector<string> V, int i, int j) {
	if (i == j) {
		return NULL;
	}

	for (int c = i; c < j; c++) {
		if (V[c] == ",") {
			pNODE ps = Ps(V, i, c);
			pNODE bexp = Expression(V, c + 1, j);
			if (ps != NULL && bexp != NULL) {
				return cons(",", ps, bexp);//bexp changed in second spot
			}
		}
	}

	pNODE bexp = Expression(V, i, j);
	if (bexp != NULL) {
		return bexp;
	}

	return NULL;
}


pNODE Expression(vector<string> V, int i, int j) {
	for (int c = i; c<j; c++) {
		if (V[c] == "<=>") {
			pNODE t1 = Implcation(V, i, c);
			pNODE t2 = Expression(V, c + 1, j);
			if (t1 != NULL && t2 != NULL)
				return cons("<=>", t1, t2);
		}
	}
	pNODE W = Implcation(V, i, j);
	if (W != NULL) {
		return W;
	}
	return NULL;
}

//Checks for the following parameneters: T,F
pNODE Constant(vector<string> V, int i, int j) {
	if (i != j - 1 || i >= V.size()) {
		return NULL;
	}
	if (V[i] == "T" || V[i] == "F") {
		return cons(V[i], NULL, NULL);
	}
	return NULL;
}
//Checks for the following parameneters: ~
pNODE Negation(vector<string> V, int i, int j) {
	for (int c = i; c<j; c++) {
		if (V[c] == "~") {
			pNODE t2 = Negation(V, c + 1, j);
			if (t2 != NULL) {
				return cons("~", t2, NULL);
			}
		}
	}
	pNODE test = Unbreakable(V, i, j);
	if (test != NULL) {
		return test;
	}
	// All attempts failed
	return NULL;
}
//Checks for the following parameneters: ^(AND)
pNODE Conjunction(vector<string> V, int i, int j) {
	for (int c = i; c<j; c++) {
		if (V[c] == "^") {
			pNODE t1 = Conjunction(V, i, c);
			pNODE t2 = Negation(V, c + 1, j);
			if (t1 != NULL && t2 != NULL)
				return cons("^", t1, t2);
		}
	}
	pNODE A = Negation(V, i, j);
	if (A != NULL) {
		return A;
	}

	return NULL;
}
//Checks for the following parameneters: v(OR)
pNODE Disjunction(vector<string> V, int i, int j) {
	for (int c = i; c<j; c++) {
		if (V[c] == "v") {
			pNODE t1 = Disjunction(V, i, c);
			pNODE t2 = Conjunction(V, c + 1, j);
			
			if (t1 != NULL && t2 != NULL)
				return cons("v", t1, t2);
		}
	}
	pNODE A = Conjunction(V, i, j);
	if (A != NULL) {
		return A;
	}
	return NULL;
}
//Checks for the following parameneters: =>(false and true is false otherwise is true)
pNODE Implcation(vector<string> V, int i, int j) {
	for (int c = i; c<j; c++) {
		if (V[c] == "=>") {
			pNODE t1 = Disjunction(V, i, c);
			pNODE t2 = Implcation(V, c + 1, j);
			if (t1 != NULL && t2 != NULL)
				return cons("=>", t1, t2);
		}
	}
	pNODE A = Disjunction(V, i, j);
	if (A != NULL) {
		return A;
	}
	return NULL;
}
pNODE Unbreakable(vector<string> V, int i, int j) {
	pNODE test = Constant(V, i, j);
	if (test != NULL) {
		return test;
	}
	
	 if (V[i] == "(") {

		if (V[j - 1] == ")") {
			pNODE temp = Ps(V, i + 1, j - 1);
			if (temp != NULL) {
				return temp;
			}
		}
		else {

			for (int l = 0; l <= j; l++) {
				if (V[l] == ")") {
					
					pNODE temp = Ps(V, i + 1, l+1 );
					if (temp != NULL) {
						return temp;
					}
				}
			}

		}
	}
	
	// U::=  "(" Expression ")"
	
	pNODE test2 = LVAR(V, i, j);
	if (test2 != NULL) {
		return test2;
	}

	// All attempts failed
	return NULL;
}

pNODE LVAR(vector<string> V, int i, int j) {

	if (!(i == (j - 1))) {
		return NULL;
	}
	//using ascii was way better here
	if (V[i].front() != 'v' && (V[i].front() >= 'a' && V[i].front() <= 'z')) {
		return cons(V[i], NULL, NULL);
	}
	else {
		return NULL;
	}

}




//End of helpers for parse
//*************************************************************************************

//Done
void Insert(string s, list<string> *L) {
	list<string> R = *L;
	list<string>::const_iterator iterator;

	//transform(s.begin(), s.end(), s.begin(), toupper);

	bool found = (find(R.begin(), R.end(), s) != R.end());

	if (found) {
		//do nothing;
		return;
	}
	//Put s into R, sorted correctly.
	else if (!found) {
		R.push_back(s);
		R.sort();
		*L = R;
	}
}
//Done

list<string> vars(AST T) {
	list<string> temp;
	if (&T == NULL) {
		return temp;
	}
	string treeString = ASTtoString(T);
	int treeStringlength = treeString.length();
	for (int i = 0; i < treeString.length(); i++) {
		if (islower(treeString[i]) && treeString[i] != 'v') {
			string t;
			int j = i;
			while (j < treeStringlength && islower(treeString[j])) {
				j = j + 1;
			}
			for (int p = i; p < j; p++) {
				t.push_back(treeString[p]);
			}
			i = i + t.length();
			temp.push_back(t);
		}
	}
	temp.sort();
	temp.unique();
	return temp;
	
}
//Done
list<string> vars(vector<AST> Ts) {
	list<string> hold;
	list<string> temp;
	list<string>::const_iterator iterator;
	for (int i = 0; i < Ts.size(); i++) {
		temp = vars(Ts[i]);
		for (iterator = temp.begin(); iterator != temp.end(); iterator++) {
			hold.push_back(*iterator);
			
		}
	}
	hold.unique();
	hold.sort();
	
	return hold;
}

//TODO: DONE
//Converts i to a binary number of N digits. 
list<bool> bits(int i, int N) {
	list<bool> numbers;
	int remainder;
	int divisor = 2;
	int currentVal = 0;
	if (N < 0 || i < 0 || i > pow(2, N) - 1) {
		return numbers;
	}
	if (N > 0) {
		if (i > 0 && (i < (pow(2, N) - 1))) {
			while (i != 0) {
				remainder = i % divisor;
				numbers.push_back(remainder);
				i = i / 2;
			}
		}
	}
	if (numbers.size()< N) {
		while (numbers.size()< N)
			numbers.push_back(0);
	}
	numbers.reverse();
	return numbers;
}

//Done
AST substitute(list<bool> vals, list<string> vars, AST Exp) {
	//Exp by substituting "T" for vars[i] whenever vals[i] = 1, and "F" for vars[i] whenever vals[i] = 0.

	list<string>::iterator strIter = vars.begin();
	list<bool>::iterator boolIter = vals.begin();
	vector<string>holder;

	if (vars.size() == vals.size()) {
		for (int i = 0; boolIter != vals.end(); boolIter++, i++) {
			if (!*boolIter) {
				holder.push_back("F");
			}
			else if (*boolIter) {
				holder.push_back("T");
			}
		}

		for (int i = 0; strIter != vars.end(); i++, strIter++) {
			if (*strIter == Exp.info) {
				Exp.info = holder[i];
				break;
			}
		}
		if (Exp.children[0] != NULL) {
			*Exp.children[0] = substitute(vals, vars, *Exp.children[0]);
		}

		if (Exp.children[1] != NULL) {
			*Exp.children[1] = substitute(vals, vars, *Exp.children[1]);
		}
	}

	//Otherwise returns empty if vars != vals
	return Exp;
}

//Done
bool witnessInvalid(list<bool> vals, list<string> vars, inference I) {
	int psSize = I.premises.size();
	bool found = false;
	for (int i = 0; i < psSize; i++) {
		found = eval(substitute(vals, vars, I.premises[i]));
		if (found == false) {
			return false;
		}
	}
	if (found == true && eval(substitute(vals, vars, I.conclusion))) {
		return true;
	}

}

bool valid(inference I) {
	bool invalid = false;
	bool found = true;
	bool Ps;
	bool conC;
	vector<AST>holder;
	holder = I.premises;
	holder.push_back(I.conclusion);
	
	inference reg;

	list<string>lvars = vars(holder);
	
	if (lvars.size() == 0) {
		
		for (int i = 0; i < I.premises.size(); i++) {
			Ps = eval(I.premises[i]);
			if (!Ps) {
				break;
			}
		}
		if (Ps) {
			
			conC = eval(I.conclusion);
		}
		else {
			found = true;
			return found;
		}

		if (Ps && !conC) {
			found = false;
			return found;
		}
		else {
			found = true;
			return found;
		}
	}
	for (int i = 0; i < I.premises.size(); i++) {
		reg.premises.push_back(*ASTpushAST(I.premises[i]));
	}

	reg.conclusion = *ASTpushAST(I.conclusion);
	for (int j = 0; j < (pow(2, lvars.size()) - 1); j++) {
		invalid = witnessInvalid(bits(j, lvars.size()), lvars, I);
		if (invalid) {
			break;
		}
		for (int k = 0; k < I.premises.size(); k++) {
			I.premises[k] = *ASTpushAST(reg.premises[k]);
		}
		I.conclusion = *ASTpushAST(reg.conclusion);
	}

	if (invalid) {
		found = false;
	}
	else
		found = true;
	
	
	return found;
}

bool eval(AST T) {
	string op = T.info;
	bool temp;
	if (op == "~") {
		temp = eval(*T.children[0]);
		if (!temp) {
			return  1;
		}
		else
			return 0;
	}
	//F
	else if (op == "F") {
		return 0;
	}
	//T
	else if (op == "T") {
		return 1;
	}
	//OR
	else if (op == "v") {
		if (eval(*T.children[0]) || eval(*T.children[1])) {
			return 1;
		}
		else
			return 0;
	}
	//AND
	else if (op == "^") {
		if ((eval(*T.children[0]) && eval(*T.children[1]))) {
			return 1;
		}
		else
			return 0;
	}
	//false and true is false otherwise is true
	else if (op == "=>") {
		if (eval(*T.children[0]) && !eval(*T.children[1])) {
			return 0;
		}
		else
			return 1;
	}
	else if (op == "<=>") {
		if ((eval(*T.children[1]) == eval(*T.children[0]))) {

			return 1;
		}
		else
			return 0;
	}
	else
		return false;

}

//modified for proj2
TPERslt TPE(string s) {

	TPERslt tpe;
	tokRslt tempToken;
	parseRslt tempParse;

	tempToken = tokenize(s);


	tempParse = parse(tempToken.syms);

	if (!tempToken.success) {
		tpe.msg = "symbol error";
		return tpe;
	}
	else if (!tempParse.success) {
		tpe.msg = "grammar error";
		return tpe;
	}
	else if (tempParse.success && tempToken.success) {
		tpe.msg = "success";
		tpe.val = valid(tempParse.I);
		return tpe;
	}

}

string validInference(string s) {
	TPERslt tpe = TPE(s);
	if (tpe.msg == "success") {
		if (tpe.val) {
			return "valid";
		}
		else if(!tpe.val){
			return "invalid";
		}
	}
	else if (tpe.msg == "grammar error") {
		return "grammar error";
	
	}
	else if (tpe.msg == "symbol error") {
		return "symbol error";
	}
}
