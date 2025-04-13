#include <iostream>
#include <fstream>
#include <string>
using namespace std;

enum StateEnum
{
	NORMAL_ST,
	TEXT_ST,
	NORMAL_SLASH_ST,

	MULTI_LINE_COMM_ST,
	MULTI_LINE_COMM_STAR_ST,

	ONE_LINE_COMM_ST,

	STRING_ST,
	STRING_BSLASH_ST,

	CSTRING_ST,
	CSTRING_BSLASH_ST,

	NUM0_ST,
	NUM2_ST,
	NUM2_START_ST,
	NUM8_ST,
	NUM10_ST,
	NUM16_ST,
	NUM16_START_ST,

	U_ST,
	L_ST,
	LL_ST,
	UL_ST,
	LU_ST,
	ULL_ST,

};

enum SignalEnum
{
	EOF_SIGNAL = 256,

};

bool IsTextSymbol(const char c)
{
	if ((c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		(c == '.') ||
		(c == '_'))
		return true;
	else
		return false;
}

bool IsIntSuffix(const char c)
{
	if (c == 'u' ||
		c == 'U' ||
		c == 'l' ||
		c == 'L')
		return true;
	else
		return false;
}

bool Is10Digit(const char c)
{
	if (c >= '0' && c <= '9')
		return true;
	else
		return false;
}

bool Is2Digit(const char c)
{
	if (c >= '0' && c <= '1')
		return true;
	else
		return false;
}

bool Is8Digit(const char c)
{
	if (c >= '0' && c <= '7')
		return true;
	else
		return false;
}

bool Is16Digit(const char c)
{
	if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
		return true;
	else
		return false;
}

enum StateEnum IntEndState(enum SignalEnum signal, ofstream &outS) // for /, ', ", u,l,U,L. THERE IS NOT LETTERS AND DIGITS
{
	StateEnum new_state = NORMAL_ST;

	if (signal == '/')
	{
		outS << " int\n";
		new_state = NORMAL_SLASH_ST;
	}
	else if (signal == '\'')
	{
		outS << " int\n";
		new_state = CSTRING_ST;
	}
	else if (signal == '\"')
	{
		outS << " int\n";
		new_state = STRING_ST;
	}
	else if (signal == 'u' || signal == 'U')
	{
		outS << (char)signal;
		new_state = U_ST;
	}
	else if (signal == 'l' || signal == 'L')
	{
		outS << (char)signal;
		new_state = L_ST;
	}
	else
	{
		new_state = NORMAL_ST;
		outS << " int\n";
	}
	return new_state;
}
enum StateEnum IntStartAndSuffixEndState(enum SignalEnum signal)
{
	StateEnum new_state = NORMAL_ST;
	if (signal == '/')
	{
		new_state = NORMAL_SLASH_ST;
	}
	else if (signal == '\'')
	{
		new_state = CSTRING_ST;
	}
	else if (signal == '\"')
	{
		new_state = STRING_ST;
	}
	return new_state;
}
enum StateEnum NewState(enum StateEnum old_state, enum SignalEnum signal, ofstream &outS)
{
	enum StateEnum new_state = old_state;
	switch (old_state)
	{
	case NORMAL_ST:
		if (signal == '/')
			new_state = NORMAL_SLASH_ST;
		else if (signal == '\"')
			new_state = STRING_ST;
		else if (signal == '\'')
			new_state = CSTRING_ST;
		else if (IsTextSymbol(signal))
			new_state = TEXT_ST;
		else if (signal == '0')
		{
			new_state = NUM0_ST;
			outS << (char)signal;
		}
		else if (Is10Digit(signal) && signal != '0') //[1-9]
		{
			new_state = NUM10_ST;
			outS << (char)signal;
		}
		else
			new_state = NORMAL_ST;
		break;

	case TEXT_ST:
		if (signal == '/')
			new_state = NORMAL_SLASH_ST;
		else if (signal == '\"')
			new_state = STRING_ST;
		else if (signal == '\'')
			new_state = CSTRING_ST;
		else if (IsTextSymbol(signal) || Is10Digit(signal))
			new_state = TEXT_ST;
		else
			new_state = NORMAL_ST;
		break;

	case NORMAL_SLASH_ST:
		if (signal == '*')
			new_state = MULTI_LINE_COMM_ST;
		else if (signal == '/')
			new_state = ONE_LINE_COMM_ST;
		else if (signal == '\"')
			new_state = STRING_ST;
		else if (signal == '\'')
			new_state = CSTRING_ST;
		else if (IsTextSymbol(signal))
			new_state = TEXT_ST;
		else if (signal == '0')
		{
			new_state = NUM0_ST;
			outS << (char)signal;
		}
		else if (Is10Digit(signal) && signal != '0') //[1-9]
		{
			new_state = NUM10_ST;
			outS << (char)signal;
		}
		else
			new_state = NORMAL_ST;
		break;

	case ONE_LINE_COMM_ST:
		if (signal == '\n' || signal == '\r')
			new_state = NORMAL_ST;
		break;

	case MULTI_LINE_COMM_ST:
		if (signal == '*')
			new_state = MULTI_LINE_COMM_STAR_ST;
		else
			new_state = MULTI_LINE_COMM_ST;
		break;

	case MULTI_LINE_COMM_STAR_ST:
		if (signal == '*')
			new_state = MULTI_LINE_COMM_STAR_ST;
		else if (signal == '/')
			new_state = NORMAL_ST;
		else
			new_state = MULTI_LINE_COMM_ST;
		break;

	case STRING_ST:
		if (signal == '\\')
			new_state = STRING_BSLASH_ST;
		else if (signal == '\"')
			new_state = NORMAL_ST;
		else
			new_state = STRING_ST;
		break;

	case CSTRING_ST:

		if (signal == '\\')
			new_state = CSTRING_BSLASH_ST;
		else if ('\"')
			new_state = NORMAL_ST;
		else
			new_state = CSTRING_ST;
		break;

	case STRING_BSLASH_ST:
		new_state = STRING_ST;
		break;

	case CSTRING_BSLASH_ST:
		new_state = CSTRING_ST;
		break;

	case NUM0_ST:
		if (signal == 'b')
		{
			new_state = NUM2_START_ST;
			outS << (char)signal;
		}
		else if (signal == 'x' || signal == 'X')
		{
			new_state = NUM16_START_ST;
			outS << (char)signal;
		}
		else if (Is8Digit(signal))
		{
			new_state = NUM8_ST;
			outS << (char)signal;
		}
		else if (IsTextSymbol(signal) && !IsIntSuffix(signal))
		{
			new_state = TEXT_ST;
			outS << (char)signal;
			outS << " ERROR";
		}
		else
			new_state = IntEndState(signal, outS);
		break;

	case NUM10_ST:
		if (Is10Digit(signal))
		{
			new_state = NUM10_ST;
			outS << (char)signal;
		}
		else if (IsTextSymbol(signal)&& !IsIntSuffix(signal))
		{
			new_state = TEXT_ST;
			outS << (char)signal;
			outS << " ERROR\n";
		}
		else{
			new_state = IntEndState(signal, outS);
		}
		break;

	case NUM8_ST:
		if (Is8Digit(signal))
		{
			new_state = NUM8_ST;
			outS << (char)signal;
		}
		else if (IsTextSymbol(signal) && !IsIntSuffix(signal))
		{
			new_state = TEXT_ST;
			outS << (char)signal;
			outS << " ERROR\n";
		}
		else
			new_state = IntEndState(signal, outS);
		break;

	case NUM2_START_ST:

		if (Is2Digit(signal))
		{
			new_state = NUM2_ST;
			outS << (char)signal;
		}
		else if (Is10Digit(signal) || IsTextSymbol(signal))
		{
			new_state = TEXT_ST;
			outS << (char)signal;
			outS << " ERROR\n";
		}
		else if (signal == '/' || signal == '\'' || signal == '\"')
		{
			new_state = IntEndState(signal, outS);
		}
		else if (signal == EOF_SIGNAL)
		{
			new_state = NORMAL_ST;
			outS << " ERROR\n";
		}
		else
		{
			new_state = NORMAL_ST;
			outS << (char)signal;
			outS << " ERROR\n";
		}
		break;

	case NUM2_ST:
		if (Is2Digit(signal))
		{
			new_state = NUM2_ST;
			outS << (char)signal;
		}
		else if (Is10Digit(signal) || (IsTextSymbol(signal) && !IsIntSuffix(signal)))
		{
			new_state = TEXT_ST;
			outS << (char)signal;
			outS << " ERROR\n";
		}
		else
			new_state = IntEndState(signal, outS);

		break;

	case NUM16_START_ST:
		if (Is16Digit(signal))
		{
			new_state = NUM16_ST;
			outS << (char)signal;
		}
		else if (IsTextSymbol(signal))
		{
			new_state = TEXT_ST;
			outS << (char)signal;
			outS << " ERROR\n";
		}
		else if (signal == '/' || signal == '\'' || signal == '\"')
		{
			new_state = IntEndState(signal, outS);
		}
		else if (signal == EOF_SIGNAL)
		{
			new_state = NORMAL_ST;
			outS << " ERROR\n";
		}
		else
		{
			new_state = NORMAL_ST;
			outS << (char)signal;
			outS << " ERROR\n";
		}
		break;

	case NUM16_ST:
		if (Is16Digit(signal))
		{
			new_state = NUM16_ST;
			outS << (char)signal;
		}
		else if (IsTextSymbol(signal) && !IsIntSuffix(signal))
		{
			new_state = TEXT_ST;
			outS << (char)signal;
			outS << " ERROR\n";
		}
		else
			new_state = IntEndState(signal, outS);
		break;

	case U_ST:
		if (signal == 'L' || signal == 'l')
		{
			outS << (char) signal;
			new_state = UL_ST;
		}
		else if (IsTextSymbol(signal) || Is10Digit(signal))
		{
			outS << (char) signal;
			outS << " ERROR\n";
			new_state = TEXT_ST;
		}
		else
		{
			new_state = IntStartAndSuffixEndState(signal);
			outS << " unsigned\n";
		}
		break;

	case L_ST:
		if (signal == 'U' || signal == 'u')
		{
			outS << (char) signal;
			new_state = LU_ST;
		}
		else if (signal == 'L' || signal == 'l')
		{
			outS << (char) signal;
			new_state = LL_ST;
		}
		else if (IsTextSymbol(signal) || Is10Digit(signal))
		{
			outS << (char) signal;
			outS << " ERROR\n";
			new_state = TEXT_ST;
		}
		else
		{
			new_state = IntStartAndSuffixEndState(signal);
			outS << " long\n";
		}
		break;

	case LL_ST:
		if (signal == 'U' || signal == 'u')
		{
			outS << (char) signal;
			new_state = ULL_ST;
		}

		else if (IsTextSymbol(signal) || Is10Digit(signal))
		{
			outS << (char) signal;
			outS << " ERROR\n";
			new_state = TEXT_ST;
		}
		else
		{
			new_state = IntStartAndSuffixEndState(signal);
			printf("%d %d", new_state, NORMAL_ST);
			outS << " long\n";
		}
		break;

	case UL_ST:
		if (signal == 'L' || signal == 'l')
		{
			outS << (char) signal;
			new_state = ULL_ST;
		}
		else if (IsTextSymbol(signal) || Is10Digit(signal))
		{
			outS << (char) signal;
			outS << " ERROR\n";
			new_state = TEXT_ST;
		}
		else
		{
			new_state = IntStartAndSuffixEndState(signal);
			outS << " unsigned long\n";
		}
		break;

	case LU_ST:
		if (IsTextSymbol(signal) || Is10Digit(signal))
		{
			outS << (char) signal;
			outS << " ERROR\n";
			new_state = TEXT_ST;
		}
		else
		{
			new_state = IntStartAndSuffixEndState(signal);
			outS << " unsigned long\n";
		}
		break;

	case ULL_ST:
		if (IsTextSymbol(signal) || Is10Digit(signal))
		{
			outS << (char) signal;
			outS << " ERROR\n";
			new_state = TEXT_ST;
		}
		else
		{
			new_state = IntStartAndSuffixEndState(signal);
			outS << " unsigned long long\n";
		}
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
		state = NewState(state, (SignalEnum)buffer_char, outS);
	}
	state = NewState(state, EOF_SIGNAL, outS);
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
