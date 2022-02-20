// FeiPS_vstupny_projekt.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstdint>
#include <cstring>

typedef uint8_t byte;

enum Architecture {
    k_amd64,
    k_armv7e
};

class  BinaryProcessor {

public:
    static byte GetParsedBinary(byte input_byte);
    static uint16_t ShiftAndCombineTwoBytes(byte input_byte_1, byte input_byte_2);
};


class RandomNumberGenerator {
    byte seed_;
public:
    byte GenerateRandomNumber();
    void Seed();
};

    int main(int argc, char* argv[])
{
        const Architecture k_architecture_flag = CStringToArchitecture(argv[1]);

}

    Architecture CStringToArchitecture(char cstring[]) {
        const int k_max_lenght = 6;
        if (strncmp(cstring, "amd64", k_max_lenght)) {
            return(k_amd64);
        }
        if (strncmp(cstring, "armv7E", k_max_lenght)) {
            return(k_armv7e);
        }

    
    }


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
