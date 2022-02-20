// FeiPS_vstupny_projekt.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstdint>
#include <cstring>
#include <ctime>

#define BUFFER_SIZE 400

using namespace std;

typedef uint8_t byte;

enum Architecture {
    k_amd64 = 8,
    k_armv7e = 4
};

class  BinaryProcessor {

public:
    static byte GetParsedBinary(byte input_byte);
    static uint16_t ShiftAndCombineTwoBytes(byte input_byte_1, byte input_byte_2);
};


class RandomNumberGenerator {
    uint16_t seed_; // a four digit number
    uint16_t Random();
public:
    RandomNumberGenerator();
    byte GenerateRandomNumber();
    void Seed();
};



int main(int argc, char* argv[]) {
    const Architecture k_architecture_flag = CStringToArchitecture(argv[1]);
    byte* buffer;
    
    switch (k_architecture_flag) {
    case k_amd64: buffer = (byte*) malloc(BUFFER_SIZE * k_amd64); break;
    case k_armv7e: buffer = (byte*)malloc(BUFFER_SIZE * k_armv7e); break;
    }


}

byte BinaryProcessor::GetParsedBinary(byte input_byte) {
   //Brian Kernighan's algorithm
   //chosen for it's ease of understanding
    unsigned int count; 
    for (count = 0; input_byte; count++)
    {
        input_byte &= input_byte - 1; 
    }
    return (count);
}

Architecture CStringToArchitecture(char cstring[]) {
        const int k_max_lenght = 6;
        if (strncmp(cstring, "amd64", k_max_lenght)) {
            return(k_amd64);
        }
        if (strncmp(cstring, "armv7E", k_max_lenght)) {
            return(k_armv7e);
        }
        //add exception
    
}


RandomNumberGenerator::RandomNumberGenerator() {
        Seed();
}

uint16_t RandomNumberGenerator::Random() {
        // Lehmer RNG with Sinclair ZX81 parameters; chosen because of simplicity and small numbers
        seed_ = 75 * seed_ % 65537;
        return (seed_);
}

byte RandomNumberGenerator::GenerateRandomNumber() {
    return(Random() % UINT8_MAX); //UINT8_MAX is maxof(byte) in other words 
 }

void RandomNumberGenerator::Seed() {
        seed_ = (uint16_t)(time(nullptr) % (10000 + 1000)) - 1000;  //reminder that seed_ is a four digit number
}


