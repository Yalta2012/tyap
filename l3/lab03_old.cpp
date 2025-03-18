#include <iostream>
#include <fstream>
#include <unordered_map>
#define  ll long long
using namespace std;



enum Signal{
	EOF_SIGNAL = 256,
	ERROR_SIGNAL,
};

enum Signal glob_c;
size_t glob_line = 1;
size_t glob_pos = 2;
enum Signal glob_error_c;
unordered_map<string,ll> symtable;

void Get(ifstream & inS);

bool IsLetter(char c);
bool IsOp (char c);
bool IsInAlphabet(char c);
bool IsDig(char c);

int SkipWS(ifstream &inS);

ll ProcS(ifstream & inS);
ll ProcC(ifstream & inS);

bool IsLetter(char c){
	return ((c>='a' && c<= 'z') || (c>='A' && c<= 'Z') || c == '_'); 
}
bool IsOp (char c){
	return (c=='+'||c=='-'||c=='*');
}
bool IsDig(char c){
	return c == '1' || c == '0';
}
bool IsWS(char c){
	return c=='\n' || c =='\r' || c == ' ' || c=='\t';
}
bool IsInAlphabet(char c){
	return IsLetter(c) || IsOp(c) || IsDig(c) || IsWS(c) || c== '(' || c==')' || c ==',' || c =='#';
}

void Parse(ifstream & inS, ofstream & outS);

void ErrorOutput(string error_message){
	cerr << error_message << endl;
}

int SkipWS(ifstream &inS){
	int res = 0;
	while (glob_c == ' ' || glob_c == '\t' || glob_c == '\n' || glob_c == '\r')
		{
			res++;
			Get(inS);
		}
	if(!res) Get(inS);
	return res;
}

ll ProcC(ifstream & inS){
	ll val = 0;
	if (glob_c != '#') {
		glob_c = ERROR_SIGNAL;
	}
	if(glob_c != ERROR_SIGNAL){
		Get(inS);
		if (IsDig((char)glob_c)){
			val<<=1;
			val+=glob_c-'0';
		}
		else{
			glob_c=ERROR_SIGNAL;
		}
	}

	while(glob_c != ERROR_SIGNAL && glob_c != EOF_SIGNAL && IsDig((char)glob_c)){
		Get(inS);
		if (IsDig((char)glob_c)){
			val<<=1;
			val+=glob_c-'0';
		}

		else{
			glob_c=ERROR_SIGNAL;
		}
	}
	
	return val;
}

void Get(ifstream & inS){
	char bc;
	if(inS>>bc){
		if (IsInAlphabet(bc)){
			glob_c = (enum Signal)bc;
			if((char)glob_c == '\n'){
				glob_line ++;
				glob_pos = 1;
				
			}
			else{
				glob_pos++;
			}
		}
		else{
			glob_error_c = (enum Signal)bc;
			glob_c = ERROR_SIGNAL;		
		}
	}
	else
		glob_c = EOF_SIGNAL;
	cout<<"("<<int(glob_c)<<")";
}

// ll ProcS(){

// }

void Parse(ifstream & inS, ofstream & outS){
	inS >> noskipws;

	Get(inS);
	while (glob_c != EOF_SIGNAL && glob_c != ERROR_SIGNAL){
		// ProcS();
			// cout<<(char)glob_c;
			// Get(inS);

			cout<<ProcC(inS)<<endl;
			SkipWS(inS);

		}
	if(glob_c == EOF_SIGNAL){
		cout<<endl<<"EOF"<<endl;
	}
	if(glob_c == ERROR_SIGNAL){

		Get(inS);
		cout<<endl<<"ERROR: "<< (int)glob_error_c  << " " << glob_line << " " << glob_pos << endl;;
	}
}


int main(int argc, char*argw[]){

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

	Parse(instream, outstream);

	instream.close();
	outstream.close();
	return 0;

}