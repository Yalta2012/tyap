#include <iostream>
#include <fstream>
#include <string>
using namespace std;

enum StateEnum
{
	NORMAL_ST,
	NORMAL_SLASH_ST,

	MULTI_LINE_COMM_ST,
	MULTI_LINE_COMM_STAR_ST,

	ONE_LINE_COMM_ST,

	STRING_ST,
	STRING_BSLASH_ST,

	CSTRING_ST,
	CSTRING_BSLASH_ST,

};

enum SignalEnum
{
	EOF_SIGNAL = 256,

};

enum StateEnum NewState(enum StateEnum old_state, enum SignalEnum signal, ofstream &outS, string *buffer_string)
{
	enum StateEnum new_state = old_state;
	switch (old_state)
	{
	case NORMAL_ST:
		if (signal == '/')
			new_state = NORMAL_SLASH_ST;
		else if (signal == '\"')
		{
			new_state = STRING_ST;
			outS << (char)signal;
		}
		else if (signal == '\'')
		{
			new_state = CSTRING_ST;
			outS << (char)signal;
		}
		else
		{
			new_state = NORMAL_ST;
			outS << (char)signal;
		}
		break;


	case NORMAL_SLASH_ST:
		if (signal == '*')
		{
			new_state = MULTI_LINE_COMM_ST;
			*buffer_string += '/';
			*buffer_string += (char)signal;
		}
		else if (signal == '/')
			new_state = ONE_LINE_COMM_ST;
		else if (signal == '\"')
			new_state = STRING_ST;
		else if (signal == '\'')
			new_state = CSTRING_ST;

		else
		{
			new_state = NORMAL_ST;
			outS << char('/');
			outS << (char)signal;
		}
		break;


	case ONE_LINE_COMM_ST:
		if (signal == '\n' || signal == '\r')
		{
			outS << (char)signal;
			new_state = NORMAL_ST;
		}
		break;


	case MULTI_LINE_COMM_ST:
		if (signal == '*')
		{
			*buffer_string += (char)signal;
			new_state = MULTI_LINE_COMM_STAR_ST;
		}
		else if (signal == EOF_SIGNAL)
		{
			outS << *buffer_string;
		}
		else
		{
			*buffer_string += (char)signal;
			new_state = MULTI_LINE_COMM_ST;
		}
		break;


	case MULTI_LINE_COMM_STAR_ST:
		if (signal == '*')
		{
			new_state = MULTI_LINE_COMM_STAR_ST;
			*buffer_string += '*';
		}
		else if (signal == '/')
		{
			new_state = NORMAL_ST;
			outS << " ";
			buffer_string->clear();
		}
		else if (signal == EOF_SIGNAL)
		{
			outS << *buffer_string;
		}
		else
			new_state = MULTI_LINE_COMM_ST;
		break;


	case STRING_ST:
		outS << (char)signal;

		if (signal == '\\')
			new_state = STRING_BSLASH_ST;
		else if (signal == '\"')
			new_state = NORMAL_ST;
		else
			new_state = STRING_ST;
		break;


	case CSTRING_ST:
		outS << (char)signal;
		if (signal == '\\')
			new_state = CSTRING_BSLASH_ST;
		else if ('\"')
			new_state = NORMAL_ST;
		else
			new_state = CSTRING_ST;
		break;


	case STRING_BSLASH_ST:
		outS << (char)signal;
		new_state = STRING_ST;
		break;


	case CSTRING_BSLASH_ST:
		outS << (char)signal;
		new_state = CSTRING_ST;
		break;

		
	default:
		break;
	}
	return new_state;
}

void Lex(ifstream &inS, ofstream &outS)
{

	string buffer_string = "";
	char buffer_char = '\0';
	inS >> noskipws;

	StateEnum state = NORMAL_ST;
	SignalEnum signal = (SignalEnum)0;
	while (inS >> buffer_char)
	{
		state = NewState(state, (SignalEnum)buffer_char, outS, &buffer_string);
	}
	state = NewState(state, EOF_SIGNAL, outS, &buffer_string);
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
		cout << "ERORR 2: same file names\n";
		return 2;
	}
	ifstream instream(infile);
	if (!instream.is_open())
	{
		cout << "ERROR 3: " << infile << "is not exist \n";
		return 3;
	}
	ofstream outstream(outfile);
	if (!outstream.is_open())
	{
		cout << "ERROR 4: " << outfile << "is not exist \n";
		instream.close();
		return 4;
	}

	Lex(instream, outstream);

	instream.close();
	outstream.close();
	return 0;
}
