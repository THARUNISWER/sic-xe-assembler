#include <bits/stdc++.h>
using namespace std;

/**********************************ALL TABLE STRUCTURES********************************************/
struct optab
{
  string opcode;
  string exist;
  int frmt;
  optab()
  {
    opcode = "undefined";
    frmt = 0;
    exist = "no";
  }

  optab(string opc, int format, string doesexist)
  {
    opcode = opc;
    frmt = format;
    exist = doesexist;
  }
};

struct regtab
{
  char regnum;
  string exist;
  regtab()
  {
    regnum = 'F';
    exist = "no";
  }
  regtab(char num, string doesexist)
  {
    regnum = num;
    exist = doesexist;
  }
};

struct labeltab
{
  string address;
  string name;
  int reltve;
  int blocknum;
  string exist;
  labeltab()
  {
    name = "undefined";
    address = "0";
    blocknum = 0;
    exist = "no";
    reltve = 0;
  }
};

struct littab
{
  string value;
  string address;
  string exist;
  int blocknum = 0;
  littab()
  {
    value = "";
    address = "?";
    blocknum = 0;
    exist = "no";
  }
};

struct blocktab
{
  string startAddress;
  string name;
  string LOCCTR;
  int number;
  string exist;
  blocktab()
  {
    name = "undefined";
    startAddress = "?";
    exist = "no";
    number = -1;
    LOCCTR = "0";
  }
};

/**********************CREATING AND LOADING TABLES WITH NECESSARY INFO********************/

typedef map<string, labeltab> SYMBOL_TABLE;
typedef map<string, optab> OPCODE_TABLE;
typedef map<string, regtab> REG_TABLE;
typedef map<string, littab> LIT_TABLE;
typedef map<string, blocktab> BLOCK_TABLE;

SYMBOL_TABLE SYMTAB;
OPCODE_TABLE OPTAB;
REG_TABLE REGTAB;
LIT_TABLE LITTAB;
BLOCK_TABLE BLOCKS;

void loadregtable()
{
  // LOADING ALL RESIGTERS WITH THEIR ASSIGNED REGISTER NUMBERS.
  regtab new_reg1('0', "yes");
  REGTAB["A"] = new_reg1;
  regtab new_reg2('1', "yes");
  REGTAB["X"] = new_reg2;
  regtab new_reg3('2', "yes");
  REGTAB["L"] = new_reg3;
  regtab new_reg4('3', "yes");
  REGTAB["B"] = new_reg4;
  regtab new_reg5('4', "yes");
  REGTAB["S"] = new_reg5;
  regtab new_reg6('5', "yes");
  REGTAB["T"] = new_reg6;
  regtab new_reg7('6', "yes");
  REGTAB["F"] = new_reg7;
  regtab new_reg8('8', "yes");
  REGTAB["PC"] = new_reg8;
  regtab new_reg9('9', "yes");
  REGTAB["SW"] = new_reg9;
}

void loadblocks()
{
  // LOADING BLOCKS
  BLOCKS["DEFAULT"].exist = "yes";
  BLOCKS["DEFAULT"].name = "DEFAULT";
  BLOCKS["DEFAULT"].startAddress = "00000";
  BLOCKS["DEFAULT"].number = 0;
  BLOCKS["DEFAULT"].LOCCTR = "0";
}

void loadoptable()
{
  // LOADING OPCODES OF ALL OPERATIONS USING CONSTRUCTORS
  optab new_info1("18", 3, "yes");
  OPTAB["ADD"] = new_info1;
  optab new_info2("58", 3, "yes");
  OPTAB["ADDF"] = new_info2;
  optab new_info3("90", 2, "yes");
  OPTAB["ADDR"] = new_info3;
  optab new_info4("40", 3, "yes");
  OPTAB["AND"] = new_info4;
  optab new_info5("B4", 2, "yes");
  OPTAB["CLEAR"] = new_info5;
  optab new_info6("28", 3, "yes");
  OPTAB["COMP"] = new_info6;
  optab new_info7("88", 3, "yes");
  OPTAB["COMPF"] = new_info7;
  optab new_info8("A0", 2, "yes");
  OPTAB["COMPR"] = new_info8;
  optab new_info9("24", 3, "yes");
  OPTAB["DIV"] = new_info9;
  optab new_info10("64", 3, "yes");
  OPTAB["DIVF"] = new_info10;
  optab new_info11("9C", 2, "yes");
  OPTAB["DIVR"] = new_info11;
  optab new_info12("C4", 1, "yes");
  OPTAB["FIX"] = new_info12;
  optab new_info13("C0", 1, "yes");
  OPTAB["FLOAT"] = new_info13;
  optab new_info14("F4", 1, "yes");
  OPTAB["HIO"] = new_info14;
  optab new_info15("3C", 3, "yes");
  OPTAB["J"] = new_info15;
  optab new_info16("30", 3, "yes");
  OPTAB["JEQ"] = new_info16;
  optab new_info17("34", 3, "yes");
  OPTAB["JGT"] = new_info17;
  optab new_info18("38", 3, "yes");
  OPTAB["JLT"] = new_info18;
  optab new_info19("48", 3, "yes");
  OPTAB["JSUB"] = new_info19;
  optab new_info20("00", 3, "yes");
  OPTAB["LDA"] = new_info20;
  optab new_info21("68", 3, "yes");
  OPTAB["LDB"] = new_info21;
  optab new_info22("50", 3, "yes");
  OPTAB["LDCH"] = new_info22;
  optab new_info23("70", 3, "yes");
  OPTAB["LDF"] = new_info23;
  optab new_info24("08", 3, "yes");
  OPTAB["LDL"] = new_info24;
  optab new_info25("6C", 3, "yes");
  OPTAB["LDS"] = new_info25;
  optab new_info26("74", 3, "yes");
  OPTAB["LDT"] = new_info26;
  optab new_info27("04", 3, "yes");
  OPTAB["LDX"] = new_info27;
  optab new_info28("D0", 3, "yes");
  OPTAB["LPS"] = new_info28;
  optab new_info29("20", 3, "yes");
  OPTAB["MUL"] = new_info29;
  optab new_info30("60", 3, "yes");
  OPTAB["MULF"] = new_info30;
  optab new_info31("98", 2, "yes");
  OPTAB["MULR"] = new_info31;
  optab new_info32("C8", 1, "yes");
  OPTAB["NORM"] = new_info32;
  optab new_info33("44", 3, "yes");
  OPTAB["OR"] = new_info33;
  optab new_info34("D8", 3, "yes");
  OPTAB["RD"] = new_info34;
  optab new_info35("AC", 2, "yes");
  OPTAB["RMO"] = new_info35;
  optab new_info36("4F", 3, "yes");
  OPTAB["RSUB"] = new_info36;
  optab new_info37("A4", 2, "yes");
  OPTAB["SHIFTL"] = new_info37;
  optab new_info38("A8", 2, "yes");
  OPTAB["SHIFTR"] = new_info38;
  optab new_info39("F0", 1, "yes");
  OPTAB["SIO"] = new_info39;
  optab new_info40("EC", 3, "yes");
  OPTAB["SSK"] = new_info40;
  optab new_info41("0C", 3, "yes");
  OPTAB["STA"] = new_info41;
  optab new_info42("78", 3, "yes");
  OPTAB["STB"] = new_info42;
  optab new_info43("54", 3, "yes");
  OPTAB["STCH"] = new_info43;
  optab new_info44("80", 3, "yes");
  OPTAB["STF"] = new_info44;
  optab new_info45("D4", 3, "yes");
  OPTAB["STI"] = new_info45;
  optab new_info46("14", 3, "yes");
  OPTAB["STL"] = new_info46;
  optab new_info47("7C", 3, "yes");
  OPTAB["STS"] = new_info47;
  optab new_info48("E8", 3, "yes");
  OPTAB["STSW"] = new_info48;
  optab new_info49("84", 3, "yes");
  OPTAB["STT"] = new_info49;
  optab new_info50("10", 3, "yes");
  OPTAB["STX"] = new_info50;
  optab new_info51("1C", 3, "yes");
  OPTAB["SUB"] = new_info51;
  optab new_info52("5C", 3, "yes");
  OPTAB["SUBF"] = new_info52;
  optab new_info53("94", 2, "yes");
  OPTAB["SUBR"] = new_info53;
  optab new_info54("B0", 2, "yes");
  OPTAB["SVC"] = new_info54;
  optab new_info55("E0", 3, "yes");
  OPTAB["TD"] = new_info55;
  optab new_info56("F8", 1, "yes");
  OPTAB["TIO"] = new_info56;
  optab new_info57("2C", 3, "yes");
  OPTAB["TIX"] = new_info57;
  optab new_info58("B8", 2, "yes");
  OPTAB["TIXR"] = new_info58;
  optab new_info59("DC", 3, "yes");
  OPTAB["WD"] = new_info59;
}

void load_all_tables()
{
  // LOADING ALL TABLES.
  loadregtable();
  loadblocks();
  loadoptable();
}
