#include <format>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <stack>

#define castom_type int

using namespace std;

class Triad {
  // actions:
  // 0 for Empty
  // V for Variable
  // C for Constant
  // */+/- for Math
  // = for Assigment
 public:
  char action;
  string first;
  string second;
  Triad(char _action, string _first, string _second);
  Triad(char _action, const char* _first, const char* _second);
  ~Triad();
  void Print(ofstream& stream);
  void Print();
  string ToString();
};
Triad::Triad(char _action, string _first, string _second) {
  action = _action;
  first = _first;
  second = _second;
}
Triad::Triad(char _action, const char* _first, const char* _second) {
  action = _action;
  first = _first;
  second = _second;
}
Triad::~Triad() {};
string Triad::ToString() {
  return string("") + static_cast<char>(action) + string("(") + first +
         string(", ") + second + string(")");
}

class Symbol {
 public:
  char type;
  char relatione;
  int triad_number;
  string value;
  Symbol();
  Symbol(char _type, char _relatione, int _triada_number, string _value);
  ~Symbol();
};

Symbol::Symbol() {
  type = 0;
  relatione = 0;
  triad_number = 0;
  value = "";
}
Symbol::Symbol(char _type, char _relatione, int _triad_nubmer, string _value) {
  type = _type;
  relatione = _relatione;
  triad_number = _triad_nubmer;
  value = _value;
}
Symbol::~Symbol() {}

class Parser {
 public:
  enum Signal { EOF_SIGNAL = 256, ERROR_SIGNAL, OK_SIGNAL };

  unordered_map<string, castom_type> symtable;
  vector<string> symtable_keys;

  vector<Triad> triad_list;
  int triads;

  stack<Symbol>symbol_stack;
  stack<Symbol>action_stack; //for *( , , ) and +( , , ,)

  enum Signal cur_c;

  string error_string;

  string error_message;
  size_t cur_line;
  size_t cur_pos;

  ifstream inS;
  ofstream ofS;

  Parser(string input_path, string output_pat);
  ~Parser();

  void SetError(string _error_message, string error_string);
  void SetError(string _error_message, Signal error_char);
  void SetError(string _error_message);

  static bool IsLetter(char c);
  static bool IsOp(char c);
  static bool IsDig(char c);
  static bool IsWS(char c);
  static bool IsDelim(char c);
  static bool IsInAlphabet(char c);

  int SkipWS();
  void Get();
  castom_type GetC();
  string GetI();
  Symbol GetSymbol();
  void Parse();

  void TriadListPrint();
  void TriadListPrint(ofstream& stream);
};

Parser::Parser(string input_path, string output_pat) {
  error_message = "";
  cur_line = 1;
  cur_pos = 0;
  triads = 1;
  inS.open(input_path);
  ofS.open(output_pat);
  triad_list.push_back(Triad('\0', string(""), string("")));
  symbol_stack.push(Symbol('$',0,0,"@"));
  if (inS) inS >> noskipws;
}
Parser::~Parser() {
  if (inS) inS.close();
  if (ofS) ofS.close();
}

void Parser::TriadListPrint() {
  for (int triad_index = 0; triad_index < triad_list.size(); triad_index++) {
    if (triad_list[triad_index].action != '\0') {
      cout << triad_index << ": " << triad_list[triad_index].ToString() << endl;
    }
  }
}
void Parser::TriadListPrint(ofstream& stream) {
  for (int triad_index = 0; triad_index < triad_list.size(); triad_index++) {
    if (triad_list[triad_index].action != '\0') {
      stream << triad_index << ": " << triad_list[triad_index].ToString()
             << endl;
    }
  }
}

void Parser::SetError(string _error_message, string _error_string) {
  if (cur_c != ERROR_SIGNAL) {
    cur_c = ERROR_SIGNAL;
    error_message = _error_message;
    error_string = _error_string;
  }
}
void Parser::SetError(string _error_message, Signal error_char) {
  string _error_string;
  if (error_char == EOF_SIGNAL)
    _error_string = "[end file]";
  else if (error_char == '\n')
    _error_string = "\\n";
  else if (error_char == '\r')
    _error_string = "\\r";
  else if (error_char == '\t')
    _error_string = "\\t";
  else if (error_char == ' ')
    _error_string = "[space]";
  else
    _error_string = string{static_cast<char>(error_char)};
  SetError(_error_message, _error_string);
}
void Parser::SetError(string _error_message) {
  SetError(_error_message, cur_c);
}

bool Parser::IsLetter(char c) {
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}
bool Parser::IsOp(char c) { return (c == '+' || c == '-' || c == '*'); }
bool Parser::IsDig(char c) { return c == '1' || c == '0'; }
bool Parser::IsWS(char c) {
  return c == '\n' || c == '\r' || c == ' ' || c == '\t';
}
bool Parser::IsDelim(char c) {
  return IsOp(c) || IsWS(c) || c == '(' || c == ')' || c == ',';
}
bool Parser::IsInAlphabet(char c) {
  return IsLetter(c) || IsOp(c) || IsDig(c) || IsWS(c) || IsDelim(c) ||
         c == '#';
}
void Parser::Get() {
  if (cur_c == ERROR_SIGNAL || cur_c == EOF_SIGNAL) return;
  char buffer_char;
  if (inS >> buffer_char) {
    cur_c = static_cast<enum Signal>(buffer_char);
    if (!IsInAlphabet(buffer_char))
      SetError("Symbol is not in alphabet", cur_c);
    else if (static_cast<char>(cur_c) == '\n') {
      cur_line++;
      cur_pos = 0;
    } else
      cur_pos++;
  } else
    cur_c = EOF_SIGNAL;
}

int Parser::SkipWS() {
  int result = 0;
  while (cur_c == ' ' || cur_c == '\n' || cur_c == '\t' || cur_c == '\r') {
    result++;
    Get();
  }
  return result;
}

castom_type Parser::GetC() {
  castom_type result = 0;
  if (cur_c != '#')
    SetError("Expected '#'");
  else
    Get();

  if (cur_c != ERROR_SIGNAL) {
    if (IsDig(static_cast<char>(cur_c))) {
      result <<= 1;
      result += cur_c - '0';
      Get();
    } else
      SetError("Expected '0' or '1'");
  }
  while (cur_c != ERROR_SIGNAL && cur_c != EOF_SIGNAL &&
         IsDig(static_cast<char>(cur_c))) {
    result <<= 1;
    result += cur_c - '0';
    Get();
  }
  return result;
}

string Parser::GetI() {
  string result = "";

  if (cur_c != ERROR_SIGNAL) {
    if (IsLetter(static_cast<char>(cur_c))) {
      result += static_cast<char>(cur_c);
      Get();
    } else
      SetError("Expected letter or '_'");
  }
  while (
      cur_c != ERROR_SIGNAL && cur_c != EOF_SIGNAL &&
      (IsDig(static_cast<char>(cur_c)) || IsLetter(static_cast<char>(cur_c)))) {
    result += static_cast<char>(cur_c);
    Get();
  }
  if (cur_c != EOF_SIGNAL && !IsDelim(cur_c)) SetError("Unexpected symbol");

  return result;
}

Symbol Parser::GetSymbol() {
  Symbol result;
  if (cur_c == '#') {
    result.value = to_string(GetC());
    result.type = 'C';
    result.triad_number = triads++;
    triad_list.push_back(Triad('C', result.value, string("@")));
  } else if (IsLetter(cur_c)) {
    result.value = GetI();
    result.type = 'I';
    result.triad_number = triads++;
    triad_list.push_back(Triad('I', result.value, string("@")));
  } else if (cur_c == EOF_SIGNAL) {
    result.type = '$';
    result.value = "";
    result.value = string{'@'};
    Get();
  } else {
    result.type = static_cast<char>(cur_c);
    result.value = "";
    result.value = string{'@'};
    Get();
  }

  if(cur_c != ERROR_SIGNAL) symbol_stack.push(result);
  return result;
}

void Parser::Parse() {
  Get();
  SkipWS();
  while (cur_c != EOF_SIGNAL && cur_c != ERROR_SIGNAL) {
    Symbol a = GetSymbol();
    cout << a.type << " " << a.value << endl;
    SkipWS();
  }
}

int main(int argc, char* argw[]) {
  if (argc < 3 || argc > 4) {
    cout << "ERROR 1: needed 2 arguments\n";
    return 1;
  };
  string infile = string(argw[1]);
  string outfile = string(argw[2]);
  if (infile == outfile) {
    cout << "ERROR 2: same file names\n";
    return 2;
  }

  Parser parser(infile, outfile);
  if (!parser.inS) {
    cout << "ERROR 3: " << infile << "is not exist \n";
    return 3;
  }
  if (!parser.ofS) {
    cout << "ERROR 4: " << outfile << "is not exist \n";
    return 4;
  }

  parser.Parse();
  // parser.Optimize();
  parser.TriadListPrint();

  return 0;
}