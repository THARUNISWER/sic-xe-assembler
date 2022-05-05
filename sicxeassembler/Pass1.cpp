#include "Functions.cpp"
#include "Tables.cpp"
#include <bits/stdc++.h>
#define endl '\n'
using namespace std;

string fileName, *BLocksNumToName, firstExecSec;
bool errorFlagraised = false;
int programLength;

/******************************************************************************************************/

void LTORG_put(string &prefix_literal, int &lineNumDelta, int lineNum, int &LOCCTR, int &lastDeltaLOCCTR, int currBlockNum)
{
  string address_literal, value_literal;
  prefix_literal = "";
  for (auto const &it : LITTAB)
  {
    address_literal = it.second.address;
    value_literal = it.second.value;
    if (address_literal != "?")
    {
      // PASS
    }
    else
    {
      lineNum += 5;
      lineNumDelta += 5;
      LITTAB[it.first].address = intToHexString(LOCCTR);
      LITTAB[it.first].blocknum = currBlockNum;
      prefix_literal += "\n" + to_string(lineNum) + "\t" + intToHexString(LOCCTR) + "\t" + to_string(currBlockNum) + "\t" + "*" + "\t" + "=" + value_literal + "\t" + " " + "\t" + " ";

      if (value_literal[0] == 'X')
      {
        LOCCTR += (value_literal.length() - 3) / 2;
        lastDeltaLOCCTR += (value_literal.length() - 3) / 2;
      }
      else if (value_literal[0] == 'C')
      {
        LOCCTR += value_literal.length() - 3;
        lastDeltaLOCCTR += value_literal.length() - 3;
      }
    }
  }
}

void evaluate_E(string exprsn, bool &reltve, string &tempOpnd, int lineNum, ofstream &errorFile, bool &errorFlagraised)
{
  string singleOpnd = "?", singleOptr = "?", stringval = "", tempVal = "", writeData = "";
  int lastOperand = 0, lastOperator = 0, numPairs = 0;
  char prevByte = ' ';
  bool illegal_exp = false;

  for (int i = 0; i < exprsn.length();)
  {
    singleOpnd = "";

    prevByte = exprsn[i];
    while ((prevByte != '+' && prevByte != '-' && prevByte != '/' && prevByte != '*') && i < exprsn.length())
    {
      singleOpnd += prevByte;
      prevByte = exprsn[++i];
    }

    if (SYMTAB[singleOpnd].exist == "yes")
    { // Check operand existence
      lastOperand = SYMTAB[singleOpnd].reltve;
      tempVal = to_string(stringHexToInt(SYMTAB[singleOpnd].address));
    }
    else if ((singleOpnd != "" || singleOpnd != "?") && if_all_numbers(singleOpnd))
    {
      lastOperand = 0;
      tempVal = singleOpnd;
    }
    else
    {
      writeData = "Line: " + to_string(lineNum) + " : Can't find symbol. Instead found " + singleOpnd;
      writeIntoTheFile(errorFile, writeData);
      illegal_exp = true;
      break;
    }

    if (lastOperand * lastOperator == 1)
    { // Check exprsns legallity
      writeData = "Line: " + to_string(lineNum) + " : illegal expression";
      writeIntoTheFile(errorFile, writeData);
      errorFlagraised = true;
      illegal_exp = true;
      break;
    }
    else if ((singleOptr == "-" || singleOptr == "+" || singleOptr == "?") && lastOperand == 1)
    {
      if (singleOptr == "-")
      {
        numPairs--;
      }
      else
      {
        numPairs++;
      }
    }

    stringval += tempVal;

    singleOptr = "";
    while (i < exprsn.length() && (prevByte == '+' || prevByte == '-' || prevByte == '/' || prevByte == '*'))
    {
      singleOptr += prevByte;
      prevByte = exprsn[++i];
    }

    if (singleOptr.length() > 1)
    {
      writeData = "Line: " + to_string(lineNum) + " : ILLEGAL OPERATOR IN EXPRESSION. FOUND " + singleOptr;
      writeIntoTheFile(errorFile, writeData);
      errorFlagraised = true;
      illegal_exp = true;
      break;
    }

    if (singleOptr == "*" || singleOptr == "/")
    {
      lastOperator = 1;
    }
    else
    {
      lastOperator = 0;
    }

    stringval += singleOptr;
  }

  if (!illegal_exp)
  {
    if (numPairs == 1)
    { // RELATIVE
      reltve = 1;
      EvaluateString tempOBJ(stringval);
      tempOpnd = intToHexString(tempOBJ.getRes());
    }
    else if (numPairs == 0)
    { // ABSOLUTE
      reltve = 0;
      cout << stringval << endl;
      EvaluateString tempOBJ(stringval);
      tempOpnd = intToHexString(tempOBJ.getRes());
    }
    else
    {
      writeData = "Line: " + to_string(lineNum) + " : ILLEGAL EXPRESSION";
      writeIntoTheFile(errorFile, writeData);
      errorFlagraised = true;
      tempOpnd = "00000";
      reltve = 0;
    }
  }
  else
  {
    tempOpnd = "00000";
    errorFlagraised = true;
    reltve = 0;
  }
}
void pass1()
{
  ifstream sourceFile;
  ofstream intermediateFile, errorFile;

  sourceFile.open(fileName);
  if (!sourceFile)
  {
    cout << "Can't find file : " << fileName << endl;
    exit(1);
  }

  intermediateFile.open("IntermediateFile_of_" + fileName);
  if (!intermediateFile)
  {
    cout << "Unable to open file: IntermediateFile_of_" << fileName << endl;
    exit(1);
  }
  writeIntoTheFile(intermediateFile, "Line\tAddress\tLabel\tOPCODE\tOPERAND\tComment");
  errorFile.open("ErrorFile_of_" + fileName);
  if (!errorFile)
  {
    cout << "Unable to open file: ErrorFile_of_" << fileName << endl;
    exit(1);
  }
  writeIntoTheFile(errorFile, "ERRORS OF PASS1:");

  string fileLine;
  string writeData, writeDataSuffix = "", writeDataPrefix = "";
  int ind = 0;

  string currentBlockName = "DEFAULT";
  int currBlockNum = 0;
  int totalBlocks = 1;

  bool statusCode;
  string label, opcode, operand, comment;
  string tempOpnd;

  int startAddress, LOCCTR, saveLOCCTR, lineNum, lastDeltaLOCCTR, lineNumDelta = 0;
  lineNum = 0;
  lastDeltaLOCCTR = 0;

  getline(sourceFile, fileLine);
  lineNum += 5;
  while (checkIfItsCommentLine(fileLine))
  {
    writeData = to_string(lineNum) + "\t" + fileLine;
    writeIntoTheFile(intermediateFile, writeData);
    getline(sourceFile, fileLine); // read first input line
    lineNum += 5;
    ind = 0;
  }

  readFirstNonBlankSpace(fileLine, ind, statusCode, label);
  readFirstNonBlankSpace(fileLine, ind, statusCode, opcode);

  if (opcode == "START")
  {
    readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
    readFirstNonBlankSpace(fileLine, ind, statusCode, comment, true);
    startAddress = stringHexToInt(operand);
    // cout<<startAddress<<endl;
    // exit(0);
    LOCCTR = startAddress;
    writeData = to_string(lineNum) + "\t" + intToHexString(LOCCTR - lastDeltaLOCCTR) + "\t" + to_string(currBlockNum) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment;
    writeIntoTheFile(intermediateFile, writeData); // Write file to intermediate file

    getline(sourceFile, fileLine); // Read next line
    lineNum += 5;
    ind = 0;
    readFirstNonBlankSpace(fileLine, ind, statusCode, label);  // Parse label
    readFirstNonBlankSpace(fileLine, ind, statusCode, opcode); // Parse OPCODE
  }
  else
  {
    startAddress = 0;
    LOCCTR = 0;
  }
  //*************************************************************
  string currentSecTitle = "DEFAULT";
  int secCntr = 0;
  //**********************************************************************
  while (opcode != "END")
  {
    //****************************************************************

    while (opcode != "END")
    {

      //****************************************************************
      if (!checkIfItsCommentLine(fileLine))
      {
        if (label != "")
        { // Label found
          if (SYMTAB[label].exist == "no")
          {
            SYMTAB[label].name = label;
            SYMTAB[label].address = intToHexString(LOCCTR);
            SYMTAB[label].reltve = 1;
            SYMTAB[label].exist = "yes";
            SYMTAB[label].blocknum = currBlockNum;
          }
          else
          {
            writeData = "Line: " + to_string(lineNum) + " : Duplicate symbol for '" + label + "'. Previously defined at " + SYMTAB[label].address;
            writeIntoTheFile(errorFile, writeData);
            errorFlagraised = true;
          }
        }
        if (OPTAB[getFinalOp(opcode)].exist == "yes")
        { //SEARCHING OPCODE IN OPTAB
          if (OPTAB[getFinalOp(opcode)].frmt == 3)
          {
            LOCCTR += 3;
            lastDeltaLOCCTR += 3;
            if (getFlagFrmt(opcode) == '+')
            {
              LOCCTR += 1;
              lastDeltaLOCCTR += 1;
            }
            if (getFinalOp(opcode) == "RSUB")
            {
              operand = " ";
            }
            else
            {
              readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
              if (operand[operand.length() - 1] == ',')
              {
                readFirstNonBlankSpace(fileLine, ind, statusCode, tempOpnd);
                operand += tempOpnd;
              }
            }

            if (getFlagFrmt(operand) == '=')
            {
              tempOpnd = operand.substr(1, operand.length() - 1);
              if (tempOpnd == "*")
              {
                tempOpnd = "X'" + intToHexString(LOCCTR - lastDeltaLOCCTR, 6) + "'";
              }
              if (LITTAB[tempOpnd].exist == "no")
              {
                LITTAB[tempOpnd].value = tempOpnd;
                LITTAB[tempOpnd].exist = "yes";
                LITTAB[tempOpnd].address = "?";
                LITTAB[tempOpnd].blocknum = -1;
              }
            }
          }
          else if (OPTAB[getFinalOp(opcode)].frmt == 1)
          {
            operand = " ";
            LOCCTR += OPTAB[getFinalOp(opcode)].frmt;
            lastDeltaLOCCTR += OPTAB[getFinalOp(opcode)].frmt;
          }
          else
          {
            LOCCTR += OPTAB[getFinalOp(opcode)].frmt;
            lastDeltaLOCCTR += OPTAB[getFinalOp(opcode)].frmt;
            readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
            if (operand[operand.length() - 1] == ',')
            {
              readFirstNonBlankSpace(fileLine, ind, statusCode, tempOpnd);
              operand += tempOpnd;
            }
          }
        }

        else if (opcode == "WORD")
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
          LOCCTR += 3;
          lastDeltaLOCCTR += 3;
        }
        else if (opcode == "RESW")
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
          LOCCTR += 3 * stoi(operand);
          lastDeltaLOCCTR += 3 * stoi(operand);
        }
        else if (opcode == "RESB")
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
          LOCCTR += stoi(operand);
          lastDeltaLOCCTR += stoi(operand);
        }
        else if (opcode == "BYTE")
        {
          readByteOperand(fileLine, ind, statusCode, operand);
          if (operand[0] == 'X')
          {
            LOCCTR += (operand.length() - 3) / 2;
            lastDeltaLOCCTR += (operand.length() - 3) / 2;
          }
          else if (operand[0] == 'C')
          {
            LOCCTR += operand.length() - 3;
            lastDeltaLOCCTR += operand.length() - 3;
          }
        }
        else if (opcode == "BASE")
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
        }
        else if (opcode == "LTORG")
        {
          operand = " ";
          LTORG_put(writeDataSuffix, lineNumDelta, lineNum, LOCCTR, lastDeltaLOCCTR, currBlockNum);
        }
        else if (opcode == "ORG")
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);

          char prevByte = operand[operand.length() - 1];
          while (prevByte == '+' || prevByte == '-' || prevByte == '/' || prevByte == '*')
          {
            readFirstNonBlankSpace(fileLine, ind, statusCode, tempOpnd);
            operand += tempOpnd;
            prevByte = operand[operand.length() - 1];
          }

          int tempVariable;
          tempVariable = saveLOCCTR;
          saveLOCCTR = LOCCTR;
          LOCCTR = tempVariable;

          if (SYMTAB[operand].exist == "yes")
          {
            LOCCTR = stringHexToInt(SYMTAB[operand].address);
          }
          else
          {
            bool reltve;
            errorFlagraised = false;
            evaluate_E(operand, reltve, tempOpnd, lineNum, errorFile, errorFlagraised);
            if (!errorFlagraised)
            {
              LOCCTR = stringHexToInt(tempOpnd);
            }
            errorFlagraised = false;
          }
        }
        else if (opcode == "USE")
        {

          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
          BLOCKS[currentBlockName].LOCCTR = intToHexString(LOCCTR);

          if (BLOCKS[operand].exist == "no")
          {
            BLOCKS[operand].exist = "yes";
            BLOCKS[operand].name = operand;
            BLOCKS[operand].number = totalBlocks++;
            BLOCKS[operand].LOCCTR = "0";
          }

          currBlockNum = BLOCKS[operand].number;
          currentBlockName = BLOCKS[operand].name;
          LOCCTR = stringHexToInt(BLOCKS[operand].LOCCTR);
        }
        else if (opcode == "EQU")
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
          tempOpnd = "";
          bool reltve;

          if (operand == "*")
          {
            tempOpnd = intToHexString(LOCCTR - lastDeltaLOCCTR, 6);
            reltve = 1;
          }
          else if (if_all_numbers(operand))
          {
            tempOpnd = intToHexString(stoi(operand), 6);
            reltve = 0;
          }
          else
          {
            char prevByte = operand[operand.length() - 1];

            while (prevByte == '+' || prevByte == '-' || prevByte == '/' || prevByte == '*')
            {
              readFirstNonBlankSpace(fileLine, ind, statusCode, tempOpnd);
              operand += tempOpnd;
              prevByte = operand[operand.length() - 1];
            }

           
            evaluate_E(operand, reltve, tempOpnd, lineNum, errorFile, errorFlagraised);
          }

          SYMTAB[label].name = label;
          SYMTAB[label].address = tempOpnd;
          SYMTAB[label].reltve = reltve;
          SYMTAB[label].blocknum = currBlockNum;
          lastDeltaLOCCTR = LOCCTR - stringHexToInt(tempOpnd);
        }
        else
        {
          readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
          writeData = "Line: " + to_string(lineNum) + " : Invalid OPCODE. Found " + opcode;
          writeIntoTheFile(errorFile, writeData);
          errorFlagraised = true;
        }
        readFirstNonBlankSpace(fileLine, ind, statusCode, comment, true);
        if (opcode == "EQU" && SYMTAB[label].reltve == 0)
        {
          writeData = writeDataPrefix + to_string(lineNum) + "\t" + intToHexString(LOCCTR - lastDeltaLOCCTR) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + writeDataSuffix;
        }
        else
        {
          writeData = writeDataPrefix + to_string(lineNum) + "\t" + intToHexString(LOCCTR - lastDeltaLOCCTR) + "\t" + to_string(currBlockNum) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + writeDataSuffix;
        }
        writeDataPrefix = "";
        writeDataSuffix = "";
      }
      else
      {
        writeData = to_string(lineNum) + "\t" + fileLine;
      }
      writeIntoTheFile(intermediateFile, writeData);

      BLOCKS[currentBlockName].LOCCTR = intToHexString(LOCCTR); // UPDATE LCCNTR OF BLOCK AFTER EVERY INSTRUCTN.
      getline(sourceFile, fileLine);                            // READ NEXT LINE
      lineNum += 5 + lineNumDelta;
      lineNumDelta = 0;
      ind = 0;
      lastDeltaLOCCTR = 0;
      readFirstNonBlankSpace(fileLine, ind, statusCode, label);  
      readFirstNonBlankSpace(fileLine, ind, statusCode, opcode); 
    }
    //*****************************************************************************************

    if (opcode != "END")
    {

      if (SYMTAB[label].exist == "no")
      {
        SYMTAB[label].name = label;
        SYMTAB[label].address = intToHexString(LOCCTR);
        SYMTAB[label].reltve = 1;
        SYMTAB[label].exist = "yes";
        SYMTAB[label].blocknum = currBlockNum;
      }

      writeIntoTheFile(intermediateFile, writeDataPrefix + to_string(lineNum) + "\t" + intToHexString(LOCCTR - lastDeltaLOCCTR) + "\t" + to_string(currBlockNum) + "\t" + label + "\t" + opcode);

      getline(sourceFile, fileLine); // Read next line
      lineNum += 5;

      readFirstNonBlankSpace(fileLine, ind, statusCode, label);  // Parse label
      readFirstNonBlankSpace(fileLine, ind, statusCode, opcode); // Parse OPCODE
    }
  }

  if (opcode == "END")
  {
    firstExecSec = SYMTAB[label].address;
    SYMTAB[firstExecSec].name = label;
    SYMTAB[firstExecSec].address = firstExecSec;
  }

  readFirstNonBlankSpace(fileLine, ind, statusCode, operand);
  readFirstNonBlankSpace(fileLine, ind, statusCode, comment, true);

  /*Change to deafult before dumping literals*/
  currentBlockName = "DEFAULT";
  currBlockNum = 0;
  LOCCTR = stringHexToInt(BLOCKS[currentBlockName].LOCCTR);

  LTORG_put(writeDataSuffix, lineNumDelta, lineNum, LOCCTR, lastDeltaLOCCTR, currBlockNum);

  writeData = to_string(lineNum) + "\t" + intToHexString(LOCCTR - lastDeltaLOCCTR) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + writeDataSuffix;
  writeIntoTheFile(intermediateFile, writeData);

  int LocctrArr[totalBlocks];
  BLocksNumToName = new string[totalBlocks];
  for (auto const &it : BLOCKS)
  {
    LocctrArr[it.second.number] = stringHexToInt(it.second.LOCCTR);
    BLocksNumToName[it.second.number] = it.first;
  }

  for (int i = 1; i < totalBlocks; i++)
  {
    LocctrArr[i] += LocctrArr[i - 1];
  }

  for (auto const &it : BLOCKS)
  {
    if (it.second.startAddress == "?")
    {
      BLOCKS[it.first].startAddress = intToHexString(LocctrArr[it.second.number - 1]);
    }
  }

  programLength = LocctrArr[totalBlocks - 1] - startAddress;

  sourceFile.close();
  intermediateFile.close();
  errorFile.close();
}
