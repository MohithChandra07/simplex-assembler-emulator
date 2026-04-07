generateSymbolTableFile();
printSymbolTable(cout);
string intToHex(uint32_t value, int width) {
    stringstream ss;
    ss << setfill('0') << setw(width) << hex << uppercase << value;
    return ss.str();
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

void generateSymbolTableFile() {
    string symFileName = baseFileName + ".sym";
    ofstream symFile(symFileName);

    if (!symFile.is_open()) return;

    printSymbolTable(symFile);
    symFile.close();
}
