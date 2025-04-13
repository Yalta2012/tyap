#include <iostream>
#include <fstream>
#include <unordered_map>
#include <format>
#include <vector>

#define castom_type int
using namespace std;

class Parser
{
public:
	enum Signal
	{
		EOF_SIGNAL = 256,
		ERROR_SIGNAL,
		OK_SIGNAL
	};

	unordered_map<string, castom_type> symtable;	
	vector<string> symtable_keys;
	enum Signal cur_c;
	enum Signal error_c;
	string error_message;
	size_t cur_line;
	size_t cur_pos;
	int operators_counter;

	ifstream inS;
	ofstream ofS;

	Parser(string input_path, string output_pat);
	~Parser();

	static bool IsLetter(char c);
	static bool IsOp(char c);
	static bool IsDig(char c);
	static bool IsWS(char c);
	static bool IsDelim(char c);
	static bool IsInAlphabet(char c);

	void Get();
	int SkipWS();
	void ErrorOutput(string error_message);
	void SetError(string error_message);

	string ProcI();
	castom_type ProcC();
	castom_type ProcE();
	castom_type ProcT();

	castom_type ProcS();
	void Parse();
};

Parser::Parser(string input_path, string output_pat)
{
	cur_c = (enum Signal)0;
	cur_line = 1;
	cur_pos = 0;
	error_c = OK_SIGNAL;
	error_message = "OK";
	operators_counter = 0;
	inS.open(input_path);
	ofS.open(output_pat);

	if (inS)
		inS >> noskipws;
}
Parser::~Parser()
{
	if (inS)
	{
		inS.close();
	}
	if (ofS)
	{
		ofS.close();
	}
}

bool Parser::IsLetter(char c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}
bool Parser::IsOp(char c)
{
	return (c == '+' || c == '-' || c == '*');
}
bool Parser::IsDig(char c)
{
	return c == '1' || c == '0';
}
bool Parser::IsWS(char c)
{
	return c == '\n' || c == '\r' || c == ' ' || c == '\t';
}
bool Parser::IsDelim(char c)
{
	return IsOp(c) || IsWS(c) || c == '(' || c == ')' || c == ',';
}
bool Parser::IsInAlphabet(char c)
{
	return IsLetter(c) || IsOp(c) || IsDig(c) || IsWS(c) || c == '(' || c == ')' || c == ',' || c == '#';
}

void Parser::Get()
{
	if (cur_c == ERROR_SIGNAL || cur_c == EOF_SIGNAL)
		return;

	char bc;
	if (inS >> bc)
	{
		if (IsInAlphabet(bc))
		{
			cur_c = (enum Signal)bc;
			if ((char)cur_c == '\n')
			{
				cur_line++;
				cur_pos = 0;
			}
			else

				cur_pos++;
		}
		else
		{

			SetError("The symbol is not included in the alphabet");
			error_c = (enum Signal)bc;
		}
	}
	else
		cur_c = EOF_SIGNAL;
}

void Parser::ErrorOutput(string error_message)
{
	ofS << error_message << " in line: " << cur_line << ", pos: " << cur_pos << " in char: \"";
	if (error_c == '\n')
		ofS << "\\n";
	else if (error_c == '\r')
		ofS << "\\r";
	else if (error_c == '\t')
		ofS << "\\t";
	else if (error_c == ' ')
		ofS << "[space]";
	else if (error_c == EOF_SIGNAL)
		ofS << "[end file]";

	else
		ofS << (char)(error_c);
	ofS << "\" (signal code:" << (int)error_c << ")" << endl;
}
void Parser::SetError(string error_message)
{
	if(cur_c != ERROR_SIGNAL){

	error_c = cur_c;
	cur_c = ERROR_SIGNAL;
	this->error_message = error_message;
	}
}

int Parser::SkipWS()
{
	int res = 0;
	while (cur_c == ' ' || cur_c == '\t' || cur_c == '\n' || cur_c == '\r')
	{
		res++;
		Get();
	}

	return res;
}

castom_type Parser::ProcC()
{

	castom_type val = 0;
	if (cur_c != '#')
			SetError("Expected '#'");
	else
		Get();

	if (cur_c != ERROR_SIGNAL)
	{
		if (IsDig((char)cur_c))
		{
			val <<= 1;
			val += cur_c - '0';
			Get();
		}
		else
			SetError("Expected '0' or '1'");
	}
	while (cur_c != ERROR_SIGNAL && cur_c != EOF_SIGNAL && IsDig((char)cur_c))
	{
		if (IsDig((char)cur_c))
		{
			val <<= 1;
			val += cur_c - '0';
			Get();
		}
	}

	if (cur_c != EOF_SIGNAL && !IsDelim(cur_c))
		SetError("Unexpected symbol");

	return val;
}

string Parser::ProcI()
{
	string res = "";

	if (cur_c != ERROR_SIGNAL)
	{
		if (IsLetter((char)cur_c))
		{
			res += (char)cur_c;
			Get();
		}
		else
			SetError("Expected letter or '_'");
	}
	while (cur_c != ERROR_SIGNAL && cur_c != EOF_SIGNAL && (IsDig((char)cur_c) || IsLetter((char)cur_c)))
	{
		res += (char)cur_c;
		Get();
	}
	if ( cur_c != EOF_SIGNAL && !IsDelim(cur_c))
		SetError("Unexpected symbol");
	return res;
}

castom_type Parser::ProcT()
{
	if (cur_c != '*' && cur_c != '+')
	{
		SetError("Something strange (from ProcT) :/");
		return 0;
	}

	const char action = (char)cur_c;
	castom_type result = (action == '*' ? 1 : 0);
	char empty_flag = 1;
	Get();
	SkipWS();
	if (cur_c != '(')
	{
		SetError("Expected '('");
	}

	if (cur_c != ERROR_SIGNAL)
	{
		Get();
		SkipWS();
	}
	
	while (cur_c != ERROR_SIGNAL && cur_c != EOF_SIGNAL && cur_c!= ')')
	{	
		
		int prepos = cur_pos;
		if (action == '*')
		{
			result *= ProcE();
		}
		else{
			result += ProcE();
		}

		if (cur_pos == prepos) empty_flag = 1;
		else empty_flag = 0;

		SkipWS();
		if (cur_c != ',' && cur_c != ')')
		{
			if(empty_flag == 0)
				SetError("Expected ',' or ')'");
			
			else
				SetError("Expected statement");
		}
		else if (cur_c == ',')
		{

			if(empty_flag==1){
				SetError("Expected statement");
			}
			else{

				Get();
				SkipWS();
				if (cur_c !='#' && !IsLetter(cur_c) && !IsOp(cur_c) && cur_c != '(') {
					SetError("Expected statement");
			}
		}

		}
		

	}
	if(cur_c == ')'){
		if(empty_flag == 1){
					SetError("Empty brakets");
		}
		else{

			Get();
			SkipWS();
		}
	}

	else 
		SetError("Expected ')'");
	
	return result;
}

castom_type Parser::ProcE()
{
	castom_type result = 0;
	if (cur_c == '-')
	{
		Get();
		SkipWS();
		result = -ProcE();
	}
	else if (cur_c == '+' || cur_c == '*')
	{
		result = ProcT();
	}

	else if (cur_c == '(')
	{

		result = ProcS();
	}
	else if (IsLetter(cur_c))
	{
		string var = ProcI();
		if (symtable.contains(var))
		{
			result = symtable[var];
		}
		else
		{
			SetError("Undefinded variable: '" + var + "'");
			cur_pos -= var.length() + 1;
			error_c = (enum Signal)var[0];
		}
	}
	else if (cur_c == '#')
	{
		result = ProcC();
	}
	else{
		SetError("Unexpecetd symbol");
	}

	return result;
}

castom_type Parser::ProcS()
{
	string var;
	castom_type val;
	if (cur_c != ERROR_SIGNAL)
	{
		if (cur_c == '(')
		{
			Get();
			SkipWS();
		}
		else

			SetError("Expected '('");
	}
	if (cur_c != ERROR_SIGNAL)
	{
		var = ProcI();
		SkipWS();
	}

	if (cur_c != ERROR_SIGNAL)
	{

		if (cur_c != ',')
			SetError("Expected ','");
		else
		{
			Get();
			SkipWS();
		}
	}
	if (cur_c != ERROR_SIGNAL)
	{
		val = ProcE();
	}
	if (cur_c!= ')'){
	
		SetError("Expected ')'");
	
	}
	else{
		Get();}
	if (cur_c != ERROR_SIGNAL)
	{
		if (!symtable.contains(var))
			symtable_keys.push_back(var);
		symtable[var] = val;
		operators_counter++;
		ofS << "Operator " << operators_counter << ":\t" << var << " = " << format("{:b}", symtable[var]) << endl;
	}
	return symtable[var];
}

void Parser::Parse()
{
	ofS << "Start parsing" << endl;

	Get();
	SkipWS();

	while (cur_c != EOF_SIGNAL && cur_c != ERROR_SIGNAL)
	{
		if (cur_c != EOF_SIGNAL)
			if (cur_c == '('){
				ProcS();

			}
			else{
			SetError("Unexpected symbol");
		}

		if (cur_c != EOF_SIGNAL)
		{
			
			SkipWS();
		}
	}

	if (cur_c == ERROR_SIGNAL)
	{
		ErrorOutput(error_message);
		ofS << "Abort parsing." << endl;
	}
	else
	{
		ofS << "End parsing." << endl;
		if (symtable_keys.size() == 0)
			ofS << "No variables defined." << endl;
		else
			ofS << symtable_keys.size() << " variable" << (symtable_keys.size() > 1 ? "s" : "") << " defined." << endl;
		for (string i : symtable_keys)
		{
			ofS << i << " = " << format("{:b}", symtable[i]) << endl;
		}
	}
}

int main(int argc, char *argw[])
{

	if (argc < 3 || argc > 4)
	{
		cout << "ERROR 1: needed 2 arguments\n";
		return 1;
	};
	string infile = string(argw[1]);
	string outfile = string(argw[2]);
	if (infile == outfile)
	{
		cout << "ERROR 2: same file names\n";
		return 2;
	}

	Parser parser(infile, outfile);
	if (!parser.inS)
	{
		cout << "ERROR 3: " << infile << "is not exist \n";
		return 3;
	}
	if (!parser.ofS)
	{
		cout << "ERROR 4: " << outfile << "is not exist \n";
		return 4;
	}

	parser.Parse();

	return 0;
}