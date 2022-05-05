#include <bits/stdc++.h>
#include "Pass1.cpp"
#define endl '\n'
using namespace std;

ifstream intermediateFile;
ofstream errorFile, objectFile, ListingFile, printtab;
string op1, op2, stringadd, label, opcode, operand, comment, objectCode, writeData, currRecord, modificationRecord = "M^", endRecord, currentSecTitle = "DEFAULT";
int lineNum, blocknum, address, startAddress, secCntr = 0, programSecLength = 0, programCntr, BaseRegVal, currTextRecLength;
bool isComment, baseNotUsed;

/************************************************************************************/

string rdUntilTab(string datainput, int &ind)
{
  string temporaryBuffer = "";

  while (ind < datainput.length() && datainput[ind] != '\t')
  {
    temporaryBuffer += datainput[ind];
    ind++;
  }
  ind++;
  if (temporaryBuffer == " ")
  {
    temporaryBuffer = "-1";
  }
  return temporaryBuffer;
}

bool rIntFile(ifstream &readFile, bool &isComment, int &lineNum, int &address, int &blocknum, string &label, string &opcode, string &operand, string &comment)
{
  string fileLine = "", temporaryBuffer = "";
  bool tempStatus;
  int ind = 0;

  if (!getline(readFile, fileLine))
  {
    return false;
  }
  lineNum = stoi(rdUntilTab(fileLine, ind));

  isComment = (fileLine[ind] == '.') ? true : false;
  if (isComment)
  {
    readFirstNonBlankSpace(fileLine, ind, tempStatus, comment, true);
    return true;
  }

  address = stringHexToInt(rdUntilTab(fileLine, ind));
  temporaryBuffer = rdUntilTab(fileLine, ind);

  if (temporaryBuffer == " ")
  {
    blocknum = -1;
  }
  else
  {
    blocknum = stoi(temporaryBuffer);
  }
  label = rdUntilTab(fileLine, ind);
  if (label == "-1")
  {
    label = " ";
  }
  opcode = rdUntilTab(fileLine, ind);
  if (opcode == "BYTE")
  {
    readByteOperand(fileLine, ind, tempStatus, operand);
  }
  else
  {
    operand = rdUntilTab(fileLine, ind);
    if (operand == "-1")
    {
      operand = " ";
    }
  }
  readFirstNonBlankSpace(fileLine, ind, tempStatus, comment, true);
  return true;
}

string ObjcodeFrmt34()
{
  string objcode;
  int halfBytes;
  halfBytes = (getFlagFrmt(opcode) == '+') ? 5 : 3;

  if (getFlagFrmt(operand) == '#')
  { // ITS AN IMMEDIATE
    if (operand.substr(operand.length() - 2, 2) == ",X")
    { 
      writeData = "Line: " + to_string(lineNum) + " INDEX BASED ADDRESSING IS NOT SUPPORTED WITH INDIRECT ADDRESSING";
      writeIntoTheFile(errorFile, writeData);
      objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
      objcode += (halfBytes == 5) ? "100000" : "0000";
      return objcode;
    }

    string tempOpnd = operand.substr(1, operand.length() - 1);

    if (if_all_numbers(tempOpnd) || ((SYMTAB[tempOpnd].exist == "yes") && (SYMTAB[tempOpnd].reltve == 0)))
    { 
      int immediateValue;

      if (if_all_numbers(tempOpnd))
      {
        immediateValue = stoi(tempOpnd);
      }
      else
      {
        immediateValue = stringHexToInt(SYMTAB[tempOpnd].address);
      }

      if (immediateValue >= (1 << 4 * halfBytes))
      {
        writeData = "Line: " + to_string(lineNum) + " IMMEDIATE VALUE EXCEDES FORMAT LIMIT";
        writeIntoTheFile(errorFile, writeData);
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
        objcode += (halfBytes == 5) ? "100000" : "0000";
      }
      else
      {
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
        objcode += (halfBytes == 5) ? '1' : '0';
        objcode += intToHexString(immediateValue, halfBytes);
      }
      return objcode;
    }
    else if (SYMTAB[tempOpnd].exist == "no")
    {

      if (halfBytes == 3)
      {
        writeData = "Line " + to_string(lineNum);
        if (halfBytes == 3)
        {
          writeData += " : INVALID FORMAT FOR EXTERNAL REFERENCE " + tempOpnd;
        }
        else
        {
          writeData += " : SYMBOL NOT FOUND. INSTEAD, FOUND " + tempOpnd;
        }
        writeIntoTheFile(errorFile, writeData);
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
        objcode += (halfBytes == 5) ? "100000" : "0000";
        return objcode;
      }
    }
    else
    {
      int operandAddress = stringHexToInt(SYMTAB[tempOpnd].address) + stringHexToInt(BLOCKS[BLocksNumToName[SYMTAB[tempOpnd].blocknum]].startAddress);

      /*PROCESS IMMEDIATE SINGLE VALUE*/
      if (halfBytes == 5)
      { /*IF FORMAT = 4*/
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
        objcode += '1';
        objcode += intToHexString(operandAddress, halfBytes);

        /*ADDING THE MODIFICATION RECORD*/
        modificationRecord += "M^" + intToHexString(address + 1, 6) + '^';
        modificationRecord += (halfBytes == 5) ? "05" : "03";
        modificationRecord += '\n';

        return objcode;
      }

      /*FORMAT = 3*/
      programCntr = address + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress);
      programCntr += (halfBytes == 5) ? 4 : 3;
      int reltveAddress = operandAddress - programCntr;

      /*PC ADDRESSING*/
      if (reltveAddress >= (-2048) && reltveAddress <= 2047)
      {
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
        objcode += '2';
        objcode += intToHexString(reltveAddress, halfBytes);
        return objcode;
      }

      /*BASE ADDRESSING*/
      if (!baseNotUsed)
      {
        reltveAddress = operandAddress - BaseRegVal;
        if (reltveAddress >= 0 && reltveAddress <= 4095)
        {
          objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
          objcode += '4';
          objcode += intToHexString(reltveAddress, halfBytes);
          return objcode;
        }
      }

      /*DIRECT ADDRESSING(WITHOUT PC OR BASE reltve)*/
      if (operandAddress <= 4095)
      {
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 1, 2);
        objcode += '0';
        objcode += intToHexString(operandAddress, halfBytes);

        //ADD MODIFICATION RECORD HERE
        modificationRecord += "M^" + intToHexString(address + 1 + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress), 6) + '^';
        modificationRecord += (halfBytes == 5) ? "05" : "03";
        modificationRecord += '\n';

        return objcode;
      }
    }
  }
  else if (getFlagFrmt(operand) == '@')
  {
    string tempOpnd = operand.substr(1, operand.length() - 1);
    if (tempOpnd.substr(tempOpnd.length() - 2, 2) == ",X" || SYMTAB[tempOpnd].exist == "no")
    { // ERROR HANDLING FOR INDIRECT ADDRESSING.

      if (halfBytes == 3)
      {
        writeData = "Line " + to_string(lineNum);

        writeData += " : SYMBOL NOT FOUND. INDIRECT ADDRESSING DOESN'T SUPPORT INDEX ADDRESSING ";

        writeIntoTheFile(errorFile, writeData);
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 2, 2);
        objcode += (halfBytes == 5) ? "100000" : "0000";
        return objcode;
      }
    }
    int operandAddress = stringHexToInt(SYMTAB[tempOpnd].address) + stringHexToInt(BLOCKS[BLocksNumToName[SYMTAB[tempOpnd].blocknum]].startAddress);
    programCntr = address + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress);
    programCntr += (halfBytes == 5) ? 4 : 3;

    if (halfBytes == 3)
    {
      int reltveAddress = operandAddress - programCntr;
      if (reltveAddress >= (-2048) && reltveAddress <= 2047)
      {
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 2, 2);
        objcode += '2';
        objcode += intToHexString(reltveAddress, halfBytes);
        return objcode;
      }

      if (!baseNotUsed)
      {
        reltveAddress = operandAddress - BaseRegVal;
        if (reltveAddress >= 0 && reltveAddress <= 4095)
        {
          objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 2, 2);
          objcode += '4';
          objcode += intToHexString(reltveAddress, halfBytes);
          return objcode;
        }
      }

      if (operandAddress <= 4095)
      {
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 2, 2);
        objcode += '0';
        objcode += intToHexString(operandAddress, halfBytes);

        //ADD MODIFICATION RECORD
        modificationRecord += "M^" + intToHexString(address + 1 + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress), 6) + '^';
        modificationRecord += (halfBytes == 5) ? "05" : "03";
        modificationRecord += '\n';

        return objcode;
      }
    }
    else
    { 
      objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 2, 2);
      objcode += '1';
      objcode += intToHexString(operandAddress, halfBytes);

      //ADD MODIFICATION RECORD
      modificationRecord += "M^" + intToHexString(address + 1 + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress), 6) + '^';
      modificationRecord += (halfBytes == 5) ? "05" : "03";
      modificationRecord += '\n';

      return objcode;
    }

    writeData = "Line: " + to_string(lineNum);
    writeData += " CAN'T FIT INTO PC OR BASE REGISTER BASED ADDRESSING.";
    writeIntoTheFile(errorFile, writeData);
    objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 2, 2);
    objcode += (halfBytes == 5) ? "100000" : "0000";

    return objcode;
  }
  else if (getFlagFrmt(operand) == '=')
  { //LITERALS FOUND
    string tempOpnd = operand.substr(1, operand.length() - 1);

    if (tempOpnd == "*")
    {
      tempOpnd = "X'" + intToHexString(address, 6) + "'";
      //ADD MODIFICATION RECORD
      modificationRecord += "M^" + intToHexString(stringHexToInt(LITTAB[tempOpnd].address) + stringHexToInt(BLOCKS[BLocksNumToName[LITTAB[tempOpnd].blocknum]].startAddress), 6) + '^';
      modificationRecord += intToHexString(6, 2);
      modificationRecord += '\n';
    }

    if (LITTAB[tempOpnd].exist == "no")
    {
      writeData = "Line " + to_string(lineNum) + " : SYMBOL NOT FOUND. INSTEAD, FOUND " + tempOpnd;
      writeIntoTheFile(errorFile, writeData);

      objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
      objcode += (halfBytes == 5) ? "000" : "0";
      objcode += "000";
      return objcode;
    }

    int operandAddress = stringHexToInt(LITTAB[tempOpnd].address) + stringHexToInt(BLOCKS[BLocksNumToName[LITTAB[tempOpnd].blocknum]].startAddress);
    programCntr = address + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress);
    programCntr += (halfBytes == 5) ? 4 : 3;

    if (halfBytes == 3)
    {
      int reltveAddress = operandAddress - programCntr;
      if (reltveAddress >= (-2048) && reltveAddress <= 2047)
      {
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
        objcode += '2';
        objcode += intToHexString(reltveAddress, halfBytes);
        return objcode;
      }

      if (!baseNotUsed)
      {
        reltveAddress = operandAddress - BaseRegVal;
        if (reltveAddress >= 0 && reltveAddress <= 4095)
        {
          objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
          objcode += '4';
          objcode += intToHexString(reltveAddress, halfBytes);
          return objcode;
        }
      }

      if (operandAddress <= 4095)
      {
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
        objcode += '0';
        objcode += intToHexString(operandAddress, halfBytes);

        //ADD MODIFICATION RECORD
        modificationRecord += "M^" + intToHexString(address + 1 + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress), 6) + '^';
        modificationRecord += (halfBytes == 5) ? "05" : "03";
        modificationRecord += '\n';

        return objcode;
      }
    }
    else
    { 
      objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
      objcode += '1';
      objcode += intToHexString(operandAddress, halfBytes);

      //ADD MODIFICATION RECORD
      modificationRecord += "M^" + intToHexString(address + 1 + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress), 6) + '^';
      modificationRecord += (halfBytes == 5) ? "05" : "03";
      modificationRecord += '\n';

      return objcode;
    }

    writeData = "Line: " + to_string(lineNum);
    writeData += " CAN'T FIT INTO PC OR BASE REGISTER BASED ADDRESSING.";
    writeIntoTheFile(errorFile, writeData);
    objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
    objcode += (halfBytes == 5) ? "100" : "0";
    objcode += "000";

    return objcode;
  }
  else
  { 
    int xbpe = 0;
    string tempOpnd = operand;
    if (operand.substr(operand.length() - 2, 2) == ",X")
    {
      tempOpnd = operand.substr(0, operand.length() - 2);
      xbpe = 8;
    }

    if (SYMTAB[tempOpnd].exist == "no")
    {
      
      if (halfBytes == 3)
      { 

        writeData = "Line " + to_string(lineNum);

        writeData += " : SYMBOL NOT FOUND. INSTEAD, FOUND " + tempOpnd;

        writeIntoTheFile(errorFile, writeData);
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
        objcode += (halfBytes == 5) ? (intToHexString(xbpe + 1, 1) + "00") : intToHexString(xbpe, 1);
        objcode += "000";
        return objcode;
      }
    }
    else
    {

      int operandAddress = stringHexToInt(SYMTAB[tempOpnd].address) + stringHexToInt(BLOCKS[BLocksNumToName[SYMTAB[tempOpnd].blocknum]].startAddress);
      programCntr = address + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress);
      programCntr += (halfBytes == 5) ? 4 : 3;

      if (halfBytes == 3)
      {
        int reltveAddress = operandAddress - programCntr;
        if (reltveAddress >= (-2048) && reltveAddress <= 2047)
        {
          objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
          objcode += intToHexString(xbpe + 2, 1);
          objcode += intToHexString(reltveAddress, halfBytes);
          return objcode;
        }

        if (!baseNotUsed)
        {
          reltveAddress = operandAddress - BaseRegVal;
          if (reltveAddress >= 0 && reltveAddress <= 4095)
          {
            objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
            objcode += intToHexString(xbpe + 4, 1);
            objcode += intToHexString(reltveAddress, halfBytes);
            return objcode;
          }
        }

        if (operandAddress <= 4095)
        {
          objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
          objcode += intToHexString(xbpe, 1);
          objcode += intToHexString(operandAddress, halfBytes);

          //ADD MODIFICATION RECORD HERE
          modificationRecord += "M^" + intToHexString(address + 1 + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress), 6) + '^';
          modificationRecord += (halfBytes == 5) ? "05" : "03";
          modificationRecord += '\n';

          return objcode;
        }
      }
      else
      { 
        objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
        objcode += intToHexString(xbpe + 1, 1);
        objcode += intToHexString(operandAddress, halfBytes);

        //ADD MODIFICATION RECORD HERE
        modificationRecord += "M^" + intToHexString(address + 1 + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress), 6) + '^';
        modificationRecord += (halfBytes == 5) ? "05" : "03";
        modificationRecord += '\n';

        return objcode;
      }

      writeData = "Line: " + to_string(lineNum);
      writeData += " CAN'T FIT INTO PC OR BASE REGISTER BASED ADDRESSING.";
      writeIntoTheFile(errorFile, writeData);
      objcode = intToHexString(stringHexToInt(OPTAB[getFinalOp(opcode)].opcode) + 3, 2);
      objcode += (halfBytes == 5) ? (intToHexString(xbpe + 1, 1) + "00") : intToHexString(xbpe, 1);
      objcode += "000";

      return objcode;
    }
  }
  return "AB";
}

void writeTextRecord(bool lastRecord = false)
{
  if (lastRecord)
  {
    if (currRecord.length() > 0)
    { //LAST TEXT RECORD
      writeData = intToHexString(currRecord.length() / 2, 2) + '^' + currRecord;
      writeIntoTheFile(objectFile, writeData);
      currRecord = "";
    }
    return;
  }
  if (objectCode != "")
  {
    if (currRecord.length() == 0)
    {
      writeData = "T^" + intToHexString(address + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress), 6) + '^';
      writeIntoTheFile(objectFile, writeData, false);
    }
    // ONJCODE LENGTH > 60
    if ((currRecord + objectCode).length() > 60)
    {
      // CURR RECORD
      writeData = intToHexString(currRecord.length() / 2, 2) + '^' + currRecord;
      writeIntoTheFile(objectFile, writeData);

      // NEW TEXT RECORD
      currRecord = "";
      writeData = "T^" + intToHexString(address + stringHexToInt(BLOCKS[BLocksNumToName[blocknum]].startAddress), 6) + '^';
      writeIntoTheFile(objectFile, writeData, false);
    }

    currRecord += objectCode;
  }
  else
  {
    // ASSEMBLY DIRECTIVES DOESN'T NEED MEMORY ALLOCATION/ADDRESS ALLOCATION
    if (opcode == "START" || opcode == "END" || opcode == "BASE" || opcode == "baseNotUsed" || opcode == "LTORG" || opcode == "ORG" || opcode == "EQU")
    {
      //*****************
    }
    else
    {
      // WRITE CURRENT RECORD, IF CURRENT RECORD EXISTS
      if (currRecord.length() > 0)
      {
        writeData = intToHexString(currRecord.length() / 2, 2) + '^' + currRecord;
        writeIntoTheFile(objectFile, writeData);
      }
      currRecord = "";
    }
  }
}

void writeEndRecord(bool write = true)
{
  if (write)
  {
    if (endRecord.length() > 0)
    {
      writeIntoTheFile(objectFile, endRecord);
    }
    else
    {
      writeEndRecord(false);
    }
  }
  if ((operand == "" || operand == " ") && currentSecTitle == "DEFAULT")
  { 
    endRecord = "E^" + intToHexString(startAddress, 6);
  }
  else if (currentSecTitle != "DEFAULT")
  {
    endRecord = "E";
  }
  else
  { 
    int firstExecutableAddress;

    firstExecutableAddress = stringHexToInt(SYMTAB[firstExecSec].address);

    endRecord = "E^" + intToHexString(firstExecutableAddress, 6) + "\n";
  }
}

void pass2()
{
  string temporaryBuffer;
  intermediateFile.open("IntermediateFile_of_" + fileName); 
  if (!intermediateFile)
  {
    cout << "Unable to open file: IntermediateFile_of_" << fileName << endl;
    exit(1);
  }
  getline(intermediateFile, temporaryBuffer); 

  objectFile.open("ObjectFile_of_" + fileName);
  if (!objectFile)
  {
    cout << "Unable to open file: ObjectFile_of_" << fileName << endl;
    exit(1);
  }

  ListingFile.open("ListingFile_of_" + fileName);
  if (!ListingFile)
  {
    cout << "Unable to open file: ListingFile_of_" << fileName << endl;
    exit(1);
  }
  writeIntoTheFile(ListingFile, "Line\tAddress\tLabel\tOPCODE\tOPERAND\tObjectCode\tComment");

  errorFile.open("ErrorFile_of_" + fileName, fstream::app);
  if (!errorFile)
  {
    cout << "Unable to open file: ErrorFile_of_" << fileName << endl;
    exit(1);
  }
  writeIntoTheFile(errorFile, "\n\nERRORS OF PASS2:");
  
  objectCode = "";
  currTextRecLength = 0;
  currRecord = "";
  modificationRecord = "";
  blocknum = 0;
  baseNotUsed = true;

  rIntFile(intermediateFile, isComment, lineNum, address, blocknum, label, opcode, operand, comment);
  while (isComment)
  { 
    writeData = to_string(lineNum) + "\t" + comment;
    writeIntoTheFile(ListingFile, writeData);
    rIntFile(intermediateFile, isComment, lineNum, address, blocknum, label, opcode, operand, comment);
  }

  if (opcode == "START")
  {
    startAddress = address;
    writeData = to_string(lineNum) + "\t" + intToHexString(address) + "\t" + to_string(blocknum) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
    writeIntoTheFile(ListingFile, writeData);
  }
  else
  {
    label = "";
    startAddress = 0;
    address = 0;
  }
  programSecLength = programLength;

  writeData = "H^" + expandString(label, 6, ' ', true) + '^' + intToHexString(address, 6) + '^' + intToHexString(programSecLength, 6);
  writeIntoTheFile(objectFile, writeData);

  rIntFile(intermediateFile, isComment, lineNum, address, blocknum, label, opcode, operand, comment);
  currTextRecLength = 0;

  while (opcode != "END")
  {
    while (opcode != "END")
    {
      if (!isComment)
      {
        if (OPTAB[getFinalOp(opcode)].exist == "yes")
        {
          if (OPTAB[getFinalOp(opcode)].frmt == 1)
          {
            objectCode = OPTAB[getFinalOp(opcode)].opcode;
          }
          else if (OPTAB[getFinalOp(opcode)].frmt == 2)
          {
            op1 = operand.substr(0, operand.find(','));
            op2 = operand.substr(operand.find(',') + 1, operand.length() - operand.find(',') - 1);

            if (op2 == operand)
            { // If not two operand i.e. a
              if (getFinalOp(opcode) == "SVC")
              {
                objectCode = OPTAB[getFinalOp(opcode)].opcode + intToHexString(stoi(op1), 1) + '0';
              }
              else if (REGTAB[op1].exist == "yes")
              {
                objectCode = OPTAB[getFinalOp(opcode)].opcode + REGTAB[op1].regnum + '0';
              }
              else
              {
                objectCode = getFinalOp(opcode) + '0' + '0';
                writeData = "Line: " + to_string(lineNum) + " INVALID REGISTER NAME";
                writeIntoTheFile(errorFile, writeData);
              }
            }
            else
            { // Two operands i.e. a,b
              if (REGTAB[op1].exist == "no")
              {
                objectCode = OPTAB[getFinalOp(opcode)].opcode + "00";
                writeData = "Line: " + to_string(lineNum) + " INVALID REGISTER NAME";
                writeIntoTheFile(errorFile, writeData);
              }
              else if (getFinalOp(opcode) == "SHIFTR" || getFinalOp(opcode) == "SHIFTL")
              {
                objectCode = OPTAB[getFinalOp(opcode)].opcode + REGTAB[op1].regnum + intToHexString(stoi(op2), 1);
              }
              else if (REGTAB[op2].exist == "no")
              {
                objectCode = OPTAB[getFinalOp(opcode)].opcode + "00";
                writeData = "Line: " + to_string(lineNum) + " INVALID REGISTER NAME";
                writeIntoTheFile(errorFile, writeData);
              }
              else
              {
                objectCode = OPTAB[getFinalOp(opcode)].opcode + REGTAB[op1].regnum + REGTAB[op2].regnum;
              }
            }
          }
          else if (OPTAB[getFinalOp(opcode)].frmt == 3)
          {
            if (getFinalOp(opcode) == "RSUB")
            {
              objectCode = OPTAB[getFinalOp(opcode)].opcode;
              objectCode += (getFlagFrmt(opcode) == '+') ? "000000" : "0000";
            }
            else
            {
              objectCode = ObjcodeFrmt34();
            }
          }
        } // If opcode in optab
        else if (opcode == "BYTE")
        {
          if (operand[0] == 'X')
          {
            objectCode = operand.substr(2, operand.length() - 3);
          }
          else if (operand[0] == 'C')
          {
            objectCode = stringToHexString(operand.substr(2, operand.length() - 3));
          }
        }
        else if (label == "*")
        {
          if (opcode[1] == 'C')
          {
            objectCode = stringToHexString(opcode.substr(3, opcode.length() - 4));
          }
          else if (opcode[1] == 'X')
          {
            objectCode = opcode.substr(3, opcode.length() - 4);
          }
        }
        else if (opcode == "WORD")
        {
          objectCode = intToHexString(stoi(operand), 6);
        }
        else if (opcode == "BASE")
        {
          if (SYMTAB[operand].exist == "yes")
          {
            BaseRegVal = stringHexToInt(SYMTAB[operand].address) + stringHexToInt(BLOCKS[BLocksNumToName[SYMTAB[operand].blocknum]].startAddress);
            baseNotUsed = false;
          }
          else
          {
            writeData = "Line " + to_string(lineNum) + " : SYMBOL NOT FOUND. INSTEAD, FOUND " + operand;
            writeIntoTheFile(errorFile, writeData);
          }
          objectCode = "";
        }
        else if (opcode == "baseNotUsed")
        {
          if (baseNotUsed)
          { // check if assembler was using base addressing
            writeData = "Line " + to_string(lineNum) + ": ASSEMBLER NOT USING BASE ADDRESSING";
            writeIntoTheFile(errorFile, writeData);
          }
          else
          {
            baseNotUsed = true;
          }
          objectCode = "";
        }
        else
        {
          objectCode = "";
        }
        
        writeTextRecord();

        if (blocknum == -1 && address != -1)
        {
          writeData = to_string(lineNum) + "\t" + intToHexString(address) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
        }
        else if (address == -1)
        {

          writeData = to_string(lineNum) + "\t" + " " + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
        }

        else
        {
          writeData = to_string(lineNum) + "\t" + intToHexString(address) + "\t" + to_string(blocknum) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
        }
      } 
      else
      {
        writeData = to_string(lineNum) + "\t" + comment;
      }
      writeIntoTheFile(ListingFile, writeData);                                                           
      rIntFile(intermediateFile, isComment, lineNum, address, blocknum, label, opcode, operand, comment); 
      objectCode = "";
    } 
  }   

    //*****************************************************************************************
    writeTextRecord();
    writeEndRecord(true);
    writeData =  writeData = to_string(lineNum) + "\t" + intToHexString(address) + "\t" + " " + label + "\t" + "END" + "    " + "FIRST";
    writeIntoTheFile(ListingFile,writeData);
    
      while (rIntFile(intermediateFile, isComment, lineNum, address, blocknum, label, opcode, operand, comment))
      {
        if (label == "*")
        {
          if (opcode[1] == 'C')
          {
            objectCode = stringToHexString(opcode.substr(3, opcode.length() - 4));
          }
          else if (opcode[1] == 'X')
          {
            objectCode = opcode.substr(3, opcode.length() - 4);
          }
          writeTextRecord();
        }
        writeData = to_string(lineNum) + "\t" + intToHexString(address) + "\t" + to_string(blocknum) + label + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
        writeIntoTheFile(ListingFile, writeData);
      }
    

    writeTextRecord(true);
    if (!isComment)
    {

      writeIntoTheFile(objectFile, modificationRecord, false); // Write modification record
      writeEndRecord(true);                                    // Write end record
      currentSecTitle = label;
      modificationRecord = "";
    }
    
  }
  //***********************************************************************

int main()
{
  cout << "INPUT FILE NAME:";
  cin >> fileName;

  cout << "\nLoading OPCODE TAB" << endl;
  load_all_tables();

  cout << "\nRUNNING PASS1" << endl;
  cout << "Writing intermediate values to 'IntermediateFile_of_" << fileName << "'" << endl;
  cout << "Writing errors to 'ErrorFile_of_" << fileName << "'" << endl;
  pass1();

  cout << "Writing SYMBOL TABLE into TablesFile_of_" + fileName << endl;
  printtab.open("TablesFile_of_" + fileName);
  writeIntoTheFile(printtab, "SYMBOL TABLE:\n");
  for (auto const &it : SYMTAB)
  {
    stringadd += it.first + ":-\t" + "name:" + it.second.name + "\t|" + "address:" + it.second.address + "\t|" + "reltve:" + intToHexString(it.second.reltve) + " \n";
  }
  writeIntoTheFile(printtab, stringadd);

  stringadd = "";
  cout << "Writing LITERAL TABLE into TablesFile_of_" + fileName << endl;

  writeIntoTheFile(printtab, "LITERAL TABLE:\n");
  for (auto const &it : LITTAB)
  {
    stringadd += it.first + ":-\t" + "value:" + it.second.value + "\t|" + "address:" + it.second.address + " \n";
  }
  writeIntoTheFile(printtab, stringadd);

  cout << "\nRUNNING PASS2" << endl;
  cout << "Writing objectcode in the 'ObjectFile_of_" << fileName << "'" << endl;
  cout << "Listing to 'ListingFile_of_" << fileName << "'\n.\n.\n." << endl;
  pass2();
  cout << "THE OUTPUT FILES ARE READY (INCLUDES OBJECT PROGRAM, TABLES)" << endl;
}
