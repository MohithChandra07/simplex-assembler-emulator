 generateMachineCodeFile();

 cout << "  " << baseFileName << ".mc\n";
 
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
