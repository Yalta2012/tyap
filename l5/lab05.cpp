#include <format>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

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
  Triad(char _action, const char *_first, const char *_second);
  ~Triad();
  void Print(ofstream &stream);
  void Print();
  string ToString();
};

Triad::Triad(char _action, string _first, string _second) {
  action = _action;
  first = _first;
  second = _second;
}

Triad::Triad(char _action, const char *_first, const char *_second) {
  action = _action;
  first = _first;
  second = _second;
}

Triad::~Triad() {};

void Triad::Print(ofstream &stream) {
  stream << static_cast<char>(action) << "(" << first << ", " << second << ")";
}

void Triad::Print() {
  cout << static_cast<char>(action) << "(" << first << ", " << second << ")";
}

string Triad::ToString() {
  return string("") + static_cast<char>(action) + string("(") + first +
         string(", ") + second + string(")");
}

class Parser {
public:
  enum Signal { EOF_SIGNAL = 256, ERROR_SIGNAL, OK_SIGNAL };

  unordered_map<string, castom_type> symtable;

  vector<string> symtable_keys;
  vector<Triad> triad_list;

  enum Signal cur_c;
  enum Signal error_c;
  string error_message;
  size_t cur_line;
  size_t cur_pos;
  int operators_counter;
  size_t triads;
  size_t rule_lines;
  ifstream inS;
  ofstream ofS;

  void TriadListPrint();
  void TriadListPrint(ofstream &stream);

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

  string GetTriadVal(int index,int pos);
  void Optimize();
};

void Parser::TriadListPrint() {
  for (int triad_index = 0; triad_index < triad_list.size(); triad_index++) {
    if (triad_list[triad_index].action != '\0') {
      cout << triad_index << ": " << triad_list[triad_index].ToString() << endl;
    }
  }
}
void Parser::TriadListPrint(ofstream &stream) {
  for (int triad_index = 0; triad_index < triad_list.size(); triad_index++) {
    if (triad_list[triad_index].action != '\0') {
      stream << triad_index << ": " << triad_list[triad_index].ToString()
             << endl;
    }
  }
}

Parser::Parser(string input_path, string output_pat) {
  triad_list.push_back(Triad('\0', string(""), string("")));
  cur_c = (enum Signal)0;
  cur_line = 1;
  cur_pos = 0;
  triads = 1;
  rule_lines = 1;
  error_c = OK_SIGNAL;
  error_message = "OK";
  operators_counter = 0;
  inS.open(input_path);
  ofS.open(output_pat);

  if (inS)
    inS >> noskipws;
}
Parser::~Parser() {
  if (inS) {
    inS.close();
  }
  if (ofS) {
    ofS.close();
  }
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
  return IsLetter(c) || IsOp(c) || IsDig(c) || IsWS(c) || c == '(' ||
         c == ')' || c == ',' || c == '#';
}

void Parser::Get() {
  if (cur_c == ERROR_SIGNAL || cur_c == EOF_SIGNAL)
    return;

  char bc;
  if (inS >> bc) {
    if (IsInAlphabet(bc)) {
      cur_c = (enum Signal)bc;
      if ((char)cur_c == '\n') {
        cur_line++;
        cur_pos = 0;
      } else

        cur_pos++;
    } else {

      SetError("The symbol is not included in the alphabet");
      error_c = (enum Signal)bc;
    }
  } else
    cur_c = EOF_SIGNAL;
}

void Parser::ErrorOutput(string error_message) {
  ofS << error_message << " in line: " << cur_line << ", pos: " << cur_pos
      << " in char: \"";
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
void Parser::SetError(string error_message) {
  if (cur_c != ERROR_SIGNAL) {

    error_c = cur_c;
    cur_c = ERROR_SIGNAL;
    this->error_message = error_message;
  }
}

int Parser::SkipWS() {
  int res = 0;
  while (cur_c == ' ' || cur_c == '\t' || cur_c == '\n' || cur_c == '\r') {
    res++;
    Get();
  }

  return res;
}

castom_type Parser::ProcC() {

  castom_type val = 0;
  if (cur_c != '#')
    SetError("Expected '#'");
  else
    Get();

  if (cur_c != ERROR_SIGNAL) {
    if (IsDig((char)cur_c)) {
      val <<= 1;
      val += cur_c - '0';
      Get();
    } else
      SetError("Expected '0' or '1'");
  }
  while (cur_c != ERROR_SIGNAL && cur_c != EOF_SIGNAL && IsDig((char)cur_c)) {
    if (IsDig((char)cur_c)) {
      val <<= 1;
      val += cur_c - '0';
      Get();
    }
  }

  if (cur_c != EOF_SIGNAL && !IsDelim(cur_c))
    SetError("Unexpected symbol");

  return val;
}

string Parser::ProcI() {
  string res = "";

  if (cur_c != ERROR_SIGNAL) {
    if (IsLetter((char)cur_c)) {
      res += (char)cur_c;
      Get();
    } else
      SetError("Expected letter or '_'");
  }
  while (cur_c != ERROR_SIGNAL && cur_c != EOF_SIGNAL &&
         (IsDig((char)cur_c) || IsLetter((char)cur_c))) {
    res += (char)cur_c;
    Get();
  }
  if (cur_c != EOF_SIGNAL && !IsDelim(cur_c))
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
  char empty_flag = 1;

  int result;
  int temp_triad;
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

  if (cur_c != ERROR_SIGNAL)
  {
    temp_triad = ProcE();
    result = temp_triad;
    SkipWS();
    if (cur_c == ',')
    {
      Get();
      SkipWS();
      if (cur_c != '#' && !IsLetter(cur_c) && !IsOp(cur_c) && cur_c != '(')
      {
        SetError("Expected statement");
      }
    }
    else if (cur_c != ')')
    {
      SetError("Expected ',' or ')'");
    }
  }


  while (cur_c != ERROR_SIGNAL && cur_c != EOF_SIGNAL && cur_c != ')')
  {
    result = ProcE();
    triad_list.push_back(Triad(static_cast<char>(action),
                               "^" + to_string(temp_triad),
                               "^" + to_string(result)));
    temp_triad = triads;
    result = triads++;
    SkipWS();
    if (cur_c == ',')
    {
      Get();
      SkipWS();
      if (cur_c != '#' && !IsLetter(cur_c) && !IsOp(cur_c) && cur_c != '(')
      {
        SetError("Expected statement");
      }
    }
    else if (cur_c != ')')
    {
      SetError("Expected ',' or ')'");
    }
  }
  if (cur_c == ')')
  {
    Get();
    SkipWS();
  }
  else
    SetError("Expected ')'");
  return result;
}

castom_type Parser::ProcE() {
  castom_type val = 0;
  int result;
  if (cur_c == '-') {
    Get();
    SkipWS();
    result = ProcE();

    // ofS << triads << ":\t" << "-(" << result << ", @)" << endl;

    triad_list.push_back(Triad('-', "^" + to_string(result), "@"));

    result = triads++;
  } else if (cur_c == '+' || cur_c == '*') {
    result = ProcT();
  }

  else if (cur_c == '(') {

    result = ProcS();
  } else if (IsLetter(cur_c)) {
    string var = ProcI();
    if (symtable.contains(var)) {
      val = symtable[var];
      // ofS << triads << ":\t" << "V(" << var << ", @)" << endl;
      triad_list.push_back(Triad('V', var, "@"));

      result = triads++;
    } else {
      SetError("Undefinded variable: '" + var + "'");
      cur_pos -= var.length() + 1;
      error_c = (enum Signal)var[0];
    }
  } else if (cur_c == '#') {
    val = ProcC();
    // ofS << triads << ":\t" << "C(" << val << ", @)" << endl;
    triad_list.push_back(Triad('C', to_string(val), "@"));
    result = triads++;
  } else {
    SetError("Unexpecetd symbol");
  }

  return result;
}

castom_type Parser::ProcS() {
  string var;
  int name_triad_number;
  castom_type val;

  if (cur_c != ERROR_SIGNAL) {
    if (cur_c == '(') {
      Get();
      SkipWS();
    } else

      SetError("Expected '('");
  }
  if (cur_c != ERROR_SIGNAL) {
    var = ProcI();
    // ofS << triads << ":\t" << "V(" << var << ", @)" << endl;
    triad_list.push_back(Triad('V', var, "@"));

    name_triad_number = triads++;

    SkipWS();
  }

  if (cur_c != ERROR_SIGNAL) {

    if (cur_c != ',')
      SetError("Expected ','");
    else {
      Get();
      SkipWS();
    }
  }
  if (cur_c != ERROR_SIGNAL) {
    val = ProcE();
  }
  if (cur_c != ')') {

    SetError("Expected ')'");
  } else {
    Get();
  }
  if (cur_c != ERROR_SIGNAL) {
    if (!symtable.contains(var))
      symtable_keys.push_back(var);
    symtable[var] = val;
    operators_counter++;

    // ofS << triads << ":\t" << "=(^" << name_triad_number << ", ^" << val <<
    // ")" << endl;
    triad_list.push_back(Triad('=', string("^") + to_string(name_triad_number),
                               string("^") + to_string(val)));
  }
  return triads++;
}

void Parser::Parse() {
  // ofS << "Start translation" << endl;

  Get();
  SkipWS();
  int triad_index = 0;
  while (cur_c != EOF_SIGNAL && cur_c != ERROR_SIGNAL) {
    if (cur_c != EOF_SIGNAL)
      if (cur_c == '(') {
        cout << "(" << rule_lines++ << ")" << endl;
        ProcS();
        for (; triad_index < triad_list.size(); triad_index++) {
          if (triad_list[triad_index].action != '\0') {
            cout << triad_index << ": " << triad_list[triad_index].ToString()
                 << endl;
          }
        }
      } else {
        SetError("Unexpected symbol");
      }

    if (cur_c != EOF_SIGNAL) {

      SkipWS();
    }
  }

  if (cur_c == ERROR_SIGNAL) {
    ErrorOutput(error_message);
    ofS << "Abort translation." << endl;
  } else {
  }
}

string Parser::GetTriadVal(int index,int pos) {
  string result = string("^") + to_string(index);
  Triad *triad = &triad_list[index];
  switch (triad->action) {
  case 'C':
    result = triad->first;
    triad->second = "@";
    triad->action = '\0';
    break;
  case 'V':
   if(pos==1){
     result = triad->first;
     triad->action = '\0';
    }
    break;

  default:
    break;
  }

  return result;
}

void Parser::Optimize() {
  if (cur_c == ERROR_SIGNAL)
    return;

  for (int i = 0; i < triad_list.size(); i++) {
    Triad *triad = &triad_list[i];

    if (triad->second[0] == '^' && triad->action != '=') {
      triad->second = GetTriadVal(stoi(triad->second.substr(1)),2);
    }

    // if (triad->first[0] == '^' && (('0' <= triad->second[0] && '9' >= triad->second[0])||triad->second[0] == '^' )) {
    //   triad->first = GetTriadVal(stoi(triad->first.substr(1)),1);
    // }

    if((triad->action=='=' &&!('0' <= triad->second[0] && '9' >= triad->second[0]) )|| (triad->action!='=' &&(('0' <= triad->second[0] && '9' >= triad->second[0]) ||triad->action=='-') )){
      triad->first = GetTriadVal(stoi(triad->first.substr(1)),1);
    }

    // if(triad->action == '=' && || triad == '')

    if (triad->action == '+' || triad->action == '*') {
      if ('0' <= triad->first[0] && '9' >= triad->first[0] &&
          '0' <= triad->second[0] && '9' >= triad->second[0]) {

        int first_int, second_int;
        first_int = stoi(triad->first);
        second_int = stoi(triad->second);

        if (triad->action == '+')
          first_int += second_int;
        else
          first_int *= second_int;

        triad->first = to_string(first_int);
        triad->second = "@";
        triad->action = 'C';
      }
    } else if (triad->action == '-' && '0' <= triad->first[0] &&
               '9' >= triad->first[0]) {
      triad->first = to_string(-stoi(triad->first));
      triad->action = 'C';
    }
  }
}

int main(int argc, char *argw[]) {

  if (argc < 3 || argc > 4) {
    cout << "ERROR 1: needed 2 arguments\n";
    return 1;
  };
  string infile = string(argw[1]);
  string outfile = string(argw[2]);
  if (infile == outfile) {
    cout << "ERORR 2: same file names\n";
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
  parser.Optimize();
  parser.TriadListPrint(parser.ofS);

  return 0;
}