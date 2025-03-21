#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ID_LEN 50
#define MAX_VARS 20

/*
     ��������� ��������� � ����������� � ������������:
     - ����������� ���������� ��������� ��� ���������� - ������
     - ������ ���������� ����� ��������������
     - �� ����������� ������ ������� (�����������) ����� ���������
*/

/*
��������� ������ ����������

  S ::= I=E;                  �������� ������������
  E ::= E+T | E-T | T         �������� + � -
  T ::= T*M | T/M | M         �������� * � /
  M ::= (E) | -M | I | C      C�����, ������� �����, ����������, ���������
  I ::= A | AK                �������������: ������ �����,
  K ::= A | AK | D | DK          ����� ����� � �����
  C ::= D | CD                ���������: ������������������ ����
  A ::= a | b | ... | z | _   �����
  D ::= 0 | 1 | ... | 9       �����

������ ������� (�������, ������ � �.�.) ������
��������� ��� ��������� �� �����������, �� �����
���� ����� �����������, ����� ������ ����������
� ����� ����������.

� ����� lab03in.txt ��������� ���������� ��������� �� �����,
����������� ���� ����������. �������, �� ����� ����� (� ����
�����) ������� ����������� � ����� ������������ ������������.
*/

// ��������� ������: ����� ��������� � ���������� ���������
void Error(const char * msg, const char * param);

// ��������� ����������:
long * VarPointer(const char * name);   // ��������� ������ ����������
long VarValue(const char * name);       // ��������� �������� ����������
void VarPrint(void);                    // ������ ��������� ����������
void VarPrintAll(void);                 // ������ ���� ����������

// ������� ��������� ������
inline void Get(void);

// ��������� ������� �������������� ��������
void ProcS(void);
long ProcE(void);
long ProcT(void);
long ProcM(void);
long ProcI(void);   // ��� �� ������ ������ A, K, D
long ProcC(void);   // D ������, ������� ��� ���������� ��������� ������
long * ProcL(void); // ��� ��� I, ������ ����� �� =, ������ � L, �.�.
                    // ��������� ��������� �������� ���� ����������,
                    // � ���� ���������� � ��������� - �� �������� ��
                    // �������� (��� ProcI ������)


// ��������� ��� �������� ���������� (����������, ��������� ��������)
struct var
{
  char name[MAX_ID_LEN];
  long value;
};

var v[MAX_VARS];          // ������ ����������
int n = 0;                // �� ����������
int last = -1;            // � ����� ��������� �����������

FILE * f = 0;
int c = EOF;



void Error(const char * msg, const char * param)
{
  printf("Error: ");        // ������ ���� ������� ����� ������������
  printf(msg, param);       // ����������� ����� ����������, ������
  printf("\n");             // �������� ����� ��������� ���������
  fclose(f);                // �������� ������, �� ��� � �.�.,
  system("pause");          // � �� ������� ������ ��� ������������
  exit(7);
}



long * VarPointer(const char * name)
{
  if (!*name)
    Error("Syntax error.", 0);

  for (int i = 0; i < n; i++)
    if (strcmp(v[i].name, name) == 0)
      return &v[last = i].value;

  if (n >= MAX_VARS - 1)
    Error("Too many variables.", 0);

  strcpy(v[n].name, name);
  last = n;
  return &v[n++].value;
}

long VarValue(const char * name)
{
  if (!*name)
    Error("Syntax error.", 0);

  for (int i = 0; i < n; i++)
    if (strcmp(v[i].name, name) == 0)
      return v[i].value;

  Error("Undefined identifier \'%s\'.", name);
}

void VarPrint(void)
{
  if (last < 0)
    printf("No variables defined yet.\n");
  else
    printf("  %s = %ld\n", v[last].name, v[last].value);
}

void VarPrintAll(void)
{
  if (!n)
    printf("No variables defined yet.\n");
  else
    for (int i = 0; i < n; i++)
      printf("%s = %ld\n", v[i].name, v[i].value);
}



inline void Get(void)
{
  c = fgetc(f);
}



// Levels III-VI

long ProcC(void)
{
  long x = 0;
  while (c >= '0' && c <= '9')    // ���� ���� �����
  {
    x *= 10;                      // �������� �� ��� �����
    x += c - '0';                 // �� �������� �����������
    Get();                        // ������� ���������
  }
  return x;
}

long ProcI(void)
{
  char s[MAX_ID_LEN + 1] = {0};
  int i = 0;
  while ((c >= 'a' && c <= 'z') || c == '_' ||   // ���� ��� �����
    (i && c >= '0' && c <= '9'))                 // ��� �� ������ �����
  {
    if (i >= MAX_ID_LEN)                         // � ����� �� ���������
      Error("Too long identifier \'%s\'.", s);   // ����������, ��
    s[i++] = c;                                  // �������� ��
    Get();
  }
  if (!i)                                        // ����� ���-������?
    Error("Identifier missing.", 0);
  return VarValue(s);                 // ���������� �������� ����������
}

long * ProcL(void)
{
  char s[MAX_ID_LEN + 1] = {0};                  // ����������
  int i = 0;
  while ((c >= 'a' && c <= 'z') || c == '_' ||
    (i && c >= '0' && c <= '9'))
  {
    if (i >= MAX_ID_LEN)
      Error("Too long identifier \'%s\'.", s);
    s[i++] = c;
    Get();
  }
  
  if (!i)
    Error("Identifier missing.", 0);
  return VarPointer(s);               // �� ���������� ����� ����������
}


// Level II

long ProcM(void)
{
  long x;

  if (c == '(')                    // ��������� � �������
  {
    Get();
    x = ProcE();                      // ���������
    if (c != ')')                     // ������ ������� �� ������?
      Error("\')\' missing.", 0);
    Get();
  }
  else
    if (c == '-')                  // ������� �����
    {
      Get();
      x = -ProcM();                   // ���������
    }
    else
      if (c >= '0' && c <= '9')    // ���������
        x = ProcC();                  // �������
      else
        if ((c >= 'a' && c <= 'z') || c == '_')  // ����������
          x = ProcI();                              // ����� ��������
        else
          Error("Syntax error.", 0);  // ���������� ���

  return x;            // ���������� ��������� ��������
}

long ProcT(void)
{
  long x = ProcM();               // ������� �������
  while (c == '*' || c == '/')    // � ���� ���� ��������
  {
    char p = c;                   // ���������� ��������
    Get();
    if (p == '*')                 // � ������� ��������� �������
      x *= ProcM();               // �������� ��������� �� ����
    else
      x /= ProcM();               // ��� �������
  }
  return x;
}

long ProcE(void)
{
  long x = ProcT();               // ����������
  while (c == '+' || c == '-')
  {
    char p = c;
    Get();
    if (p == '+')
      x += ProcT();
    else
      x -= ProcT();
  }
  return x;
}


// Level I

void ProcS(void)
{
  long * p = ProcL();             // ����� ������� - �������������
  if (c != '=')                   // ����� =
    Error("\'=\' missing.", 0);
  Get();                          // ����� ������� ��������
  *p = ProcE();                   // ��������� - ������ �������
  if (c != ';')                   // ������ ������ ���� ;
    Error("\';\' missing.", 0);
  Get();
}


// High level

void Run(void)
{
  int i;
  Get();                          // ����� ������ ������ �
  for (i = 0; ; i++)          // � ����� ���������:
  {
    while (c >= 0 && c <= ' ')    // ��������� ������ �������
      Get();                      // ��, ����� � ����� ����������
    if (c == EOF)                 // � ���� �� ����� �����
      break;
    printf("Operator %2d: ", i+1);
    ProcS();                      // �� ��������� ��������� ��������
    VarPrint();                   // � ������� ��������� ��� ��������
  }
  printf("Result (%d variables defined in %d operators):\n", n, i);
  VarPrintAll();
}


int main(int argc, char ** argv)
{
  printf("\n");
  if (argc < 2)
  {
    printf("Command line parameter (file name) missing.\n");
    system("pause");
    return 1;
  }

  f = fopen(argv[1], "rb");
  if (!f)
  {
    printf("Can't open file \'%s\'.\n", argv[1]);
    system("pause");
    return 2;
  }

  Run();             // ��������� ������ ���������

  system("pause");
  fclose(f);
  return 0;
}
