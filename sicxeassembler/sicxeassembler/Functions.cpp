
#include <bits/stdc++.h>
using namespace std;
#define endl '\n'

// EXPANDS THE INPUT STRING TO A GIVEN INPUT SIZE.
string expandString(string datainput, int length, char fillCh, bool backistrue = false)
{
  if (backistrue)
  {
    if (length <= datainput.length())
    {
      return datainput.substr(0, length);
    }
    else
    {
      for (int i = length - datainput.length(); i > 0; i--)
      {
        datainput += fillCh;
      }
    }
  }
  else
  {
    if (length <= datainput.length())
    {
      return datainput.substr(datainput.length() - length, length);
    }
    else
    {
      for (int i = length - datainput.length(); i > 0; i--)
      {
        datainput = fillCh + datainput;
      }
    }
  }
  return datainput;
}

// CONVERTS THE HEXADECIMAL STRING TO INTEGER AND RETURNS IT.
int stringHexToInt(string x)
{
  return stoul(x, nullptr, 16);
}

// IT CONVERTS THE INPUT INT VALUE TO A HEXADECIMAL STRING.
string intToHexString(int y, int x = 5)
{
  stringstream s;
  s << setfill('0') << setw(x) << hex << y;
  string st = s.str();
  st = st.substr(st.length() - x, x);
  transform(st.begin(), st.end(), st.begin(), ::toupper);
  return st;
}

// STRING INPUT VALUE WHICH IS IN INT IS CONVERTED TO HEXADECIMAL STRING.
string stringToHexString(const string &input)
{
  static const char *const lut = "0123456789ABCDEF";
  size_t len = input.length();

  string output;
  output.reserve(2 * len);
  for (size_t i = 0; i < len; ++i)
  {
    const unsigned char c = input[i];
    output.push_back(lut[c >> 4]);
    output.push_back(lut[c & 15]);
  }
  return output;
}

// CHECKS IF THERE ARE ANY BLANK SPACES.
bool checkIfItsBlankSpace(char x)
{
  if (x == ' ' || x == '\t')
  {
    return true;
  }
  return false;
}

// CHECKS IF ITS A COMMENT LINE.
bool checkIfItsCommentLine(string line)
{
  if (line[0] == '.')
  {
    return true;
  }
  return false;
}

bool if_all_numbers(string x)
{
  bool all_num = true;
  int i = 0;
  while (all_num && (i < x.length()))
  {
    all_num &= isdigit(x[i++]);
  }
  return all_num;
}

// GET OPCODES.
string getFinalOp(string opcode)
{
  if (opcode[0] == '+' || opcode[0] == '@')
  {
    return opcode.substr(1, opcode.length() - 1);
  }
  return opcode;
}

// READING THE FIRST NON-BLANK SPACE.
void readFirstNonBlankSpace(string line, int &ind, bool &status, string &datainput, bool readTillTheEnd = false)
{
  datainput = "";
  status = true;
  if (readTillTheEnd)
  {
    datainput = line.substr(ind, line.length() - ind);
    if (datainput == "")
    {
      status = false;
    }
    return;
  }
  while (ind < line.length() && !checkIfItsBlankSpace(line[ind]))
  { // If no whitespace is prsent then it is some kind of data
    datainput += line[ind];
    ind++;
  }

  if (datainput == "")
  {
    status = false;
  }

  while (ind < line.length() && checkIfItsBlankSpace(line[ind]))
  { // Increase ind until we skip all whitespace
    ind++;
  }
}

// GET THE FORMAT OF THE FLAG.
char getFlagFrmt(string input)
{
  if (input[0] == '#' || input[0] == '+' || input[0] == '@' || input[0] == '=')
  {
    return input[0];
  }
  return ' ';
}

// READ THE BYTE OPERAND.
void readByteOperand(string line, int &ind, bool &status, string &datainput)
{
  datainput = "";
  status = true;
  if (line[ind] == 'C')
  {
    datainput += line[ind++];
    char identifier = line[ind++];
    datainput += identifier;
    while (ind < line.length() && line[ind] != identifier)
    {
      datainput += line[ind];
      ind++;
    }
    datainput += identifier;
  }
  else
  {
    while (ind < line.length() && !checkIfItsBlankSpace(line[ind]))
    {
      datainput += line[ind];
      ind++;
    }
  }

  if (datainput == "")
  {
    status = false;
  }

  while (ind < line.length() && checkIfItsBlankSpace(line[ind]))
  { // Increase ind to pass all whitespace
    ind++;
  }
}

void writeIntoTheFile(ofstream &fileused, string datainput, bool newline = true)
{
  if (newline)
  {
    fileused << datainput << endl;
  }
  else
  {
    fileused << datainput;
  }
}

/*************************** SOME STRING FUNCTIONALITIES*******************************************/

class EvaluateString
{
public:
  int getRes();
  EvaluateString(string datainput);

private:
  string dataStored;
  int ind;
  char peek();
  int factor();
  int num();
  char get();
  int term();
};

EvaluateString::EvaluateString(string datainput)
{
  dataStored = datainput;
  ind = 0;
}

int EvaluateString::getRes()
{
  int result = term();
  while (peek() == '+' || peek() == '-')
  {
    if (get() == '+')
    {
      result += term();
    }
    else
    {
      result -= term();
    }
  }
  return result;
}

int EvaluateString::term()
{
  int result = factor();
  while (peek() == '*' || peek() == '/')
  {
    if (get() == '*')
    {
      result *= factor();
    }
    else
    {
      result /= factor();
    }
  }
  return result;
}

int EvaluateString::factor()
{
  if (peek() >= '0' && peek() <= '9')
  {
    return num();
  }
  else if (peek() == '(')
  {
    get();
    int result = getRes();
    get();
    return result;
  }
  else if (peek() == '-')
  {
    get();
    return -factor();
  }
  return 0;
}

int EvaluateString::num()
{
  int ans = get() - '0';
  while (peek() >= '0' && peek() <= '9')
  {
    ans = 10 * ans + get() - '0';
  }
  return ans;
}

char EvaluateString::get()
{
  return dataStored[ind++];
}

char EvaluateString::peek()
{
  return dataStored[ind];
}
