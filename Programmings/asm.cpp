/* 
    Name      : Mohith Chandra Gugulothu 
    Roll      : 2403cs04
    Course    : Computer Architecture CS2206
    Project   : Two Pass Assembler & Emulator for SIMPLEX Instruction Set
    File Name : asm.cpp -- assembler
    Language  : C++
    Compile   : g++ asm.cpp -o asm
    Useage    : ./asm <filename>.asm

    All work presented here is independently developed by the author (Mohtih Chandra Gugulothu)
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iomanip>
#include <cstdint>
using namespace std;

struct InstructionInfo {
    int opcode;
    int operandType;
    string mnemonic;
};

struct SymbolEntry {
    string name;
    int address;
    int defined;
    int used;
};

struct LineInfo {
    int address;
    uint32_t code;
    string original;
    string label;
    string mnemonic;
    string operand;
    int lineNum;
    string errorMsg;
    string warningMsg;
    int hasLabel;
    int isData;
    int isSet;
    int isValid;
    int hasError;
    int hasWarning;
};

class Assembler {
private:
    vector<InstructionInfo> instructionTable;
    map<string, int> instructionMap;
    vector<SymbolEntry> symbolTable;
    map<string, int> symbolMap;
    vector<LineInfo> lines;
    vector<string> errorMessages;
    vector<uint32_t> objectCode;
    int programCounter;
    int lineNumber;
    int errorCount;
    int warningCount;
    string sourceFileName;
    string baseFileName;
    const uint32_t MAX_24BIT = 0xFFFFFF;
    const uint32_t MAX_32BIT = 0xFFFFFFFF;

public:
    Assembler() {
        programCounter = 0;
        lineNumber = 0;
        errorCount = 0;
        warningCount = 0;
        initializeInstructionTable();
    }
    
    void initializeInstructionTable() {
        instructionTable.push_back({0, 1, "ldc"});
        instructionTable.push_back({1, 1, "adc"});
        instructionTable.push_back({2, 2, "ldl"});
        instructionTable.push_back({3, 2, "stl"});
        instructionTable.push_back({4, 2, "ldnl"});
        instructionTable.push_back({5, 2, "stnl"});
        instructionTable.push_back({6, 0, "add"});
        instructionTable.push_back({7, 0, "sub"});
        instructionTable.push_back({8, 0, "shl"});
        instructionTable.push_back({9, 0, "shr"});
        instructionTable.push_back({10, 1, "adj"});
        instructionTable.push_back({11, 0, "a2sp"});
        instructionTable.push_back({12, 0, "sp2a"});
        instructionTable.push_back({13, 2, "call"});
        instructionTable.push_back({14, 0, "return"});
        instructionTable.push_back({15, 2, "brz"});
        instructionTable.push_back({16, 2, "brlz"});
        instructionTable.push_back({17, 2, "br"});
        instructionTable.push_back({18, 0, "HALT"});
        instructionTable.push_back({-1, 1, "data"});
        instructionTable.push_back({-2, 1, "SET"});
        
        for (const auto& instr : instructionTable) {
            instructionMap[instr.mnemonic] = instr.opcode;
        }
    }
    
    void assemble(const string& filename) {
        sourceFileName = filename;
        size_t dotPos = filename.find_last_of('.');
        baseFileName = (dotPos != string::npos) ? filename.substr(0, dotPos) : filename;
        
        cout << "\nAssembling: " << filename << "\n";
        
        pass1();
        pass2();
        generateListingFile();
        generateLogFile();
        generateObjectFile();
            generateMachineCodeFile();
    generateSymbolTableFile();
    printSymbolTable(cout);
        
        cout << "\nOutput files generated:\n";
        cout << "  " << baseFileName << ".lst\n";
        cout << "  " << baseFileName << ".log\n";
        cout << "  " << baseFileName << ".o\n";
          cout << "  " << baseFileName << ".mc\n";
          cout << "  " << baseFileName << ".sym\n";
        cout << "Errors: " << errorCount << ", Warnings: " << warningCount << "\n";
    }
    
private:
    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t");
        return str.substr(first, last - first + 1);
    }
    
    string removeComments(const string& line) {
        size_t commentPos = line.find(';');
        if (commentPos != string::npos) {
            return line.substr(0, commentPos);
        }
        return line;
    }
    
    bool isValidLabel(const string& label) {
        if (label.empty()) return false;
        if (!isalpha(label[0]) && label[0] != '_') return false;
        for (char c : label) {
            if (!isalnum(c) && c != '_') return false;
        }
        return true;
    }
    
    bool isNumber(const string& str, int32_t& value) {
        char* endptr;
        const char* cstr = str.c_str();
        if (str.size() > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
            value = strtol(cstr, &endptr, 16);
            return (*endptr == '\0');
        }
        else if (str.size() > 1 && str[0] == '0' && isdigit(str[1])) {
            value = strtol(cstr, &endptr, 8);
            return (*endptr == '\0');
        }
        else {
            value = strtol(cstr, &endptr, 10);
            return (*endptr == '\0');
        }
    }
    
    int findSymbol(const string& name) {
        auto it = symbolMap.find(name);
        return (it != symbolMap.end()) ? it->second : -1;
    }
    
    void addSymbol(const string& name, int address, int defined) {
        if (symbolMap.find(name) == symbolMap.end()) {
            SymbolEntry entry = {name, address, defined, 0};
            symbolMap[name] = symbolTable.size();
            symbolTable.push_back(entry);
        }
    }
    
    string intToHex(uint32_t value, int width) {
        stringstream ss;
        ss << setfill('0') << setw(width) << hex << uppercase << value;
        return ss.str();
    }
    
    uint32_t encodeInstruction(int opcode, int32_t operand) {
        return ((operand & MAX_24BIT) << 8) | (opcode & 0xFF);
    }

    void printSymbolTable(ostream& out) {
        out << "\nSymbol Table\n";
        out << "===============================\n";
        out << left << setw(20) << "Label"
            << setw(12) << "Address"
            << setw(10) << "Defined"
            << setw(10) << "Used" << "\n";
        out << "===============================\n";

        if (symbolTable.empty()) {
            out << "No symbols defined.\n";
            return;
        }

        for (const auto& symbol : symbolTable) {
            out << left << setw(20) << symbol.name
                << setw(12) << intToHex(symbol.address, 8)
                << setw(10) << symbol.defined
                << setw(10) << symbol.used << "\n";
        }
    }
    
    void pass1() {
        ifstream file(sourceFileName);
        if (!file.is_open()) {
            cout << "Error: Cannot open file\n";
            return;
        }
        
        string line;
        programCounter = 0;
        lineNumber = 0;
        
        while (getline(file, line)) {
            lineNumber++;
            
            LineInfo lineInfo;
            lineInfo.lineNum = lineNumber;
            lineInfo.address = programCounter;
            lineInfo.original = line;
            lineInfo.hasLabel = 0;
            lineInfo.isData = 0;
            lineInfo.isSet = 0;
            lineInfo.isValid = 1;
            lineInfo.hasError = 0;
            lineInfo.hasWarning = 0;
            lineInfo.code = 0;
            
            string cleanLine = removeComments(line);
            cleanLine = trim(cleanLine);
            
            if (cleanLine.empty()) {
                lines.push_back(lineInfo);
                continue;
            }
            
            size_t colonPos = cleanLine.find(':');
            if (colonPos != string::npos) {
                string label = trim(cleanLine.substr(0, colonPos));
                
                if (label.empty()) {
                    lineInfo.hasWarning = 1;
                    lineInfo.warningMsg = "Empty label";
                    warningCount++;
                } else if (!isValidLabel(label)) {
                    lineInfo.hasError = 1;
                    lineInfo.isValid = 0;
                    lineInfo.errorMsg = "Invalid label name: " + label;
                    errorCount++;
                } else {
                    if (findSymbol(label) != -1) {
                        lineInfo.hasError = 1;
                        lineInfo.isValid = 0;
                        lineInfo.errorMsg = "Duplicate label: " + label;
                        errorCount++;
                    } else {
                        addSymbol(label, programCounter, 1);
                        lineInfo.hasLabel = 1;
                        lineInfo.label = label;
                    }
                }
                
                cleanLine = trim(cleanLine.substr(colonPos + 1));
            }
            
            if (!cleanLine.empty()) {
                istringstream iss(cleanLine);
                iss >> lineInfo.mnemonic;
                
                auto it = instructionMap.find(lineInfo.mnemonic);
                if (it == instructionMap.end()) {
                    lineInfo.hasError = 1;
                    lineInfo.isValid = 0;
                    lineInfo.errorMsg = "Invalid mnemonic: " + lineInfo.mnemonic;
                    errorCount++;
                } else {
                    int opcode = it->second;
                    
                    string rest;
                    getline(iss, rest);
                    lineInfo.operand = trim(rest);
                    
                    if (!lineInfo.operand.empty()) {
                        istringstream check(lineInfo.operand);
                        string token;
                        int tokenCount = 0;
                        while (check >> token) tokenCount++;
                        if (tokenCount > 1) {
                            lineInfo.hasError = 1;
                            lineInfo.isValid = 0;
                            lineInfo.errorMsg = "Extra text after operand";
                            errorCount++;
                        }
                    }
                    
                    if (opcode == -1) {
                        lineInfo.isData = 1;
                        if (lineInfo.operand.empty()) {
                            lineInfo.hasError = 1;
                            lineInfo.isValid = 0;
                            lineInfo.errorMsg = "Missing operand for data";
                            errorCount++;
                        }
                        if (!lineInfo.hasLabel) {
                            lineInfo.hasWarning = 1;
                            lineInfo.warningMsg = "Unlabeled data";
                            warningCount++;
                        }
                        programCounter++;
                    }
                    else if (opcode == -2) {
                        lineInfo.isSet = 1;
                        if (!lineInfo.hasLabel) {
                            lineInfo.hasError = 1;
                            lineInfo.isValid = 0;
                            lineInfo.errorMsg = "SET without label";
                            errorCount++;
                        }
                        if (lineInfo.operand.empty()) {
                            lineInfo.hasError = 1;
                            lineInfo.isValid = 0;
                            lineInfo.errorMsg = "SET without value";
                            errorCount++;
                        }
                        /* Resolve SET value immediately in pass1 so that
                           forward and backward references both work correctly */
                        if (lineInfo.hasLabel && !lineInfo.operand.empty()) {
                            int32_t setValue;
                            if (isNumber(lineInfo.operand, setValue)) {
                                int idx = findSymbol(lineInfo.label);
                                if (idx != -1) {
                                    symbolTable[idx].address = setValue;
                                }
                            }
                        }
                    }
                    else {
                        int operandType = 0;
                        for (const auto& instr : instructionTable) {
                            if (instr.opcode == opcode) {
                                operandType = instr.operandType;
                                break;
                            }
                        }
                        
                        if (operandType > 0 && lineInfo.operand.empty()) {
                            lineInfo.hasError = 1;
                            lineInfo.isValid = 0;
                            lineInfo.errorMsg = "Missing operand for " + lineInfo.mnemonic;
                            errorCount++;
                        }
                        else if (operandType == 0 && !lineInfo.operand.empty()) {
                            lineInfo.hasError = 1;
                            lineInfo.isValid = 0;
                            lineInfo.errorMsg = "Unexpected operand for " + lineInfo.mnemonic;
                            errorCount++;
                        }
                        
                        programCounter++;
                    }
                }
            }
            
            lines.push_back(lineInfo);
        }
        
        file.close();
    }
    
    void pass2() {
        objectCode.clear();
        programCounter = 0;
        
        for (auto& line : lines) {
            
            if (line.mnemonic.empty() && !line.isData && !line.isSet) {
                continue;   /* skip – no code, no PC bump */
            }
            
            if (line.isSet) {
                if (line.isValid && !line.operand.empty() && line.hasLabel) {
                    int32_t value;
                    if (isNumber(line.operand, value)) {
                        int idx = findSymbol(line.label);
                        if (idx != -1) {
                            symbolTable[idx].address = value;
                        }
                    }
                }
                continue;
            }
            
            if (line.isData) {
                if (!line.operand.empty() && line.isValid) {
                    int32_t value = 0;
                    if (isNumber(line.operand, value)) {
                        line.code = value & MAX_32BIT;
                        objectCode.push_back(line.code);
                    } else {
                        int idx = findSymbol(line.operand);
                        if (idx != -1) {
                            line.code = symbolTable[idx].address & MAX_32BIT;
                            objectCode.push_back(line.code);
                            symbolTable[idx].used = 1;
                        } else {
                            line.code = 0;
                            objectCode.push_back(0);
                        }
                    }
                }
                programCounter++;
                continue;
            }
            
            auto it = instructionMap.find(line.mnemonic);
            if (it != instructionMap.end() && line.isValid) {
                int opcode = it->second;
                
                int operandType = 0;
                for (const auto& instr : instructionTable) {
                    if (instr.opcode == opcode) {
                        operandType = instr.operandType;
                        break;
                    }
                }
                
                int32_t operandValue = 0;
                
                if (operandType > 0 && !line.operand.empty()) {
                    if (isNumber(line.operand, operandValue)) {
                        // literal number — use as-is
                    } else {
                        int idx = findSymbol(line.operand);
                        if (idx != -1) {
                            operandValue = symbolTable[idx].address;
                            symbolTable[idx].used = 1;
                            if (operandType == 2) {
                                /* branch/call: offset relative to PC+1 */
                                operandValue = operandValue - (programCounter + 1);
                            }
                        } else {
                            line.hasError = 1;
                            line.errorMsg = "Undefined label: " + line.operand;
                        }
                    }
                }
                
                line.code = encodeInstruction(opcode, operandValue);
                if (!line.hasError) {
                    objectCode.push_back(line.code);
                }
                programCounter++;
            } else if (!line.mnemonic.empty()) {
                programCounter++;
            }
        }
    }
    
    void generateListingFile() {
        string listFileName = baseFileName + ".lst";
        ofstream listFile(listFileName);
        
        if (!listFile.is_open()) return;
        
        listFile << "Address  Machine Code  Assembly\n";
        listFile << "================================\n";
        
        int codeIndex = 0;
        for (const auto& line : lines) {
            if (line.original.empty()) {
                listFile << "\n";
                continue;
            }
            
            listFile << intToHex(line.address, 8) << "  ";
            
            if (!line.hasError && !line.isSet && !line.mnemonic.empty() && codeIndex < (int)objectCode.size()) {
                listFile << intToHex(objectCode[codeIndex], 8) << "  ";
                codeIndex++;
            } else {
                listFile << "          ";
            }
            
            if (line.hasLabel && !line.label.empty()) {
                listFile << line.label << ": ";
            }
            if (!line.mnemonic.empty()) {
                listFile << line.mnemonic;
                if (!line.operand.empty()) {
                    listFile << " " << line.operand;
                }
            }
            
            listFile << "\n";
        }
        
        listFile.close();
    }
    
    void generateLogFile() {
        string logFileName = baseFileName + ".log";
        ofstream logFile(logFileName);
        
        if (!logFile.is_open()) return;
        
        logFile << "Address   Machine Code  Assembly" << string(20, ' ') << "Errors/Warnings\n";
        logFile << "=========================================================================\n";
        
        for (const auto& line : lines) {

            /* blank and comment-only lines: print blank line, no address */
            if (line.mnemonic.empty() && !line.isData && !line.isSet
                && !line.hasError && !line.hasLabel) {
                logFile << "\n";
                continue;
            }

            /* address column */
            logFile << intToHex(line.address, 8) << "  ";

            /* machine code column - only for valid non-error lines */
            if (!line.hasError && line.isValid &&
                !line.isSet && !line.mnemonic.empty()) {
                logFile << intToHex(line.code, 8) << "  ";
            } else {
                logFile << "          ";
            }

            /* assembly text */
            if (line.hasLabel && !line.label.empty()) {
                logFile << line.label << ": ";
            }
            if (!line.mnemonic.empty()) {
                logFile << line.mnemonic;
                if (!line.operand.empty()) {
                    logFile << " " << line.operand;
                }
            }

            /* error / warning annotation */
            if (line.hasError) {
                logFile << "  >> ERROR: " << line.errorMsg;
            } else if (line.hasWarning) {
                logFile << "  >> WARNING: " << line.warningMsg;
            }

            logFile << "\n";
        }
        
        logFile.close();
    }
    
    void generateObjectFile() {
        string objFileName = baseFileName + ".o";
        FILE* objFile = fopen(objFileName.c_str(), "wb");
        
        if (!objFile) return;
        
        /* Use line.code directly — avoids codeIndex getting out of sync
           when error lines are skipped. Only emit lines that:
             - have no error
             - are not SET pseudo-ops
             - actually have a mnemonic (real instruction or data)      */
        vector<uint32_t> validCode;
        for (const auto& line : lines) {
            if (!line.hasError && line.isValid &&
                !line.isSet  && !line.mnemonic.empty()) {
                validCode.push_back(line.code);
            }
        }
        
        if (!validCode.empty()) {
            fwrite(validCode.data(), sizeof(uint32_t), validCode.size(), objFile);
        }
        fclose(objFile);
    }

    void generateMachineCodeFile() {
        string machineCodeFileName = baseFileName + ".mc";
        ofstream machineCodeFile(machineCodeFileName);

        if (!machineCodeFile.is_open()) return;

        for (const auto& line : lines) {
            if (!line.hasError && line.isValid &&
                !line.isSet && !line.mnemonic.empty()) {
                machineCodeFile << intToHex(line.code, 8) << "\n";
            }
        }

        machineCodeFile.close();
    }

    void generateSymbolTableFile() {
        string symFileName = baseFileName + ".sym";
        ofstream symFile(symFileName);

        if (!symFile.is_open()) return;

        printSymbolTable(symFile);
        symFile.close();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <file.asm>\n";
        cout << "Output: file.lst, file.log, file.o\n";
        return 1;
    }
    
    string filename = argv[1];
    Assembler assembler;
    assembler.assemble(filename);
    
    return 0;
}
