#include <iostream>
#include <fstream>
using namespace std;

enum StateUnum{
	NORMAL_STATE,
	NORMAL_SLASH,
	MULTI_LINE_COMM,
	MULTI_LINE_COMM_STAR,


};

enum SignalUnum{
	REGULAR_SYMBOL,
	SLASH,
	STAR
};



void Lexer(ifstream inS, ofstream outS){
 size_t cursor=0;
 size_t buffer_len = 0;
 char buffer_char = '\0';
 inS>>buffer_char;
while(!inS>>buffer_char){
	cout<<buffer_char;
	outS<<buffer_char;
}
 
}

int main(int argc, char*argw[]){
	if (argc < 3 || argc > 4) {cout<<"ERROR 1: needed 2 arguments\n";
	return 1;};
	string infile = string(argw[1]);
	string outfile = string(argw[2]);
	if (infile == outfile) {cout<<"ERORR 2: same file names\n"; return 2;}
	ifstream instream(infile);
	if(! instream.is_open()){cout<<"ERROR 3: "<<infile<<"is not exist \n"; return 3;}
	ofstream outstream(outfile);
	if(! outstream.is_open()){cout<<"ERROR 4: "<<outfile<<"is not exist \n"; instream.close();return 4;}


	


	instream.close();
	outstream.close();
	return 0;
}