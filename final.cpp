#include <bits/stdc++.h>
using namespace std;

// ANSI Color codes
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define CYAN "\033[1;36m"
#define YELLOW "\033[1;33m"
#define PINK "\033[38;5;205m"
#define RESET "\033[0m"

const int MAX_HISTORY_SIZE = 100;
const double EPSILON = 1e-15;

// +Define structures for history records
struct ConversionRecord {
    string inputValue;
    string outputValue;
    string sourceBase;
    string targetBase;
    string timestamp;
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
    string timestamp;
};

// +Utility Class
// Se Nita
class Utils {
public:
    //static because we dont need to instantiate Utils
    static void waitForEnter() { 
        cout << "\n" << CYAN << "Press Enter to continue..." << RESET;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
    }
    // Clear console Function
    static void clearConsole() {
            system("cls");
    }
    // tim3estamp function
    static string getCurrentTimestamp() {
        time_t now = time(nullptr);
        char buf[80];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return string(buf);
    }
    // hash password function
    static string hashPassword(const string& password) {
        hash<string> hasher;
        size_t h1 = hasher(password);
        size_t h2 = hasher(password + "salt123");
        return to_string(h1 ^ h2);
    }
};

// +Number Converter Class
// vannydet & chheangsing
class NumberConverter {
private:
    // only digits and letters up to base 36
    static double getDigitValue(char c) {
        if (isdigit(c)) return c - '0';
        char upper = toupper(c);
        if (upper >= 'A' && upper <= 'Z') return 10 + (upper - 'A');
        return -1.0;
    }
    
    // Convert integer part of number to target base
    static string convertIntegerPart(unsigned long long num, int base) {
        if (num == 0) return "0";
        
        string result;
        while (num > 0) {
            int rem = num % base;
            result.push_back(rem < 10 ? '0' + rem : 'A' + (rem - 10));
            num /= base;
        }
        reverse(result.begin(), result.end());
        return result;
    }
    // Convert fractional part of number to target base
    static string convertFractionalPart(double frac, int base, int maxDigits = 20) {
        if (frac <= 0.0) return "";
        
        string result = ".";
        int count = 0;
        
        while (frac > EPSILON && count < maxDigits) {
            frac *= base;
            int digit = static_cast<int>(frac);
            result.push_back(digit < 10 ? '0' + digit : 'A' + (digit - 10));
            frac -= digit;
            count++;
        }
        
        return result;
    }

public:
    // Validate if the input is valid for the base choose
    static bool isValidNumber(const string& str, int base) {
        if (str.empty()) return false;
        
        size_t i = 0;
        if (str[0] == '+' || str[0] == '-') i++;
        
        bool hasDigit = false;
        bool hasDecimal = false;
        
        for (; i < str.length(); i++) {
            if (str[i] == '.') {
                if (hasDecimal) return false;
                hasDecimal = true;
                continue;
            }
            
            double val = getDigitValue(str[i]);
            if (val < 0 || val >= base) return false;
            hasDigit = true;
        }
        
        return hasDigit;
    }
    // Convert to decimal
    static double toDecimal(const string& num, int base) {

        if (!isValidNumber(num, base)) {
            cerr << RED << "Invalid number for base " << base << RESET << "\n";
            return numeric_limits<double>::quiet_NaN();
        }
        
        bool negative = false;
        size_t start = 0;
        
        if (num[0] == '-') {
            negative = true;
            start = 1;
        } else if (num[0] == '+') {
            start = 1;
        }
        
        size_t decPos = num.find('.');
        if (decPos == string::npos) decPos = num.length();
        
        double result = 0.0;
        
        // Integer part
        for (size_t i = start; i < decPos; i++) {
            result = result * base + getDigitValue(num[i]);
        }
        
        // Fractional part
        double frac = 1.0 / base;
        for (size_t i = decPos + 1; i < num.length(); i++) {
            result += getDigitValue(num[i]) * frac;
            frac /= base;
        }
        
        return negative ? -result : result;
    }
    // Convert from decimal to target base
    static string fromDecimal(double num, int base) {
        if (!isfinite(num)) return "NaN";
        
        bool negative = num < 0;
        if (negative) num = -num;
        
        unsigned long long intPart = static_cast<unsigned long long>(num);
        double fracPart = num - intPart;
        
        string result;
        if (negative) result += '-';
        result += convertIntegerPart(intPart, base);
        result += convertFractionalPart(fracPart, base);
        
        return result;
    }
    // Convert between any two bases
    static string convert(const string& num, int fromBase, int toBase) {
        double decimal = toDecimal(num, fromBase);
        if (isnan(decimal)) return "Error";
        return fromDecimal(decimal, toBase);
    }
};

// +History Manager Class
// Chheangsing
class HistoryManager {
private:
     //using vector cuz we need to maintain order
    vector<ConversionRecord> conversions;
    vector<CalculationRecord> calculations;
    string username;
    // trim to limit size 100
    void trimHistory() {
        if (conversions.size() > MAX_HISTORY_SIZE) {
            conversions.erase(conversions.begin());
        }
        if (calculations.size() > MAX_HISTORY_SIZE) {
            calculations.erase(calculations.begin());
        }
    }
public:
    HistoryManager(const string& user) : username(user) {}
   // +Add conversion record 
    void addConversion(const string& input, const string& output, 
                      const string& fromBase, const string& toBase) {
        ConversionRecord rec = {input, output, fromBase, toBase, Utils::getCurrentTimestamp()};
        conversions.push_back(rec);
        trimHistory();
        
        // Save to file
        ofstream file(username + "_history.csv", ios::app);
        if (file.is_open()) {
            file << "CONV," << rec.timestamp << "," << fromBase << "," 
                 << input << "," << toBase << "," << output << "\n";
        }
    }
    // +Add calculation record
    void addCalculation(const string& op1, const string& op2,const string& base1, const string& base2,const string& operation, double result) {
        // Create record and add to history
        CalculationRecord rec;
        rec.firstOperand = op1;
        rec.secondOperand = op2;
        rec.firstBase = base1;
        rec.secondBase = base2;
        rec.operation = operation;
        rec.timestamp = Utils::getCurrentTimestamp(); 
        
        stringstream ss;
        ss << fixed << setprecision(10) << result;
        rec.decimalResult = ss.str();
        rec.binaryResult = NumberConverter::fromDecimal(result, 2);
        rec.octalResult = NumberConverter::fromDecimal(result, 8);
        rec.hexResult = NumberConverter::fromDecimal(result, 16);
        
        calculations.push_back(rec);
        trimHistory();
        
        // Save to file
        ofstream file(username + "_history.csv", ios::app); // Append mode
        if (file.is_open()) {
            file << "CALC," << rec.timestamp << "," << base1 << "," << op1 
                 << "," << operation << "," << base2 << "," << op2 
                 << "," << rec.decimalResult << "," << rec.binaryResult << "," << rec.octalResult << "," << rec.hexResult << "\n";

        }
        file.close();
    }
    // +Display conversion history
    void displayConversionHistory() const {
        cout << "\n" << CYAN << "====== Conversion History ======" << RESET << "\n";
        if (conversions.empty()) {
            cout << YELLOW << "No conversions yet.\n" << RESET;
            return;
        }
        // Display all conversion records
        for (size_t i = 0; i < conversions.size(); i++) {
            const auto& rec = conversions[i];
            cout << (i + 1) << ". [" << rec.timestamp << "] "
                 << rec.inputValue << " {" << rec.sourceBase << "} to "
                 << rec.outputValue << " {" << rec.targetBase << "}\n";
        }
        
    }
    // +Display calculation history
    void displayCalculationHistory() const {
        cout << "\n" << CYAN << "====== Calculation History ======" << RESET << "\n";
        if (calculations.empty()) {
            cout << YELLOW << "No calculations yet.\n" << RESET;
            return;
        }
        // Display all calculation records
        for (size_t i = 0; i < calculations.size(); i++) {
            const auto& rec = calculations[i];
            cout << (i + 1) << ". [" << rec.timestamp << "] "
                 << rec.firstOperand <<"(" << rec.firstBase << ") "
                 << rec.operation << " "
                 << rec.secondOperand << "(" << rec.secondBase << ")\n"
                 << "   Dec: " << rec.decimalResult << "  Bin: " << rec.binaryResult
                 << "  Oct: " << rec.octalResult << "  Hex: " << rec.hexResult << "\n";
        }
    }
};

//+ Authentication Class
// Se Nita
class Authentication {
public:
    // Check if user exists so we dont duplicate
    static bool userExists(const string& username) {
        ifstream file("credentials.csv", ios::in);
        if (!file.is_open()) return false;
        
        string line;
        while (getline(file, line)) {
            size_t pos = line.find(',');
            if (pos != string::npos && line.substr(0, pos) == username) {
                return true;
            }
        }
        file.close();
        return false;
    }
    // Register new user
    static bool registerUser(const string& username, const string& password) {
        if (userExists(username)) {
            cout << RED << "Username already exists!\n" << RESET;
            return false;
        }
        
        ofstream file("credentials.csv", ios::app);
        if (!file.is_open()) {
            cout << RED << "Error: Cannot access credentials file.\n" << RESET;
            return false;
        }
        
        file << username << "," << Utils::hashPassword(password) << "\n";
        cout << GREEN << "Registration successful!\n" << RESET;
        file.close();
        return true;
    }
    // Login user
    static bool login(const string& username, const string& password) {
        ifstream file("credentials.csv");
        if (!file.is_open()) {
            cout << RED << "Error: Cannot access credentials file.\n" << RESET;
            return false;
        }
        string hashedPass = Utils::hashPassword(password);
        string line;
        
        while (getline(file, line)) {
            size_t pos = line.find(','); //size_t is the type unsigned int 
            if (pos != string::npos) {
                string storedUser = line.substr(0, pos);
                string storedPass = line.substr(pos + 1);
                
                if (storedUser == username && storedPass == hashedPass) {
                    return true;
                }
            }
        }
        file.close();
        
        return false;
    }
};


//+ Calculator Class
//  David 
class Calculator {
private:
    HistoryManager& history;
    
    bool getBaseChoice(int& base, string& baseStr) {
        cout << "Enter base (2=Binary, 8=Octal, 10=Decimal, 16=Hex): ";
        string input;
        getline(cin, input);
        
        try {
            base = stoi(input);
        } catch (...) {
            cout << RED << "Invalid input!\n" << RESET;
            return false;
        }
        
        if (base != 2 && base != 8 && base != 10 && base != 16) {
            cout << RED << "Invalid base! Choose 2, 8, 10, or 16.\n" << RESET;
            return false;
        }
        
        baseStr = input;
        return true;
    }

public:

    Calculator(HistoryManager& hist) : history(hist) {} //constructor 
    // Adding operation
    double addingOperation(double a, double b){
        return a + b;
    }
    double subtractingOperation(double a, double b){
        return a - b;
    }
    double multiplyingOperation(double a, double b){
        return a * b;
    }
    double dividingOperation(double a, double b){
        return a / b;
    }
    // Perform arithmetic operation

    void performOperation(char op) {
        string opName;
        switch(op) {
            case '+': 
                opName = "Addition"; 
                break;
            case '-': 
                opName = "Subtraction"; 
                break;
            case '*': 
                opName = "Multiplication"; 
                break;
            case '/': 
                opName = "Division"; 
                break;
        }
        
        cout << "\n" << CYAN << "====== " << opName << " ======" << RESET << "\n";
        
        int base1, base2;
        string base1Str, base2Str, num1, num2;
        
        if (!getBaseChoice(base1, base1Str)) {
            return;
        }
        
        cout << "Enter first number: ";
        getline(cin, num1);
        if (!NumberConverter::isValidNumber(num1, base1)) {
            cout << RED << "Invalid number for base " << base1 << "!\n" << RESET;
            return;
        }
        
        if (!getBaseChoice(base2, base2Str)) {
            return;
        }
        
        cout << "Enter second number: ";
        getline(cin, num2);
        if (!NumberConverter::isValidNumber(num2, base2)) {
            cout << RED << "Invalid number for base " << base2 << "!\n" << RESET;
            return;
        }
        
        double val1 = NumberConverter::toDecimal(num1, base1);
        double val2 = NumberConverter::toDecimal(num2, base2);
        
        if (isnan(val1) || isnan(val2)) return;
        
        double result;
        switch(op) {
            case '+': 
                result = addingOperation(val1, val2 );
                break;
            case '-': 
                result = subtractingOperation( val1, val2);
                break;
            case '*': 
                result = multiplyingOperation( val1, val2);
                break;
            case '/':
                if (abs(val2) < EPSILON) {
                    cout << RED << "Error: Division by zero!\n" << RESET;
                    return;
                }
                result = dividingOperation( val1, val2);
                break;
            default: return;
        }
        
        cout << "\n" << GREEN << "Result:" << RESET << "\n";
        cout << "Decimal:      " << fixed << setprecision(10) << result << "\n";
        cout << "Binary:       " << NumberConverter::fromDecimal(result, 2) << "\n";
        cout << "Octal:        " << NumberConverter::fromDecimal(result, 8) << "\n";
        cout << "Hexadecimal:  " << NumberConverter::fromDecimal(result, 16) << "\n";
        
        history.addCalculation(num1, num2, base1Str, base2Str, string(1, op), result); 
    }
};

// converter UI Class
// norin
class ConverterUI {
private:
    HistoryManager& history;
    
    void performConversion(const string& fromName, const string& toName, int fromBase, int toBase) {
        cout << "\n" << CYAN << "====== " << fromName << " to " << toName 
             << " ======" << RESET << "\n";
        
        cout << "Enter number: ";
        string input;
        getline(cin, input);
        
        if (!NumberConverter::isValidNumber(input, fromBase)) {
            cout << RED << "Invalid number for base " << fromBase << "!\n" << RESET;
            return;
        }
        
        string result = NumberConverter::convert(input, fromBase, toBase);
        if (result == "Error") return;
        
        cout << GREEN << toName << ": " << result << RESET << "\n";
        history.addConversion(input, result, fromName, toName);
    }

public:
    ConverterUI(HistoryManager& hist) : history(hist) {}
    
    void showMenu() {
        while (true) {
            cout << "\n" << PINK << "============================================\n";
            cout << "   Number Base Conversion Tool          \n";
            cout << "============================================" << RESET << "\n";
            cout << " 1.  Decimal to Binary       7.  Octal to Decimal\n";
            cout << " 2.  Decimal to Octal        8.  Octal to Binary\n";
            cout << " 3.  Decimal to Hex          9.  Octal to Hex\n";
            cout << " 4.  Binary to Decimal       10. Hex to Decimal\n";
            cout << " 5.  Binary to Octal         11. Hex to Binary\n";
            cout << " 6.  Binary to Hex           12. Hex to Octal\n";
            cout << " 13. View History           0.  Back\n";
            cout << "-------------------------------------------\n";
            cout << "Choice: ";
            
            string input;
            getline(cin, input);
            int choice;
            
            try {
                choice = stoi(input);
            } catch (...) {
                cout << RED << "Invalid input!\n" << RESET;
                continue;
            }
            
            switch(choice) {
                case 1: 
                    performConversion("Decimal", "Binary", 10, 2); 
                    break;
                case 2: 
                    performConversion("Decimal", "Octal", 10, 8); 
                    break;
                case 3: 
                    performConversion("Decimal", "Hexadecimal", 10, 16); 
                    break;
                case 4: 
                    performConversion("Binary", "Decimal", 2, 10); 
                    break;
                case 5: 
                    performConversion("Binary", "Octal", 2, 8); 
                    break;
                case 6: 
                    performConversion("Binary", "Hexadecimal", 2, 16); 
                    break;
                case 7: 
                    performConversion("Octal", "Decimal", 8, 10); 
                    break;
                case 8: 
                    performConversion("Octal", "Binary", 8, 2); 
                    break;
                case 9: 
                    performConversion("Octal", "Hexadecimal", 8, 16); 
                    break;
                case 10: 
                    performConversion("Hexadecimal", "Decimal", 16, 10); 
                    break;
                case 11: 
                    performConversion("Hexadecimal", "Binary", 16, 2); 
                    break;
                case 12: 
                    performConversion("Hexadecimal", "Octal", 16, 8); 
                    break;
                case 13: 
                    history.displayConversionHistory(); 
                    break;
                case 0: 
                    return;
                default:
                    cout << RED << "Invalid choice!\n" << RESET;
            }
            
            if (choice != 0 && choice != 13){
             Utils::waitForEnter();
            }
        }
    }
};

// calculator UI Class
// norin
class CalculatorUI {
private:
    Calculator& calc;
    HistoryManager& history;

public:
    CalculatorUI(Calculator& calculator, HistoryManager& hist) 
        : calc(calculator), history(hist) {}
    
    void showMenu() {
        while (true) {
            cout << "\n" << PINK << "============================================\n";
            cout << "   Multi-Base Calculator                \n";
            cout << "============================================" << RESET << "\n";
            cout << " 1. Addition\n";
            cout << " 2. Subtraction\n";
            cout << " 3. Multiplication\n";
            cout << " 4. Division\n";
            cout << " 5. View History\n";
            cout << " 0. Back\n";
            cout << "-------------------------------------------\n";
            cout << "Choice: ";
            
            string input;
            getline(cin, input);
            int choice;
            
            try {
                choice = stoi(input);
            } catch (...) {
                cout << RED << "Invalid input!\n" << RESET;
                continue;
            }
            
            switch(choice) {
                case 1: 
                    calc.performOperation('+'); 
                    break;
                case 2: 
                    calc.performOperation('-'); 
                    break;
                case 3: 
                    calc.performOperation('*'); 
                     break;
                case 4: 
                    calc.performOperation('/'); 
                    break;
                case 5: 
                    history.displayCalculationHistory();
                    break;
                case 0: 
                    return;
                default: 
                    cout << RED << "Invalid choice!\n" << RESET;
            }
            
            if (choice >= 1 && choice <= 4) {
                Utils::waitForEnter();
            }
        }
    }
};


int main() {
    string currentUser;
    bool loggedIn = false;
    
    // Login/Registration Loop
    while (!loggedIn) {
        Utils::clearConsole();
        cout << PINK << "============================================\n";
        cout << "  Number Base Converter & Calculator    \n";
        cout << "============================================" << RESET << "\n\n";
        cout << " 1. Login\n";
        cout << " 2. Register\n";
        cout << " 0. Exit\n";
        cout << "-------------------------------------------\n";
        cout << "Choice: ";
        
        string input;
        getline(cin, input);
        int choice;
        
        try {
            choice = stoi(input);
        } catch (...) {
            cout << RED << "Invalid input!\n" << RESET;
            Utils::waitForEnter();
            continue;
        }
        
        string username, password;
        
        if (choice == 1) {
            cout << "\nUsername: ";
            getline(cin, username);
            cout << "Password: ";
            getline(cin, password);
            
            if (Authentication::login(username, password)) {
                currentUser = username;
                loggedIn = true;
                cout << GREEN << "Login successful! Welcome, " << username << "!\n" << RESET;
                Utils::waitForEnter();
            } else {
                cout << RED << "Invalid credentials!\n" << RESET;
                Utils::waitForEnter();
            }
        } else if (choice == 2) {
            cout << "\nChoose username: ";
            getline(cin, username);
            cout << "Choose password: ";
            getline(cin, password);
            
            if (Authentication::registerUser(username, password)) {
                cout << GREEN << "You can now log in with your credentials.\n" << RESET;
                Utils::waitForEnter();
            }

        } else if (choice == 0) {
            cout << "Goodbye!\n";
            return 0;
        } else {
            cout << RED << "Invalid choice!\n" << RESET;
            Utils::waitForEnter();
        }
    }
    
    // Main Program
    Utils::clearConsole();
    HistoryManager history(currentUser);
    Calculator calc(history);
    ConverterUI converterUI(history);
    CalculatorUI calcUI(calc, history);
    
    while (true) {
        cout << "\n" << PINK << "============================================\n";
        cout << "         Main Menu                      \n";
        cout << "============================================" << RESET << "\n";
        cout << " User: " << GREEN << currentUser << RESET << "\n";
        cout << "-------------------------------------------\n";
        cout << " 1. Number Converter\n";
        cout << " 2. Calculator\n";
        cout << " 0. Logout\n";
        cout << "-------------------------------------------\n";
        cout << "Choice: ";
        
        string input;
        getline(cin, input);
        int choice;
        
        try {
            choice = stoi(input);
        } catch (...) {
            cout << RED << "Invalid input!\n" << RESET;
            continue;
        }
        
        if (choice == 1) {
            Utils::clearConsole();
            converterUI.showMenu();
            Utils::clearConsole();
        } else if (choice == 2) {
            Utils::clearConsole();
            calcUI.showMenu();
            Utils::clearConsole();
        } else if (choice == 0) {
            cout << GREEN << "Logging out...\n" << RESET;
            break;
        } else {
            cout << RED << "Invalid choice!\n" << RESET;
        }
    }
    
    return 0;
}