#include <iostream>
#include <fstream>
using namespace std;

enum StateEnum
{
	NORMAL_STATE,
	NORMAL_SLASH,
	MULTI_LINE_COMM,
	MULTI_LINE_COMM_STAR

};

enum StateEnum NewState(enum StateEnum old_state, char signal, ofstream &outS, char *comment_flag)
{
	enum StateEnum new_state = old_state;
	*comment_flag = 0;

	if (old_state == NORMAL_STATE)
	{
		if (signal == '/')
			new_state = NORMAL_SLASH;
		else
		{
			new_state = NORMAL_STATE;
			outS << signal;
		}
	}

	else if (old_state == NORMAL_SLASH)
	{
		if (signal == '*')
		{
			new_state = MULTI_LINE_COMM;
			*comment_flag = 1;
		}
		else
		{
			new_state = NORMAL_STATE;
			outS << "/";
		}
	}

	else if (old_state == MULTI_LINE_COMM)
	{
		if (signal == '*')
			new_state = MULTI_LINE_COMM_STAR;
		else
			new_state = MULTI_LINE_COMM;
	}

	else if (old_state == MULTI_LINE_COMM_STAR)
	{
		if (signal == '*')
			new_state = MULTI_LINE_COMM_STAR;
		else if (signal == '/')
			new_state = NORMAL_STATE;
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
	char comment_flag = 0;
	size_t last_comment_start = 0;
	char buffer_char = '\0';
	inS >> noskipws;

	StateEnum state = NORMAL_STATE;
	StateEnum prev_state = NORMAL_STATE;
	while (inS >> buffer_char)
	{
		state = NewState(state, buffer_char, outS, &comment_flag);
		if (comment_flag) last_comment_start = size_t(inS.tellg()) - 2;
	}

	if (state == MULTI_LINE_COMM || state == MULTI_LINE_COMM_STAR)
	{
		inS.clear();
		inS.seekg(last_comment_start, ios::beg);

		while (inS >> buffer_char)
		{
			outS << buffer_char;
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
