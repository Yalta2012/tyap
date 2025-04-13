#include <iostream>
#include <fstream>
#include <string>
using namespace std;

enum StateEnum
{
	NORMAL_STATE,
	NORMAL_SLASH,
	MULTI_LINE_COMM,
	MULTI_LINE_COMM_STAR

};

enum SignalEnum
{
	EOF_SIGNAL = 256,

};

enum StateEnum NewState(enum StateEnum old_state, enum SignalEnum signal, ofstream &outS, string *buffer_string)
{
	enum StateEnum new_state = old_state;

	if (old_state == NORMAL_STATE)
	{
		if (signal == '/')
			new_state = NORMAL_SLASH;
		else
		{
			new_state = NORMAL_STATE;
			outS << (char)signal;
		}
	}

	else if (old_state == NORMAL_SLASH)
	{
		if (signal == '*')
		{
			new_state = MULTI_LINE_COMM;
			*buffer_string += '/';
			*buffer_string += (char)signal;
		}
		else
		{
			new_state = NORMAL_STATE;
			outS << char('/');
			outS << (char)signal;
		}
	}

	else if (old_state == MULTI_LINE_COMM)
	{
		if (signal == '*')
		{
			*buffer_string += (char)signal;
			new_state = MULTI_LINE_COMM_STAR;
		}
		else if (signal == EOF_SIGNAL)
		{
			outS << *buffer_string;
		}
		else
		{
			*buffer_string += (char)signal;
			new_state = MULTI_LINE_COMM;
		}
	}

	else if (old_state == MULTI_LINE_COMM_STAR)
	{
		if (signal == '*')
		{
			new_state = MULTI_LINE_COMM_STAR;
			*buffer_string += '*';
		}
		else if (signal == '/')
		{
			new_state = NORMAL_STATE;
			outS << " ";
			buffer_string->clear();
		}
		else if (signal == EOF_SIGNAL)
		{
			outS << *buffer_string;
		}
		else
			new_state = MULTI_LINE_COMM;
	}

	else
	{
		new_state = old_state;
	}

	return new_state;
}

void Lex(ifstream &inS, ofstream &outS)
{

	string buffer_string = "";
	char buffer_char = '\0';
	inS >> noskipws;

	StateEnum state = NORMAL_STATE;
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
		cout << "ERROR 2: same file names\n";
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
