#include <bits/stdc++.h>
using namespace std;

#define RED "\033[1;31m"
#define RESET "\033[0m"
#define PINK "\033[38;5;205m"

const int MAX_HISTORY_SIZE = 100;

struct ConversionRecord {
    string inputValue;
    string outputValue;
    string sourceBase;
    string targetBase;
};

struct CalculationRecord {
    string firstOperand;
    string secondOperand;
    string firstBase;
    string secondBase;
    string operation;
    string decimalResult;
    string binaryResult;
    string octalResult;
    string hexResult;
};

vector<ConversionRecord> conversionHistory;
vector<CalculationRecord> calculationHistory;

void waitForEnter() {
    cout << "Press Enter to continue...";
    string dummy;
    getline(cin, dummy);
}

void flushInputBuffer() {
    if (cin.fail()) {
        cin.clear();
    }
    string temp;
    getline(cin, temp);
}

void clearConsole() {
    system("cls||clear");
}

void reverseString(string &str) {
    reverse(str.begin(), str.end());
}

string convertIntegerPortion(double number, int base) {
    if (number == 0) {
        return "0";
    }
    
    string result;
    unsigned long long unsignedNum = (number < 0) 
        ? -static_cast<long long>(number) 
        : static_cast<unsigned long long>(number);
    
    while (unsignedNum > 0) {
        int remainder = unsignedNum % base;
        char digit = (remainder < 10) 
            ? char('0' + remainder) 
            : char('A' + (remainder - 10));
        result.push_back(digit);
        unsignedNum /= base;
    }
    
    reverseString(result);
    return result;
}

string convertFractionalPortion(double fraction, int base, int maxDigits = 20) {
    if (fraction <= 0.0) {
        return "";
    }
    
    string result;
    result.push_back('.');
    int digitCount = 0;
    
    while (fraction > 0.0 && digitCount < maxDigits) {
        fraction *= base;
        int digit = static_cast<int>(floor(fraction + 1e-15));
        char digitChar = (digit < 10) 
            ? char('0' + digit) 
            : char('A' + (digit - 10));
        result.push_back(digitChar);
        fraction -= digit;
        digitCount++;
    }
    
    return result;
}

string convertFromDecimalToBase(double decimalValue, int targetBase) {
    if (!isfinite(decimalValue)) {
        return "NaN";
    }
    
    bool isNegative = decimalValue < 0.0;
    if (isNegative) {
        decimalValue = -decimalValue;
    }
    
    double integerPart = floor(decimalValue + 1e-15);
    double fractionalPart = decimalValue - integerPart;
    
    string integerString = convertIntegerPortion(integerPart, targetBase);
    string fractionalString = convertFractionalPortion(fractionalPart, targetBase);
    
    stringstream ss;
    if (isNegative) {
        ss << '-';
    }
    ss << integerString;
    if (!fractionalString.empty()) {
        ss << fractionalString;
    }
    
    return ss.str();
}

double getDigitValue(char character) {
    if (isdigit(static_cast<unsigned char>(character))) {
        return character - '0';
    }
    
    char upperChar = toupper(static_cast<unsigned char>(character));
    if (upperChar >= 'A' && upperChar <= 'F') {
        return 10 + (upperChar - 'A');
    }
    
    return -1.0;
}

double convertFromBaseToDecimal(const string &numberString, int sourceBase) {
    if (sourceBase < 2 || sourceBase > 36) {
        return numeric_limits<double>::quiet_NaN();
    }
    
    if (numberString.empty()) {
        return numeric_limits<double>::quiet_NaN();
    }
    
    bool isNegative = false;
    size_t startIndex = 0;
    
    if (numberString[0] == '+' || numberString[0] == '-') {
        isNegative = (numberString[0] == '-');
        startIndex = 1;
    }
    
    size_t decimalPointPosition = string::npos;
    for (size_t i = startIndex; i < numberString.size(); i++) {
        if (numberString[i] == '.') {
            decimalPointPosition = i;
            break;
        }
    }
    
    if (decimalPointPosition == string::npos) {
        decimalPointPosition = numberString.size();
    }
    
    double result = 0.0;
    
    for (size_t i = startIndex; i < decimalPointPosition; i++) {
        double digitValue = getDigitValue(numberString[i]);
        if (digitValue < 0.0) {
            cerr << RED << "Invalid digit '" << numberString[i] 
                 << "' for base " << sourceBase << RESET << "\n";
            return numeric_limits<double>::quiet_NaN();
        }
        if (static_cast<int>(digitValue) >= sourceBase) {
            cerr << RED << "Digit '" << numberString[i] 
                 << "' invalid for base " << sourceBase << RESET << "\n";
            return numeric_limits<double>::quiet_NaN();
        }
        result = result * sourceBase + digitValue;
    }
    
    double fractionalMultiplier = 1.0 / sourceBase;
    for (size_t i = decimalPointPosition + 1; i < numberString.size(); i++) {
        double digitValue = getDigitValue(numberString[i]);
        if (digitValue < 0.0) {
            cerr << RED << "Invalid digit '" << numberString[i] 
                 << "' for base " << sourceBase << RESET << "\n";
            return numeric_limits<double>::quiet_NaN();
        }
        if (static_cast<int>(digitValue) >= sourceBase) {
            cerr << RED << "Digit '" << numberString[i] 
                 << "' invalid for base " << sourceBase << RESET << "\n";
            return numeric_limits<double>::quiet_NaN();
        }
        result += digitValue * fractionalMultiplier;
        fractionalMultiplier /= sourceBase;
    }
    
    return isNegative ? -result : result;
}

string decimalToBinary(double decimal) {
    return convertFromDecimalToBase(decimal, 2);
}

string decimalToOctal(double decimal) {
    return convertFromDecimalToBase(decimal, 8);
}

string decimalToHex(double decimal) {
    return convertFromDecimalToBase(decimal, 16);
}

double binaryToDecimal(const string &binary) {
    return convertFromBaseToDecimal(binary, 2);
}

double octalToDecimal(const string &octal) {
    return convertFromBaseToDecimal(octal, 8);
}

double hexToDecimal(const string &hex) {
    return convertFromBaseToDecimal(hex, 16);
}

bool isValidDecimalNumber(const string &input) {
    if (input.empty()) {
        return false;
    }
    
    size_t index = 0;
    int decimalPointCount = 0;
    
    if (input[0] == '-' || input[0] == '+') {
        index++;
    }
    
    bool hasDigit = false;
    for (; index < input.size(); index++) {
        char currentChar = input[index];
        if (isdigit(static_cast<unsigned char>(currentChar))) {
            hasDigit = true;
        } else if (currentChar == '.') {
            decimalPointCount++;
            if (decimalPointCount > 1) {
                return false;
            }
        } else {
            return false;
        }
    }
    
    return hasDigit;
}

bool isValidNumberForBase(const string &input, int base) {
    if (input.empty()) {
        cout << "Error: No digits in input!\n";
        return false;
    }
    
    size_t index = 0;
    if (input[0] == '-' || input[0] == '+') {
        index++;
    }
    
    int decimalPointCount = 0;
    bool hasDigit = false;
    
    for (; index < input.size(); index++) {
        char currentChar = toupper(static_cast<unsigned char>(input[index]));
        
        if (currentChar == '.') {
            decimalPointCount++;
            if (decimalPointCount > 1) {
                cout << "Error: Multiple decimal points!\n";
                return false;
            }
            continue;
        }
        
        int digitValue;
        if (isdigit(static_cast<unsigned char>(currentChar))) {
            digitValue = currentChar - '0';
        } else if (currentChar >= 'A' && currentChar <= 'F') {
            digitValue = currentChar - 'A' + 10;
        } else {
            cout << "Error: Invalid character '" << input[index] << "'!\n";
            return false;
        }
        
        if (digitValue >= base) {
            cout << "Error: Digit '" << input[index] 
                 << "' is invalid for base " << base << "!\n";
            return false;
        }
        
        hasDigit = true;
    }
    
    if (!hasDigit) {
        cout << "Error: No digits in input!\n";
        return false;
    }
    
    return true;
}

void saveConversionToHistory(const string &input, const string &output, 
                             const string &fromBase, const string &toBase) {
    if (static_cast<int>(conversionHistory.size()) < MAX_HISTORY_SIZE) {
        conversionHistory.push_back({input, output, fromBase, toBase});
    }
}

void displayConversionHistory() {
    cout << "\n--- Conversion History ---\n";
    if (conversionHistory.empty()) {
        cout << "No history yet.\n";
        return;
    }
    
    for (size_t i = 0; i < conversionHistory.size(); i++) {
        const auto &record = conversionHistory[i];
        cout << (i + 1) << ". " << record.inputValue << " (" << record.sourceBase 
             << ") To " << record.outputValue << " (" << record.targetBase << ")\n";
    }
}

void writeConversionToFile(const string &input, const string &output,
                          const string &fromBase, const string &toBase) {
    fstream file;
    file.open("store.csv", ios::app);
    if (file.is_open()) {
        file << "Conversion: " << fromBase << " (" << input 
             << ") to " << toBase << ": " << output << endl;
        file.close();
    } else {
        cout << "Unable to open history file.\n";
    }
}

void performDecimalToBinaryConversion() {
    cout << "---Decimal to Binary---\n";
    string input;
    cout << "Enter a number: ";
    getline(cin, input);
    
    if (!isValidDecimalNumber(input)) {
        cout << RED << "Invalid input!\n" << RESET;
        return;
    }
    
    double decimalValue;
    try {
        decimalValue = stold(input);
    } catch (...) {
        cout << RED << "Invalid numeric input!\n" << RESET;
        return;
    }
    
    string binaryResult = decimalToBinary(decimalValue);
    cout << "Binary: " << binaryResult << "\n";
    
    saveConversionToHistory(input, binaryResult, "Decimal", "Binary");
    writeConversionToFile(input, binaryResult, "Decimal", "Binary");
}

void performDecimalToOctalConversion() {
    cout << "---Decimal To Octal---\n";
    string input;
    cout << "Enter a number: ";
    getline(cin, input);
    
    if (!isValidDecimalNumber(input)) {
        cout << RED << "Invalid input!\n" << RESET;
        return;
    }
    
    double decimalValue;
    try {
        decimalValue = stold(input);
    } catch (...) {
        cout << RED << "Invalid numeric input!\n" << RESET;
        return;
    }
    
    string octalResult = decimalToOctal(decimalValue);
    cout << "Octal: " << octalResult << "\n";
    
    saveConversionToHistory(input, octalResult, "Decimal", "Octal");
    writeConversionToFile(input, octalResult, "Decimal", "Octal");
}

void performDecimalToHexConversion() {
    cout << "---Decimal To Hexadecimal---\n";
    string input;
    cout << "Enter a number: ";
    getline(cin, input);
    
    if (!isValidDecimalNumber(input)) {
        cout << RED << "Invalid input!\n" << RESET;
        return;
    }
    
    double decimalValue;
    try {
        decimalValue = stold(input);
    } catch (...) {
        cout << RED << "Invalid numeric input!\n" << RESET;
        return;
    }
    
    string hexResult = decimalToHex(decimalValue);
    cout << "Hexadecimal : " << hexResult << "\n";
    
    saveConversionToHistory(input, hexResult, "Decimal", "Hexadecimal");
    writeConversionToFile(input, hexResult, "Decimal", "Hexadecimal");
}

void performBinaryToDecimalConversion() {
    cout << "---Binary to Decimal---\n";
    string input;
    cout << "Enter binary number: ";
    getline(cin, input);
    
    double result = binaryToDecimal(input);
    if (!isnan(result)) {
        stringstream ss;
        ss << setprecision(10) << result;
        string resultString = ss.str();
        cout << "Decimal: " << resultString << "\n";
        saveConversionToHistory(input, resultString, "Binary", "Decimal");
    }
    
    fstream file;
    file.open("store.csv", ios::app);
    if (file.is_open()) {
        stringstream ss;
        ss << setprecision(10) << result;
        file << "Conversion: Binary (" << input << ") to Decimal: " << ss.str() << endl;
        file.close();
    }
}

void performBinaryToOctalConversion() {
    cout << "---Binary to Octal---\n";
    string input;
    cout << "Enter binary number: ";
    getline(cin, input);
    
    double decimalValue = binaryToDecimal(input);
    if (isnan(decimalValue)) {
        return;
    }
    
    string octalResult = decimalToOctal(decimalValue);
    cout << "Octal: " << octalResult << "\n";
    
    saveConversionToHistory(input, octalResult, "Binary", "Octal");
    writeConversionToFile(input, octalResult, "Binary", "Octal");
}

void performBinaryToHexConversion() {
    cout << "---Binary to Hexadecimal---\n";
    string input;
    cout << "Enter binary number: ";
    getline(cin, input);
    
    double decimalValue = binaryToDecimal(input);
    if (isnan(decimalValue)) {
        return;
    }
    
    string hexResult = decimalToHex(decimalValue);
    cout << "Hexadecimal: " << hexResult << "\n";
    
    saveConversionToHistory(input, hexResult, "Binary", "Hexadecimal");
    writeConversionToFile(input, hexResult, "Binary", "Hexadecimal");
}

void performOctalToDecimalConversion() {
    cout << "---Octal to Decimal---\n";
    string input;
    cout << "Enter octal number: ";
    getline(cin, input);
    
    double decimalValue = octalToDecimal(input);
    if (!isnan(decimalValue)) {
        stringstream ss;
        ss << setprecision(10) << decimalValue;
        string resultString = ss.str();
        cout << "Decimal: " << resultString << "\n";
        saveConversionToHistory(input, resultString, "Octal", "Decimal");
    }
    
    fstream file;
    file.open("store.csv", ios::app);
    if (file.is_open()) {
        stringstream ss;
        ss << setprecision(10) << decimalValue;
        file << "Conversion: Octal (" << input << ") to Decimal: " << ss.str() << endl;
        file.close();
    }
}

void performOctalToBinaryConversion() {
    cout << "---Octal to Binary---\n";
    string input;
    cout << "Enter octal number: ";
    getline(cin, input);
    
    double decimalValue = octalToDecimal(input);
    if (isnan(decimalValue)) {
        return;
    }
    
    string binaryResult = decimalToBinary(decimalValue);
    cout << "Binary: " << binaryResult << "\n";
    
    saveConversionToHistory(input, binaryResult, "Octal", "Binary");
    writeConversionToFile(input, binaryResult, "Octal", "Binary");
}

void performOctalToHexConversion() {
    cout << "---Octal to Hexadecimal---\n";
    string input;
    cout << "Enter octal number: ";
    getline(cin, input);
    
    double decimalValue = octalToDecimal(input);
    if (isnan(decimalValue)) {
        return;
    }
    
    string hexResult = decimalToHex(decimalValue);
    cout << "Hexadecimal: " << hexResult << "\n";
    
    saveConversionToHistory(input, hexResult, "Octal", "Hexadecimal");
    writeConversionToFile(input, hexResult, "Octal", "Hexadecimal");
}

void performHexToDecimalConversion() {
    cout << "---Hexadecimal to Decimal---\n";
    string input;
    cout << "Enter hexadecimal number: ";
    getline(cin, input);
    
    double decimalValue = hexToDecimal(input);
    if (!isnan(decimalValue)) {
        stringstream ss;
        ss << setprecision(10) << decimalValue;
        string resultString = ss.str();
        cout << "Decimal: " << resultString << "\n";
        saveConversionToHistory(input, resultString, "Hexadecimal", "Decimal");
    }
    
    fstream file;
    file.open("store.csv", ios::app);
    if (file.is_open()) {
        stringstream ss;
        ss << setprecision(10) << decimalValue;
        file << "Conversion: Hexadecimal (" << input << ") to Decimal: " << ss.str() << endl;
        file.close();
    }
}

void performHexToBinaryConversion() {
    cout << "---Hexadecimal to Binary---\n";
    string input;
    cout << "Enter hexadecimal number: ";
    getline(cin, input);
    
    double decimalValue = hexToDecimal(input);
    if (isnan(decimalValue)) {
        return;
    }
    
    string binaryResult = decimalToBinary(decimalValue);
    cout << "Binary: " << binaryResult << "\n";
    
    saveConversionToHistory(input, binaryResult, "Hexadecimal", "Binary");
    writeConversionToFile(input, binaryResult, "Hexadecimal", "Binary");
}

void performHexToOctalConversion() {
    cout << "---Hexadecimal to Octal---\n";
    string input;
    cout << "Enter hexadecimal number: ";
    getline(cin, input);
    
    double decimalValue = hexToDecimal(input);
    if (isnan(decimalValue)) {
        return;
    }
    
    string octalResult = decimalToOctal(decimalValue);
    cout << "Octal: " << octalResult << "\n";
    
    saveConversionToHistory(input, octalResult, "Hexadecimal", "Octal");
    writeConversionToFile(input, octalResult, "Hexadecimal", "Octal");
}

void saveCalculationToHistory(const string &operand1, const string &operand2,
                              const string &base1, const string &base2,
                              double result, const string &operation) {
    if (static_cast<int>(calculationHistory.size()) >= MAX_HISTORY_SIZE) {
        return;
    }
    
    CalculationRecord record;
    record.firstOperand = operand1;
    record.secondOperand = operand2;
    record.firstBase = base1;
    record.secondBase = base2;
    record.operation = operation;
    
    stringstream ss;
    ss << setprecision(10) << result;
    record.decimalResult = ss.str();
    record.binaryResult = decimalToBinary(result);
    record.octalResult = decimalToOctal(result);
    record.hexResult = decimalToHex(result);
    
    calculationHistory.push_back(move(record));
}

void displayCalculationHistory() {
    cout << "\n--- Calculator History ---\n";
    if (calculationHistory.empty()) {
        cout << "No history yet.\n";
        return;
    }
    
    for (size_t i = 0; i < calculationHistory.size(); i++) {
        const auto &record = calculationHistory[i];
        cout << (i + 1) << ". " << record.firstOperand << " (" << record.firstBase 
             << ") " << record.operation << " " << record.secondOperand 
             << " (" << record.secondBase << ")\n";
        cout << "   Result in Decimal    : " << record.decimalResult << "\n";
        cout << "   Result in Binary     : " << record.binaryResult << "\n";
        cout << "   Result in Octal      : " << record.octalResult << "\n";
        cout << "   Result in Hexadecimal: " << record.hexResult << "\n";
    }
}

bool getUserBaseChoice(int &base, string &baseString) {
    cout << "Enter base (2,8,10,16): ";
    string input;
    getline(cin, input);
    
    try {
        base = stoi(input);
    } catch (...) {
        cout << RED << "Invalid choice. Try again!\n" << RESET;
        return false;
    }
    
    if (base != 2 && base != 8 && base != 10 && base != 16) {
        cout << RED << "Invalid choice. Try again!\n" << RESET;
        return false;
    }
    
    baseString = input;
    return true;
}

void executeArithmeticOperation(const string &operationSymbol) {
    string operationName;
    if (operationSymbol == "+") operationName = "Addition";
    else if (operationSymbol == "-") operationName = "Subtraction";
    else if (operationSymbol == "*") operationName = "Multiplication";
    else operationName = "Division";
    
    cout << "\n--- " << operationName << " Calculator ---\n";
    
    string firstNumber, secondNumber, firstBaseStr, secondBaseStr;
    int firstBase = 0, secondBase = 0;
    
    if (!getUserBaseChoice(firstBase, firstBaseStr)) {
        return;
    }
    
    do {
        cout << "Enter first number: ";
        getline(cin, firstNumber);
    } while (!isValidNumberForBase(firstNumber, firstBase));
    
    if (!getUserBaseChoice(secondBase, secondBaseStr)) {
        return;
    }
    
    do {
        cout << "Enter second number: ";
        getline(cin, secondNumber);
    } while (!isValidNumberForBase(secondNumber, secondBase));
    
    double firstDecimal = 0.0;
    double secondDecimal = 0.0;
    
    if (firstBase == 2) {
        firstDecimal = binaryToDecimal(firstNumber);
    } else if (firstBase == 8) {
        firstDecimal = octalToDecimal(firstNumber);
    } else if (firstBase == 10) {
        try {
            firstDecimal = stold(firstNumber);
        } catch (...) {
            cout << RED << "Invalid number!\n" << RESET;
            return;
        }
    } else if (firstBase == 16) {
        firstDecimal = hexToDecimal(firstNumber);
    }
    
    if (secondBase == 2) {
        secondDecimal = binaryToDecimal(secondNumber);
    } else if (secondBase == 8) {
        secondDecimal = octalToDecimal(secondNumber);
    } else if (secondBase == 10) {
        try {
            secondDecimal = stold(secondNumber);
        } catch (...) {
            cout << RED << "Invalid number!\n" << RESET;
            return;
        }
    } else if (secondBase == 16) {
        secondDecimal = hexToDecimal(secondNumber);
    }
    
    if (isnan(firstDecimal) || isnan(secondDecimal)) {
        return;
    }
    
    double result = 0.0;
    if (operationSymbol == "+") {
        result = firstDecimal + secondDecimal;
    } else if (operationSymbol == "-") {
        result = firstDecimal - secondDecimal;
    } else if (operationSymbol == "*") {
        result = firstDecimal * secondDecimal;
    } else if (operationSymbol == "/") {
        if (fabs(secondDecimal) < 1e-18) {
            cout << "Error: Division by zero!\n";
            return;
        }
        result = firstDecimal / secondDecimal;
    }
    
    cout << "\nResult:\n";
    stringstream ss;
    ss << setprecision(10) << result;
    cout << "Decimal: " << ss.str() << "\n";
    cout << "Binary: " << decimalToBinary(result) << "\n";
    cout << "Octal: " << decimalToOctal(result) << "\n";
    cout << "Hexadecimal: " << decimalToHex(result) << "\n";
    
    saveCalculationToHistory(firstNumber, secondNumber, firstBaseStr, 
                            secondBaseStr, result, operationSymbol);
    
    fstream file;
    file.open("store.csv", ios::app);
    if (file.is_open()) {
        file << "Calculation: " << firstNumber << " (" << firstBaseStr << ") " 
             << operationSymbol << " " << secondNumber << " (" << secondBaseStr << ") = "
             << setprecision(10) << result << " (Decimal), "
             << decimalToBinary(result) << " (Binary), "
             << decimalToOctal(result) << " (Octal), "
             << decimalToHex(result) << " (Hexadecimal)" << endl;
        file.close();
    }
}

void performAddition() {
    executeArithmeticOperation("+");
}

void performSubtraction() {
    executeArithmeticOperation("-");
}

void performMultiplication() {
    executeArithmeticOperation("*");
}

void performDivision() {
    executeArithmeticOperation("/");
}


void showCalculatorMenu() {
    while (true) {
        cout << "\n====== Number Base Calculation Tool ======\n";
        cout << "1.  Addition\n";
        cout << "2.  Subtraction\n";
        cout << "3.  Multiplication\n";
        cout << "4.  Division\n";
        cout << "5.  View History\n";
        cout << "0.  Back to menu \n";
        cout << "========================================\n";
        cout << "Enter your choice: ";
        
        string choiceInput;
        getline(cin, choiceInput);
        int choice = -1;
        
        try {
            choice = stoi(choiceInput);
        } catch (...) {
            cout << RED << "Invalid input! Please enter a number.\n" << RESET;
            continue;
        }
        
        switch(choice) {
            case 1:
                performAddition();
                waitForEnter();
                clearConsole();
                break;
            case 2:
                performSubtraction();
                waitForEnter();
                clearConsole();
                break;
            case 3:
                performMultiplication();
                waitForEnter();
                clearConsole();
                break;
            case 4:
                performDivision();
                waitForEnter();
                clearConsole();
                break;
            case 5:
                displayCalculationHistory();
                waitForEnter();
                clearConsole();
                break;
            case 0:
                clearConsole();
                cout << "Returning to main menu...\n";
                return;
            default:
                cout << RED << "Invalid choice. Please try again.\n" << RESET;
        }
    }
}

void showConversionMenu() {
    while (true) {
        cout << "\n====== Number Base Conversion Tool ======\n";
        cout << "1.  Decimal to Binary\n";
        cout << "2.  Decimal to Octal\n";
        cout << "3.  Decimal to Hexadecimal\n";
        cout << "4.  Binary to Decimal\n";
        cout << "5.  Binary to Octal\n";
        cout << "6.  Binary to Hexadecimal\n";
        cout << "7.  Octal to Decimal\n";
        cout << "8.  Octal to Binary\n";
        cout << "9.  Octal to Hexadecimal\n";
        cout << "10. Hexadecimal to Decimal\n";
        cout << "11. Hexadecimal to Binary\n";
        cout << "12. Hexadecimal to Octal\n";
        cout << "13. Check History \n";
        cout << "0.  Back to menu \n";
        cout << "========================================\n";
        cout << "Enter your choice: ";
        
        string choiceInput;
        getline(cin, choiceInput);
        int choice = -1;
        
        try {
            choice = stoi(choiceInput);
        } catch (...) {
            cout << RED << "Invalid input! Please enter a number.\n" << RESET;
            continue;
        }
        
        switch(choice) {
            case 1:
                performDecimalToBinaryConversion();
                waitForEnter();
                clearConsole();
                break;
            case 2:
                performDecimalToOctalConversion();
                waitForEnter();
                clearConsole();
                break;
            case 3:
                performDecimalToHexConversion();
                waitForEnter();
                clearConsole();
                break;
            case 4:
                performBinaryToDecimalConversion();
                waitForEnter();
                clearConsole();
                break;
            case 5:
                performBinaryToOctalConversion();
                waitForEnter();
                clearConsole();
                break;
            case 6:
                performBinaryToHexConversion();
                waitForEnter();
                clearConsole();
                break;
            case 7:
                performOctalToDecimalConversion();
                waitForEnter();
                clearConsole();
                break;
            case 8:
                performOctalToBinaryConversion();
                waitForEnter();
                clearConsole();
                break;
            case 9:
                performOctalToHexConversion();
                waitForEnter();
                clearConsole();
                break;
            case 10:
                performHexToDecimalConversion();
                waitForEnter();
                clearConsole();
                break;
            case 11:
                performHexToBinaryConversion();
                waitForEnter();
                clearConsole();
                break;
            case 12:
                performHexToOctalConversion();
                waitForEnter();
                clearConsole();
                break;
            case 13:
                displayConversionHistory();
                waitForEnter();
                clearConsole();
                break;
            case 0:
                clearConsole();
                cout << "Returning to main menu...\n";
                return;
            default:
                cout << RED << "Invalid choice. Please try again.\n" << RESET;
        }
    }
}
void hashPassword(string &password) {
    hash<string> hasher;
    size_t hashed = hasher(password);
    password = to_string(hashed);
}
void verifyCredentials(const string &username, const string &password) {
    fstream file;
    file.open("credentials.csv", ios::in);
    if (!file.is_open()) {
        cout << "Unable to open credentials file.\n";
        exit(1);
    }
    
    string line;
    bool found = false;
    while (getline(file, line)) {
        stringstream ss(line);
        string storedUsername, storedPassword;
        if (getline(ss, storedUsername, ',') && getline(ss, storedPassword)) {
            if (storedUsername == username && storedPassword == password) {
                found = true;
                break;
            }
        }
    }
    
    file.close();
    
    if (!found) {
        cout << RED << "Invalid username or password. Exiting the program.\n" << RESET;
        exit(0);
    }

}
void loginUser() {
    try{
        string username, password;
        cout << "Username: ";
        getline(cin, username);
        cout << "Password: ";
        getline(cin, password);
        hashPassword(password);
        verifyCredentials(username, password);
    }catch(...){
        cout << RED << "An error occurred during login. Exiting the program.\n" << RESET;
        exit(1);
    }
}

void displayLoginScreen() {
    clearConsole();
    cout << PINK << "========================================\n" << RESET;
    cout << PINK << "Welcome to Number Base Converter & Calculator Tool      \n" << RESET;
    cout << PINK << "========================================\n\n" << RESET;
    cout << "Please login to continue...\n";
    cout << "-------------------------\n";
    cout << "press 1 to login\n";
    cout<< "press 2 to register\n";
    cout << "press 0 to exit\n";
    cout << "-------------------------\n";
    cout << "Enter your choice: ";
    string choiceInput;
    getline(cin, choiceInput);
    int choice = -1;
    try {
        choice = stoi(choiceInput);
    } catch (...) {
        cout << RED << "Invalid input!\n" << RESET;
        return;
    }
    switch (choice)
    {
    case 0:
        cout << "Exiting the program. Goodbye!\n";
        exit(0);
        break;
    case 1:
        
        loginUser();
        
        break;
    case 2:
        {
            string username, password;
            cout << "Choose a Username: ";
            getline(cin, username);
            cout << "Choose a Password: ";
            getline(cin, password);
            fstream file;
            file.open("credentials.csv", ios::app);
            if (file.is_open()) {
                hashPassword(password);
                file << username << "," << password << endl;
                file.close();
            } else {
                cout << "Unable to open credentials file.\n";
                exit(1);
            }
            cout << "Registration successful! You can now log in.\n";
            loginUser();
        }
        break;
    default:
        cout << RED << "Invalid choice. Exiting the program.\n" << RESET;
        break;
    }
    clearConsole();
}

int main() {
    displayLoginScreen();
    while (true) {
        cout << "\n====== Main Menu ======\n";
        cout << "Enter 1 if you want to convert \n";
        cout << "Enter 2 if you want to calculate \n";
        cout << "Enter 0 if you want to exit the program\n";
        cout << "Please enter your choice : ";
        
        string choiceInput;
        getline(cin, choiceInput);
        int choice = -1;
        
        try {
            choice = stoi(choiceInput);
        } catch (...) {
            cout << RED << "\nInvalid input! Please enter a number.\n" << RESET;
            waitForEnter();
            flushInputBuffer();
            continue;
        }
        
        switch (choice) {
            case 1:
                clearConsole();
                showConversionMenu();
                break;
            case 2:
                clearConsole();
                showCalculatorMenu();
                break;
            case 0:
                displayLoginScreen();
                return 0;
            default:
                cout << RED << "Invalid choice. Try again.\n" << RESET;
        }
    }
    
    return 0;
}