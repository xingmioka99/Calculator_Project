#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
//History Part
#define RED "\033[1;31m"
#define RESET   "\033[0m"
#define PINK "\033[38;5;205m"
#define MAX_HISTORY 100
typedef struct {
    char input[100];
    char output[100];
    char fromBase[100];
    char toBase[100];
} History;
History history[MAX_HISTORY];
int historyCount = 0;

// Helper functions
void pressEnter() {
    printf("Press Enter to continue...");
    while (getchar() != '\n'); 
}

void clearInputBuffer() {
    while (getchar() != '\n');
}

void clearScreen() {
    system("cls  clear");
}

void addingHistory(char* input, char* output, char* fromBase, char* toBase){
    if (historyCount < MAX_HISTORY) {
        strcpy(history[historyCount].input, input);
        strcpy(history[historyCount].fromBase, fromBase);
        strcpy(history[historyCount].output, output);
        strcpy(history[historyCount].toBase, toBase);
        historyCount++;
    }
}

void checkHistory(){
    printf("\n--- Conversion History ---\n");
    for (int i = 0; i < historyCount; i++) {
        printf("%d. %s (%s) To %s (%s)\n",
               i + 1,
               history[i].input,
               history[i].fromBase,
               history[i].output,
               history[i].toBase);
    }
    if (historyCount == 0) {
        printf("No history yet.\n");
    }
}

void reverseString(char* str, int len) {
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

void convertIntPart(int num, int base, char* result, int* len) {
    *len = 0;
    if (num == 0) {
        result[(*len)++] = '0';
    } else {
        while (num > 0) {
            int rem = num % base;
            result[(*len)++] = (rem < 10) ? rem + '0' : rem - 10 + 'A';
            num /= base;
        }
        reverseString(result, *len);
    }
    result[*len] = '\0';
}

void convertFracPart(float frac, int base, char* result, int* len) {
    *len = 0;
    if (frac > 0) {
        result[(*len)++] = '.';
        int count = 0;
        while (frac > 0 && count < 20) {
            frac *= base;
            int digit = (int)frac;
            result[(*len)++] = (digit < 10) ? digit + '0' : digit - 10 + 'A';
            frac -= digit;
            count++;
        }
    }
    result[*len] = '\0';
}

char* convertDecToBase(float dec, int base) {
    char* result = malloc(140);
    if (result==NULL) return NULL;

    int isNegative = dec < 0;
    if (isNegative) dec = -dec;

    int intPart = (int)dec;
    float fracPart = dec - intPart;

    char intStr[65], fracStr[65];
    int intLen = 0, fracLen = 0;

    convertIntPart(intPart, base, intStr, &intLen);
    convertFracPart(fracPart, base, fracStr, &fracLen);

    snprintf(result, 140, "%s%s%s", isNegative ? "-" : "", intStr, fracStr);
    return result;
}

float convertBaseToDec(char* num, int base) {
    float result = 0.0;
    int isNegative = 0;
    int len = strlen(num);
    int pointIndex = -1;

    int i = 0;
    if (num[0] == '-') {
        isNegative = 1;
        i++;
    }

    // Find decimal point
    for (int j = i; j < len; j++) {
        if (num[j] == '.') {
            pointIndex = j;
            break;
        }
    }

    if (pointIndex == -1) pointIndex = len;

    // Convert integer part
    for (int j = i; j < pointIndex; j++) {
        char c = toupper(num[j]);
        int digit;
        
        if (isdigit(c)) digit = c - '0';
        else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;
        else {
            printf(RED"Invalid digit '%c' for base %d\n"RESET, c, base);
            return NAN;
        }

        if (digit >= base) {
            printf(RED"Digit '%c' invalid for base %d\n"RESET, c, base);
            return NAN;
        }

        result = result * base + digit;
    }

    // Convert fractional part
    float fractionBase = 1.0f / base;
    for (int j = pointIndex + 1; j < len; j++) {
        char c = toupper(num[j]);
        int digit;
        
        if (isdigit(c)) 
        digit = c - '0';
        else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;
        else {
            printf("Invalid digit '%c' for base %d\n", c, base);
            return NAN;
        }

        if (digit >= base) {
            printf("Digit '%c' invalid for base %d\n", c, base);
            return NAN;
        }

        result += digit * fractionBase;
        fractionBase /= base;
    }

    return isNegative ? -result : result;
}

char* calculateDecToBin(float dec) { return convertDecToBase(dec, 2); }
char* calculateDecToOct(float dec) { return convertDecToBase(dec, 8); }
char* calculateDecToHex(float dec) { return convertDecToBase(dec, 16); }

float calculateBinToDec(char* bin) { return convertBaseToDec(bin, 2); }
float calculateOctToDec(char* oct) { return convertBaseToDec(oct, 8); }
float calculateHexToDec(char* hex) { return convertBaseToDec(hex, 16); }

//Convert Part
bool isValidDecimal(const char* str) {
    int i = 0;
    int dotCount = 0;

    if (str[i] == '-' || str[i] == '+') i++; // optional sign

    bool digitFound = false;

    for (; str[i] != '\0'; i++) {
        if (isdigit(str[i])) {
            digitFound = true;
        } else if (str[i] == '.') {
            dotCount++;
            if (dotCount > 1) return false;
        } else {
            return false;
        }
    }
    return digitFound;
}

void decToBin() {
    printf("---Decimal to Binary---\n");
    char input[100];
    printf("Enter a number: ");
    scanf("%s", input);
    clearInputBuffer();

    if (!isValidDecimal(input)) {
        printf(RED"Invalid input!\n"RESET);
        return;
    }

    float dec = atof(input);
    char* binary = calculateDecToBin(dec);
    if (binary != NULL) {
        printf("Binary: %s\n", binary);
        addingHistory(input,binary,"Decimal","Binary");
        free(binary);
    } else {
        printf("Memory allocation failed.\n");
    }
}

void decToOct(){
    printf("---Decimal To Octal---\n");
    char input[100];
    printf("Enter a number: ");
    scanf("%s", input);
    clearInputBuffer();

    if (!isValidDecimal(input)) {
        printf("Invalid input!\n");
        return;
    }

    float dec = atof(input);
    char* oct = calculateDecToOct(dec);

    if (oct != NULL) {
        printf("Octal: %s\n", oct);
        addingHistory(input,oct,"Decimal","Octal");
        free(oct);
    } else {
        printf("Memory allocation failed.\n");
    }
}

void decToHex(){
    printf("---Decimal To Hexadecimal---\n");
    char input[100];
    printf("Enter a number: ");
    scanf("%s", input);
    clearInputBuffer();
    if (!isValidDecimal(input)) {
        printf("Invalid input!\n");
        return;
    }

    float dec = atof(input);
    char* hex = calculateDecToHex(dec);

    if (hex != NULL) {
        printf("Hexadecimal : %s\n", hex);
        addingHistory(input,hex,"Decimal","Hexadecimal");
        free(hex);
    } else {
        printf("Memory allocation failed.\n");
    }
}

void binToDec() {
    printf("---Binary to Decimal---\n");
    char input[128];
    printf("Enter binary number: ");
    scanf("%s", input);
    clearInputBuffer();
    float result = calculateBinToDec(input);
    if (!isnan(result)){
        char resultStr[50];
        sprintf(resultStr, "%.10g", result);
        printf("Decimal: %.10g\n", result);    
        addingHistory(input,resultStr,"Binary","Decimal");
    }
}

void binToOct() {
    printf("---Binary to Octal---\n");
    char input[128];
    printf("Enter binary number: ");
    scanf("%s", input);
    clearInputBuffer();
    float result = calculateBinToDec(input);
    if (isnan(result)) return;
    char* oct = calculateDecToOct(result);
    if (oct != NULL) {
        printf("Octal: %s\n", oct);
        addingHistory(input,oct,"Binary","Octal");
        free(oct);
    } else printf("Memory allocation failed.\n");
}
void binToHex() {
    printf("---Binary to Hexadecimal---\n");
    char input[128];
    printf("Enter binary number: ");
    scanf("%s", input);
    clearInputBuffer();
    float result = calculateBinToDec(input);
    if (isnan(result)) return;
    char* hex = calculateDecToHex(result);
    if (hex != NULL) {
        printf("Hexadecimal: %s\n", hex);
        addingHistory(input,hex,"Binary","Hexadecimal");
        free(hex);
    } else printf("Memory allocation failed.\n");
}

void octToDec() {
    printf("---Octal to Decimal---\n");
    char input[128];
    printf("Enter octal number: ");
    scanf("%s", input);
    clearInputBuffer();
    float result = calculateOctToDec(input);
    if (!isnan(result)){
        char resultStr[50];
        sprintf(resultStr, "%.10g", result);
        addingHistory(input,resultStr,"Octal","Decimal");
        printf("Decimal: %.10g\n", result);
    }
}

void octToBin() {
    printf("---Octal to Binary---\n");
    char input[128];
    printf("Enter octal number: ");
    scanf("%s", input);
    clearInputBuffer();
    float result = calculateOctToDec(input);
    if (isnan(result)) return;
    char* bin = calculateDecToBin(result);
    if (bin != NULL) {
        printf("Binary: %s\n", bin);
        addingHistory(input,bin,"Octal","Binary");
        free(bin);
    } else printf("Memory allocation failed.\n");
}

void octToHex() {
    printf("---Octal to Hexadecimal---\n");
    char input[128];
    printf("Enter octal number: ");
    scanf("%s", input);
    clearInputBuffer();
    float result = calculateOctToDec(input);
    if (isnan(result)) return;
    char* hex = calculateDecToHex(result);
    if (hex != NULL) {
        printf("Hexadecimal: %s\n", hex);
        addingHistory(input,hex,"Octal","Hexadecimal");
        free(hex);
    } else printf("Memory allocation failed.\n");
}

void hexToDec() {
    printf("---Hexadecimal to Decimal---\n");
    char input[128];
    printf("Enter hexadecimal number: ");
    scanf("%s", input);
    clearInputBuffer();
    float result = calculateHexToDec(input);
    if (!isnan(result)){
        char resultStr[50];
        sprintf(resultStr, "%.10g", result);
        printf("Decimal: %.10g\n", result);
        addingHistory(input,resultStr,"Hexadecimal","Decimal");
    }
}

void hexToBin() {
    printf("---Hexadecimal to Binary---\n");
    char input[128];
    printf("Enter hexadecimal number: ");
    scanf("%s", input);
    clearInputBuffer();
    float result = calculateHexToDec(input);
    if (isnan(result)) return;
    char* bin = calculateDecToBin(result);
    if (bin != NULL) {
        printf("Binary: %s\n", bin);
        addingHistory(input,bin,"Hexadecimal","Binary");
        free(bin);
    } else printf("Memory allocation failed.\n");
}

void hexToOct() {
    printf("---Hexadecimal to Octal---\n");
    char input[128];
    printf("Enter hexadecimal number: ");
    scanf("%s", input);
    clearInputBuffer();
    float result = calculateHexToDec(input);
    if (isnan(result)) return;
    char* oct = calculateDecToOct(result);
    if (oct != NULL) {
        printf("Octal: %s\n", oct);
        addingHistory(input,oct,"Hexadecimal","Octal");
        free(oct);
    } else printf("Memory allocation failed.\n");
}

// CALCULATE PART
typedef struct {
    char operand1[100];     // First input number (e.g., "A")
    char operand2[100];     // Second input number (e.g., "B")
    char base1[20];         // Base of operand1 (e.g., "Binary")
    char base2[20];         // Base of operand2 (e.g., "Hexadecimal")
    char operation[10];     // "+", "-", "*", "/"
    char resultDec[100];    // Decimal result
    char resultBin[100];    // Binary result
    char resultOct[100];    // Octal result
    char resultHex[100];    // Hexadecimal result
} HistoryCal;
HistoryCal hisCal[MAX_HISTORY];
int hisCountCal = 0;

void addToCalculatorHistory(char* num1Str, char* num2Str, char* base1Str, char* base2Str, float result, char* operation) {
    if (hisCountCal >= MAX_HISTORY) return;
    strcpy(hisCal[hisCountCal].operand1, num1Str);
    strcpy(hisCal[hisCountCal].operand2, num2Str);
    strcpy(hisCal[hisCountCal].base1, base1Str);
    strcpy(hisCal[hisCountCal].base2, base2Str);
    strcpy(hisCal[hisCountCal].operation, operation);

    sprintf(hisCal[hisCountCal].resultDec, "%.10g", result);

    char* bin = calculateDecToBin(result);
    char* oct = calculateDecToOct(result);
    char* hex = calculateDecToHex(result);

    if (bin) {
        strcpy(hisCal[hisCountCal].resultBin, bin);
        free(bin);
    }
    if (oct) {
        strcpy(hisCal[hisCountCal].resultOct, oct);
        free(oct);
    }
    if (hex) {
        strcpy(hisCal[hisCountCal].resultHex, hex);
        free(hex);
    }
    hisCountCal++;
}

void checkHistoryCalculation() {
    printf("\n--- Calculator History ---\n");
    for (int i = 0; i < hisCountCal; i++) {
        printf("%d. %s (%s) %s %s (%s)\n",
            i + 1,
            hisCal[i].operand1, hisCal[i].base1,
            hisCal[i].operation,
            hisCal[i].operand2, hisCal[i].base2
        );

        printf("   Result in Decimal    : %s\n", hisCal[i].resultDec);
        printf("   Result in Binary     : %s\n", hisCal[i].resultBin);
        printf("   Result in Octal      : %s\n", hisCal[i].resultOct);
        printf("   Result in Hexadecimal: %s\n", hisCal[i].resultHex);
    }

    if (hisCountCal == 0) {
        printf("No history yet.\n");
    }
}

int isValidInput(char* input, int base) {
    int i = 0;
    int dotCount = 0;
    int hasDigit = 0;

    if (input[i] == '-' || input[i] == '+') i++; // handle sign

    for (; input[i] != '\0'; i++) {
        char c = toupper(input[i]);

        if (c == '.') {
            dotCount++;
            if (dotCount > 1) {
                printf("Error: Multiple decimal points!\n");
                return 0;
            }
            continue;
        }

        int digit;
        if (isdigit(c)) {
            digit = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            digit = c - 'A' + 10;
        } else {
            printf("Error: Invalid character '%c'!\n", c);
            return 0;
        }

        if (digit >= base) {
            printf("Error: Digit '%c' is invalid for base %d!\n", c, base);
            return 0;
        }
        hasDigit = 1;
    }

    if (!hasDigit) {
        printf("Error: No digits in input!\n");
        return 0;
    }

    return 1;
}
void addNumbers() {
    printf("\n--- Addition Calculator ---\n");   
    // Get first number and base
    char num1Str[100], base1Str[10];
    printf("Enter base of first number (2,8,10,16): ");
    scanf("%s", base1Str);
    clearInputBuffer ();
    int base1 = atoi(base1Str);
    if(base1 != 2 && base1 != 8 && base1 != 10 && base1 != 16){
        printf("Invalid choice. Try again!");
        return;
    }
    do{
        printf("Enter first number: ");
        scanf("%s", num1Str);
        clearInputBuffer ();
    }
    while(!isValidInput(num1Str,base1));
    
    // Get second number and base
    char num2Str[100], base2Str[10];
    printf("Enter base of second number (2,8,10,16): ");
    scanf("%s", base2Str);
    clearInputBuffer();
    int base2 = atoi(base2Str);
    if(base2 != 2 && base2 != 8 && base2 != 10 && base2 != 16){
        printf("Invalid choice. Try again!");
        return;
    }
    do{
        printf("Enter second number: ");
        scanf("%s", num2Str);
        clearInputBuffer();
    }
    while(!isValidInput(num2Str,base2));
    // Convert both numbers to decimal
    float dec1, dec2;
    switch(base1) {
        case 2: dec1 = calculateBinToDec(num1Str); break;
        case 8: dec1 = calculateOctToDec(num1Str); break;
        case 10: dec1 = atof(num1Str); break;
        case 16: dec1 = calculateHexToDec(num1Str); break;
        default: 
            return;
    }
    switch(base2) {
        case 2: dec2 = calculateBinToDec(num2Str); break;
        case 8: dec2 = calculateOctToDec(num2Str); break;
        case 10: dec2 = atof(num2Str); break;
        case 16: dec2 = calculateHexToDec(num2Str); break;
        default: 
            return;
    }
    // Perform addition
    float result = dec1 + dec2;
    
    // Display result in all bases
    printf("\nResult:\n");
    printf("Decimal: %.10g\n", result);
    char* bin = calculateDecToBin(result);
    if (bin) {
        printf("Binary: %s\n", bin);
    }
    char* oct = calculateDecToOct(result);
    if (oct) {
        printf("Octal: %s\n", oct);
    }
    
    char* hex = calculateDecToHex(result);
    if (hex) {
        printf("Hexadecimal: %s\n", hex);
    }
    addToCalculatorHistory(num1Str, num2Str, base1Str, base2Str, result, "+");

    // Free memory
    if (bin) free(bin);
    if (oct) free(oct);
    if (hex) free(hex);
}
void subtractNumbers() {
    printf("\n--- Subtraction Calculator ---\n");
    
    // Get first number and base
    char num1Str[100], base1Str[10];
    printf("Enter base of first number (2,8,10,16): ");
    scanf("%s", base1Str);
    clearInputBuffer();
    int base1 = atoi(base1Str);
    if(base1 != 2 && base1 != 8 && base1 != 10 && base1 != 16){
        printf("Invalid choice. Try again!");
        return;
    }
    do{
        printf("Enter first number: ");
        scanf("%s", num1Str);
        clearInputBuffer();
    }
    while(!isValidInput(num1Str,base1));
    
    // Get second number and base
    char num2Str[100], base2Str[10];
    printf("Enter base of second number (2,8,10,16): ");
    scanf("%s", base2Str);
    clearInputBuffer();
    int base2 = atoi(base2Str);
    if(base2 != 2 && base2 != 8 && base2 != 10 && base2 != 16){
        printf("Invalid choice. Try again!");
        return;
    }
    do{
        printf("Enter second number: ");
        scanf("%s", num2Str);
        clearInputBuffer();
    }
    while(!isValidInput(num2Str,base2));
    // Convert both numbers to decimal
    float dec1, dec2;
    switch(base1) {
        case 2: dec1 = calculateBinToDec(num1Str); break;
        case 8: dec1 = calculateOctToDec(num1Str); break;
        case 10: dec1 = atof(num1Str); break;
        case 16: dec1 = calculateHexToDec(num1Str); break;
        default: 
            return;
    }
    
    switch(base2) {
        case 2: dec2 = calculateBinToDec(num2Str); break;
        case 8: dec2 = calculateOctToDec(num2Str); break;
        case 10: dec2 = atof(num2Str); break;
        case 16: dec2 = calculateHexToDec(num2Str); break;
        default: 
            return;
    }
    
    // Perform subtraction
    float result = dec1 - dec2;
    
    // Display result in all bases
    printf("\nResult:\n");
    printf("Decimal: %.10g\n", result);
    
    char* bin = calculateDecToBin(result);
    if (bin) {
        printf("Binary: %s\n", bin);
    }
    
    char* oct = calculateDecToOct(result);
    if (oct) {
        printf("Octal: %s\n", oct);
    }
    
    char* hex = calculateDecToHex(result);
    if (hex) {
        printf("Hexadecimal: %s\n", hex);
    }
    addToCalculatorHistory(num1Str, num2Str, base1Str, base2Str, result, "-");

    // Free memory
    if (bin) free(bin);
    if (oct) free(oct);
    if (hex) free(hex);

}
void multiplyNumbers() {
    printf("\n--- Multiplication Calculator ---\n");
    
    // Get first number and base
    char num1Str[100], base1Str[10];
    printf("Enter base of first number (2,8,10,16): ");
    scanf("%s", base1Str);
    clearInputBuffer();
    int base1 = atoi(base1Str);
    if(base1 != 2 && base1 != 8 && base1 != 10 && base1 != 16){
        printf("Invalid choice. Try again!");
        return;
    }
    do{
        printf("Enter first number: ");
        scanf("%s", num1Str);
        clearInputBuffer();
    }
    while(!isValidInput(num1Str,base1));
    
    // Get second number and base
    char num2Str[100], base2Str[10];
    printf("Enter base of second number (2,8,10,16): ");
    scanf("%s", base2Str);
    clearInputBuffer();
    int base2 = atoi(base2Str);
    if(base2 != 2 && base2 != 8 && base2 != 10 && base2 != 16){
        printf("Invalid choice. Try again!");
        return;
    }
    do{
        printf("Enter second number: ");
        scanf("%s", num2Str);
        clearInputBuffer();
    }
    while(!isValidInput(num2Str,base2));
    // Convert both numbers to decimal
    float dec1, dec2;
    switch(base1) {
        case 2: dec1 = calculateBinToDec(num1Str); break;
        case 8: dec1 = calculateOctToDec(num1Str); break;
        case 10: dec1 = atof(num1Str); break;
        case 16: dec1 = calculateHexToDec(num1Str); break;
        default: 
            return;
    }
    
    switch(base2) {
        case 2: dec2 = calculateBinToDec(num2Str); break;
        case 8: dec2 = calculateOctToDec(num2Str); break;
        case 10: dec2 = atof(num2Str); break;
        case 16: dec2 = calculateHexToDec(num2Str); break;
        default: 
            return;
    }
    
    // Perform multiplication
    float result = dec1 * dec2;
    
    // Display result in all bases
    printf("\nResult:\n");
    printf("Decimal: %.10g\n", result);
    
    char* bin = calculateDecToBin(result);
    if (bin) {
        printf("Binary: %s\n", bin);
    }
    
    char* oct = calculateDecToOct(result);
    if (oct) {
        printf("Octal: %s\n", oct);
    }
    
    char* hex = calculateDecToHex(result);
    if (hex) {
        printf("Hexadecimal: %s\n", hex);
    }
    addToCalculatorHistory(num1Str, num2Str, base1Str, base2Str, result, "*");

    // Free memory
    if (bin) free(bin);
    if (oct) free(oct);
    if (hex) free(hex);
}
void divideNumbers() {
    printf("\n--- Division Calculator ---\n");
    
    // Get first number and base
    char num1Str[100], base1Str[10];
    printf("Enter base of first number (2,8,10,16): ");
    scanf("%s", base1Str);
    clearInputBuffer();
    int base1 = atoi(base1Str);
    if(base1 != 2 && base1 != 8 && base1 != 10 && base1 != 16){
        printf("Invalid choice. Try again!");
        return;
    }
    do{
        printf("Enter first number: ");
        scanf("%s", num1Str);
        clearInputBuffer();
    }
    while(!isValidInput(num1Str,base1));
    // Get second number and base
    char num2Str[100], base2Str[10];
    printf("Enter base of second number (2,8,10,16): ");
    scanf("%s", base2Str);
    clearInputBuffer();
    int base2 = atoi(base2Str);
    if(base2 != 2 && base2 != 8 && base2 != 10 && base2 != 16){
        printf("Invalid choice. Try again!");
        return;
    }
    do{
        printf("Enter second number: ");
        scanf("%s", num2Str);
        clearInputBuffer();
    }
    while(!isValidInput(num2Str,base2));
    // Convert both numbers to decimal
    float dec1, dec2;
    switch(base1) {
        case 2: dec1 = calculateBinToDec(num1Str); break;
        case 8: dec1 = calculateOctToDec(num1Str); break;
        case 10: dec1 = atof(num1Str); break;
        case 16: dec1 = calculateHexToDec(num1Str); break;
        default: 
            return;
    }
    
    switch(base2) {
        case 2: dec2 = calculateBinToDec(num2Str); break;
        case 8: dec2 = calculateOctToDec(num2Str); break;
        case 10: dec2 = atof(num2Str); break;
        case 16: dec2 = calculateHexToDec(num2Str); break;
        default: 
            return;
    }
    
    // Check for division by zero
    if (dec2 == 0) {
        printf("Error: Division by zero!\n");
        return;
    }
    
    // Perform division
    float result = dec1 / dec2;
    
    // Display result in all bases
    printf("\nResult:\n");
    printf("Decimal: %.10g\n", result);
    
    char* bin = calculateDecToBin(result);
    if (bin) {
        printf("Binary: %s\n", bin);
    }
    
    char* oct = calculateDecToOct(result);
    if (oct) {
        printf("Octal: %s\n", oct);
    }
    
    char* hex = calculateDecToHex(result);
    if (hex) {
        printf("Hexadecimal: %s\n", hex);
    }
    addToCalculatorHistory(num1Str, num2Str, base1Str, base2Str, result, "/");
    // Free memory
    if (bin) free(bin);
    if (oct) free(oct);
    if (hex) free(hex);
}

        //MENU BLOCK
void greeting(){
    printf(PINK"========================================================\n"RESET);
    printf("    Welcome to Number Base Converter & calculator Tool! \n");
    printf("    Created by: Nob Johnson and Oeng Chheangsing        \n");
    printf("    This tool helps you convert and calculate between :                \n");
    printf("    Binary, Octal, Decimal, Hexadecimal.                \n");
    printf(PINK"========================================================\n\n"RESET);
    printf("What can i help you today ?\n");
    printf("Do you want to convert or calculate ? \n");
}

void choiceForCalculate(){
    int choice;
    while (1) {
        printf("\n====== Number Base Calculation Tool ======\n");
        printf("1.  Addition\n");
        printf("2.  Subtraction\n");
        printf("3.  Multiplication\n");
        printf("4.  Division\n");
        printf("5.  View History\n");
        printf("0.  Back to menu \n");
        printf("========================================\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf(RED"Invalid input! Please enter a number.\n"RESET);
            // Clear invalid input from buffer
            clearInputBuffer(); 
            continue; // ask again
        }
        clearInputBuffer();
        switch(choice) {
            case 1: addNumbers(); pressEnter(); clearScreen(); break;
            case 2: subtractNumbers(); pressEnter(); clearScreen(); break;
            case 3: multiplyNumbers(); pressEnter(); clearScreen(); break;
            case 4: divideNumbers(); pressEnter(); clearScreen(); break;
            case 5: checkHistoryCalculation(); pressEnter(); clearScreen();  break;
            case 0: 
                clearScreen();
                printf("Returning to main menu...\n");
                return;  
            default: 
                printf(RED"Invalid choice. Please try again.\n"RESET);
        }
    }
}

void choiceForConvert(){
    int choice;
    while (1) {
        printf("\n====== Number Base Conversion Tool ======\n");
        printf("1.  Decimal to Binary\n");
        printf("2.  Decimal to Octal\n");
        printf("3.  Decimal to Hexadecimal\n");
        printf("4.  Binary to Decimal\n");
        printf("5.  Binary to Octal\n");
        printf("6.  Binary to Hexadecimal\n");
        printf("7.  Octal to Decimal\n");
        printf("8.  Octal to Binary\n");
        printf("9.  Octal to Hexadecimal\n");
        printf("10. Hexadecimal to Decimal\n");
        printf("11. Hexadecimal to Binary\n");
        printf("12. Hexadecimal to Octal\n");
        printf("13. Check History \n");
        
        printf("0.  Back to menu \n");
        printf("========================================\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf(RED"Invalid input! Please enter a number.\n"RESET);
            // Clear invalid input from buffer
            clearInputBuffer(); 
            continue; // ask again
        }
        clearInputBuffer();

        switch(choice) {
            case 1: decToBin(); pressEnter(); clearScreen(); break;
            case 2: decToOct(); pressEnter(); clearScreen(); break;
            case 3: decToHex(); pressEnter(); clearScreen(); break;
            case 4: binToDec(); pressEnter(); clearScreen(); break;
            case 5: binToOct(); pressEnter(); clearScreen(); break;
            case 6: binToHex(); pressEnter(); clearScreen(); break;
            case 7: octToDec(); pressEnter(); clearScreen(); break;
            case 8: octToBin(); pressEnter(); clearScreen(); break;
            case 9: octToHex(); pressEnter(); clearScreen(); break;
            case 10: hexToDec(); pressEnter(); clearScreen(); break;
            case 11: hexToBin(); pressEnter(); clearScreen(); break;
            case 12: hexToOct(); pressEnter(); clearScreen(); break;
            case 13: checkHistory(); pressEnter(); clearScreen(); break;
            case 0: 
                clearScreen();
                printf("Returning to main menu...\n");
                return;  
            default: 
                printf(RED"Invalid choice. Please try again.\n"RESET);
        }
    }
}

int  main(){
    
    int choice;
    greeting();
    while(1){
        printf("Enter 1 if you want to convert \n");
        printf("Enter 2 if you want to calculate \n");
        printf("Enter 0 if you want to exit the program\n");
        printf("Please enter your choice : ");
        if (scanf("%d", &choice) != 1) {
            printf(RED"\nInvalid input! Please enter a number.\n"RESET);
            // Clear invalid input from buffer
            pressEnter();
            clearInputBuffer(); 
            continue; // ask again
        }
        clearInputBuffer();
        switch (choice) {
            case 1:
                clearScreen();
                choiceForConvert();
                break;
            case 2:
                clearScreen();
                choiceForCalculate();
                break;            
            case 0:
                printf("Goodbye!\n");
                exit(0);
            default:
                printf(RED"Invalid choice. Try again.\n"RESET);
        }
    }

    return 0;
}