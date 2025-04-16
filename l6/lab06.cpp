#include <format>
#include <fstream>
#include <iostream>
#include <stack>
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
  enum ParserState { OK_PRS, STOP_PRS, ERROR_PRS };
  enum RelationSignal {
    EQ_RS,    // equal
    LS_RS,    // less
    MR_RS,    // more
    LSEQ_RS,  // less or more
    ER_RS     // error
  };

  unordered_map<string, castom_type> symtable;
  vector<string> symtable_keys;

  vector<Triad> triad_list;
  int triads;

  stack<Symbol> symbol_stack;
  stack<Symbol> action_stack;  // for *( , , ) and +( , , ,)

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
  void SetError(string _error_message, Symbol _symbol);
  void SetError(string _error_message, Signal error_char);
  void SetError(string _error_message);
  void ErrorOutput();

  static bool IsLetter(char c);
  static bool IsOp(char c);
  static bool IsDig(char c);
  static bool IsWS(char c);
  static bool IsDelim(char c);
  static bool IsInAlphabet(char c);
  static int NTtoIndex(char c);

  int SkipWS();
  void Get();
  castom_type GetC();
  string GetI();

  Symbol GetSymbol();
  enum ParserState Reduse();
  enum ParserState ReduseS_();
  static char GetRelation(Symbol a, Symbol b);
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
  symbol_stack.push(Symbol('$', 0, 0, "@"));
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
void Parser::SetError(string _error_message, Symbol _symbol) {
  string _error_string = string("type: ") + string{_symbol.type};
  if (_symbol.type == 'C')
    _error_string += string(" constant witg value: ") + _symbol.value;
  else if (_symbol.type == 'I')
    _error_string += string(" variable name: ") + _symbol.value;
  SetError(_error_message, _error_string);
}
void Parser::SetError(string _error_message) {
  SetError(_error_message, cur_c);
}
void Parser::ErrorOutput() {
  ofS << error_message << " in line: " << cur_line << ", pos: " << cur_pos
      << " in: " << error_string << endl;
  cout << error_message << " in line: " << cur_line << ", pos: " << cur_pos
       << " in: " << error_string << endl;
}

bool Parser::IsLetter(char c) {
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}
bool Parser::IsOp(char c) {
  return (c == '+' || c == '-' || c == '*' || c == ':');
}
bool Parser::IsDig(char c) { return c == '1' || c == '0'; }
bool Parser::IsWS(char c) {
  return c == '\n' || c == '\r' || c == ' ' || c == '\t';
}
bool Parser::IsDelim(char c) {
  return IsOp(c) || IsWS(c) || c == '(' || c == ')' || c == ',' || '[' || ']';
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

int Parser::NTtoIndex(char c) {
  int result;
  switch (c) {
    case 'S':
      result = 0;
      break;
    case 'E':
      result = 1;
      break;
    case 'T':
      result = 2;
      break;
    case ':':
      result = 3;
      break;
    case '(':
      result = 4;
      break;
    case ')':
      result = 5;
      break;
    case ',':
      result = 6;
      break;
    case '+':
      result = 7;
      break;
    case '*':
      result = 8;
      break;
    case '-':
      result = 9;
      break;
    case 'I':
      result = 10;
      break;
    case 'C':
      result = 11;
      break;
    case '$':
      result = 12;
      break;
    case '[':
      result = 13;
      break;
    case ']':
      result = 14;
      break;
    case 'M':
      result = 15;
      break;
    default:
      result = -1;
      break;
  }
  return result;
}
char Parser::GetRelation(Symbol a, Symbol b) {
  char relation_table[16][16] = {
      // clang-format off
	  /*              S     E     T     :     (     )     ,     +     *     -     I     C     $     [     ]     M     */
      /*    S   */ {0,    0,    0,    0,    0,    '>',  '>',  0,    0,    0,    '>',  '>',  '=',  '=',  '>',  0   },
      /*    E   */ {0,    0,    0,    0,    0,    '>',  '>',  0,    0,    0,    0,    0,    0,    0,    '>',  0   },
      /*    T   */ {0,    0,    '<',  0,    0,    '=',  '=',  0,    0,    0,    0,    0,    0,    0,    0,    0   },
      /*    :   */ {0,    0,    0,    0,    '<',  0,    0,    '<',  '<',  '<',  '<',  '<',  0,    '<',  0,    '=' },
      /*    (   */ {0,    '<',  '=',  0,    0,    0,    0,    '<',  '<',  '<',  '<',  '<',  0,    '<',  0 ,   '<' },
      /*    )   */ {0,    0,    0,    0,    0,    '>',  '>',  0,    0,    0,    0,    0,    0,    0,    '>',  0   },
      /*    ,   */ {0,    '<',  0,    0,    0,    0,    0,    '<',  '<',  '<',  '<',  '<',  0,    '<',  0,    '=' },
      /*    +   */ {0,    0,    0,    0,    '=',  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0   },
      /*    *   */ {0,    0,    0,    0,    '=',  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0   },
      /*    -   */ {0,    '=',  0,    0,    '<',  0,    0,    '<',  '<',  '<',  '<',  '<',  0,    '<',   0,    '='},
      /*    I   */ {0,    0,    0,    '=',  0,    '>',  '>',  0,    0,    0,    0,    0,    0,    0,    '>',  0   },
      /*    C   */ {0,    0,    0,    0,    0,    '>',  '>',  0,    0,    0,    0,    0,    0,    0,    '>',  0   },
      /*    $   */ {0,    0,    0,    0,    '<',  0,    0,    0,    0,    0,    0,    0,    0,    '<',  0,    0   },
      /*    [   */ {0,    0,    0,    0,    0,    '>',  '>',  0,    0,    0,    '=',  0,    0,    0,    0,    0   },
      /*    ]   */ {'=',  0,    0,    0,    '<',  '>',  '>',  0,    0,    0,    0,    0,    '>',  '>',  '>',  0   },
      /*    M   */ {0,    0,    0,    0,    0,    '>',  '>',  0,    0,    0,    0,    0,    0,    0,    '=',  0   },
      
  };
  // clang-format on
  char result;
  int x = NTtoIndex(a.type);
  int y = NTtoIndex(b.type);

  if (x < 0 || y < 0)
    result = 0;
  else
    result = relation_table[x][y];
  if (!result) result = '0';
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
  } else {
    result.type = static_cast<char>(cur_c);
    result.value = "";
    result.value = string{'@'};
    Get();
  }
  return result;
}

enum Parser::ParserState Parser::Reduse() {
  Symbol new_symbol;
  enum ParserState result = OK_PRS;
  string rule = "";
  Symbol bufferI, bufferC, bufferE, bufferT, bufferS, bufferM;

  while (symbol_stack.top().relatione == '=') {
    if (rule == ",M" && symbol_stack.top().type == 'T') break;
    // if(rule == "-" && symbol_stack.top().type=='E') break;
    rule = symbol_stack.top().type + rule;
    if (symbol_stack.top().type == 'I') bufferI = symbol_stack.top();
    if (symbol_stack.top().type == 'C') bufferC = symbol_stack.top();
    if (symbol_stack.top().type == 'E') bufferE = symbol_stack.top();
    if (symbol_stack.top().type == 'T') bufferT = symbol_stack.top();
    if (symbol_stack.top().type == 'S') bufferS = symbol_stack.top();
    if (symbol_stack.top().type == 'M') bufferM = symbol_stack.top();

    symbol_stack.pop();
  }
  rule = symbol_stack.top().type + rule;
  if (symbol_stack.top().type == 'I') bufferI = symbol_stack.top();
  if (symbol_stack.top().type == 'C') bufferC = symbol_stack.top();
  if (symbol_stack.top().type == 'E') bufferE = symbol_stack.top();
  if (symbol_stack.top().type == 'T') bufferT = symbol_stack.top();
  if (symbol_stack.top().type == 'S') bufferS = symbol_stack.top();
  if (symbol_stack.top().type == 'M') bufferM = symbol_stack.top();

  symbol_stack.pop();

  if (rule == "I" || rule == "C" || rule == "S") {
    new_symbol.type = 'E';
    if (rule == "I") {
      if (symtable.contains(bufferI.value)) {
        new_symbol.value = bufferI.value;
        new_symbol.triad_number = bufferI.triad_number;
      } else {
        triad_list.pop_back();
        SetError("Undefinded variable: '" + bufferI.value + "'");
      }
    } else if (rule == "C") {
      new_symbol.value = bufferC.value;
      new_symbol.triad_number = bufferC.triad_number;

    } else {
      new_symbol.triad_number = bufferS.triad_number;
    }
  } else if (rule == "S[I:M]" || rule == "[I:M]") {
    new_symbol.type = 'S';
    new_symbol.triad_number = triads++;

    if (!symtable.contains(bufferI.value))
      symtable_keys.push_back(bufferI.value);
    symtable[bufferI.value] = 0;

    triad_list.push_back(Triad('=',
                               string("^") + to_string(bufferI.triad_number),
                               string("^") + to_string(bufferM.triad_number)));
  } else if (rule == "-E") {
    new_symbol.type = 'E';
    new_symbol.triad_number = triads++;
    triad_list.push_back(
        Triad('-', string("^") + to_string(bufferE.triad_number), "@"));

  } else if (rule == "E") {
    new_symbol.type = 'M';
    new_symbol.triad_number = bufferE.triad_number;

  } else if (rule == "+(T)") {
    new_symbol.type = 'E';
    new_symbol.triad_number = bufferT.triad_number;
    action_stack.pop();

  } else if (rule == "*(T)") {
    new_symbol.type = 'E';
    new_symbol.triad_number = bufferT.triad_number;
    action_stack.pop();

  } else if (rule == "T,M") {
    new_symbol.type = 'T';
    new_symbol.triad_number = triads++;
    triad_list.push_back(Triad(action_stack.top().type,
                               string("^") + to_string(bufferT.triad_number),
                               string("^") + to_string(bufferM.triad_number)));

  } else if (rule == "M") {
    new_symbol.type = 'T';
    new_symbol.triad_number = bufferM.triad_number;
  } else {
    SetError("Unknown rule", rule);
  }
  if (cur_c != ERROR_SIGNAL) {
    new_symbol.relatione = GetRelation(symbol_stack.top(), new_symbol);
    cout << "Reduse: <" << rule << "> to <" << new_symbol.type
         << "> triad: " << new_symbol.triad_number << endl;
    symbol_stack.push(new_symbol);
  }
  return result;
}

void Parser::Parse() {
  enum ParserState parser_state = OK_PRS;
  Symbol new_simbol;

  Get();
  SkipWS();

  if (cur_c != ERROR_SIGNAL) new_simbol = GetSymbol();
  if (cur_c == ERROR_SIGNAL) parser_state = ERROR_PRS;

  while (parser_state == OK_PRS) {
    new_simbol.relatione = GetRelation(symbol_stack.top(), new_simbol);

    if (new_simbol.type == '$' && symbol_stack.top().type == 'S') {
      parser_state = STOP_PRS;
      cout << "Done" << endl;

    } else if (new_simbol.relatione == '<' || new_simbol.relatione == '=' ||
               new_simbol.relatione == '%') {
      symbol_stack.push(new_simbol);
      cout << "Shift: <" << symbol_stack.top().type << ">";
      if (symbol_stack.top().type == 'C' || symbol_stack.top().type == 'I')
        cout << " triad: " << symbol_stack.top().triad_number;
      cout << endl;
      if (new_simbol.type == '+' || new_simbol.type == '*')
        action_stack.push(new_simbol);

      SkipWS();

      new_simbol = GetSymbol();

      if (cur_c == ERROR_SIGNAL) parser_state = ERROR_PRS;

    } else if (new_simbol.relatione == '0') {
      parser_state = ERROR_PRS;

      SetError(
          "Unexpected nonterminal after " + string{symbol_stack.top().type},
          new_simbol);
    } else if (new_simbol.relatione == '>') {
      parser_state = Reduse();
    } else {
      SetError(string("Something wrong with table! Relation: ") +
               string{new_simbol.relatione});
    }
  }

  if (cur_c == ERROR_SIGNAL) {
    ErrorOutput();
  } else {
    TriadListPrint(ofS);
    // TriadListPrint();
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

  return 0;
}