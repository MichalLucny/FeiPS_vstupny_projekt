#include <iostream>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <exception>

#define BUFFER_SIZE 400
#define HEADER_SIZE 3

using namespace std;

typedef uint8_t byte;

enum Architecture {
    k_amd64,
    k_armv7e
};

class  BinaryProcessor {
public:
    static byte GetParsedBinary (byte input_byte);
    static uint16_t CombineAndShiftTwoBytes (byte input_byte_1, byte input_byte_2);
};

class RandomNumberGenerator {
    uint16_t seed_; // a four digit number
    uint16_t Random();
public:
    RandomNumberGenerator();
    byte GenerateRandomNumber();
    void Seed();
};

Architecture CStringToArchitecture(char cstring[]);
uint16_t GenerateCrc(byte* header, int header_lenght, byte* buffer, int buffer_lenght, Architecture flag);

int main(int argc, char* argv[]) {
    Architecture k_architecture_flag;
    try {
        k_architecture_flag = CStringToArchitecture(argv[1]);
    }
    catch (invalid_argument & e) {
        cerr << "Error: " << e.what();
        exit(1);
    };

    byte* buffer;
    
    switch (k_architecture_flag) {
    case k_armv7e: buffer = (byte*) malloc(BUFFER_SIZE * 4); break;
    case k_amd64:
    default:    
        buffer = (byte*)malloc(BUFFER_SIZE * 8); break;
    }

    RandomNumberGenerator rng;

    for (int i = 0; i < BUFFER_SIZE;i++) {
        buffer[i] = rng.GenerateRandomNumber();
    }

    FILE* f;
    fopen_s(&f,"output.dat", "wb");

    
    byte header[HEADER_SIZE] = { 0xAA,0xBB,0x01 };

    for (int i = 0; i < BUFFER_SIZE;) {
        fwrite(header,sizeof(byte), sizeof(header), f);
        
        fwrite(&buffer[i], sizeof(byte), sizeof(buffer[i]), f);
        byte byte1 = BinaryProcessor::GetParsedBinary(buffer[i]);
        fwrite(&byte1, sizeof(byte), sizeof(byte1), f);
      

        fwrite(&buffer[i+1], sizeof(byte), sizeof(buffer[i+1])+sizeof(buffer[i + 2]), f);
        uint16_t byte23 = BinaryProcessor::CombineAndShiftTwoBytes(buffer[i+1], buffer[i + 2]);
        fwrite(&byte23, sizeof(uint16_t), sizeof(byte23), f);
        
        uint16_t crc = GenerateCrc(header, HEADER_SIZE, &buffer[i],3, k_architecture_flag);
        i += 3;
    }

    fclose(f);
    free(buffer);

    return(0);
}



uint16_t GenerateCrc (byte* header, int header_lenght, byte* buffer, int buffer_lenght, Architecture flag) {
    byte* msg;
    msg = (byte*) malloc((header_lenght + buffer_lenght) * sizeof(byte));
    int i;
    for (i = 0; i < header_lenght; i++) {
        msg[i] = header[i];
    }
    for (int j = 0; j < buffer_lenght; j++, i++) {
        msg[i] = buffer[j];
    }

    int dataLenght = header_lenght + buffer_lenght;

    //Copyright (c) 2019 Tiago Ventura
    
    uint16_t crc = 0xFFFF;
    unsigned int i = 0;
    char bit = 0;

    for (i = 0; i < dataLenght; i++)
    {
        crc ^= msg[i];

        for (bit = 0; bit < 8; bit++)
        {
            if (crc & 0x0001)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }


    uint8_t crcl = (uint8_t)crc;
    uint8_t crch = (uint8_t)(crc >> 8);


    switch (flag) {
    case k_armv7e:
        return (crch << 8 | crcl);
    case k_amd64:
        return (crcl << 8 | crch);
    default:
        return (256);
    }
}

Architecture CStringToArchitecture(char cstring[]) {
    const int k_max_lenght = 6;
    if (strncmp(cstring, "amd64", k_max_lenght)) {
        return(k_amd64);
    }
    if (strncmp(cstring, "armv7E", k_max_lenght)) {
        return(k_armv7e);
    }
    throw (invalid_argument("Incorrect architecture selected!"));

}


byte BinaryProcessor::GetParsedBinary(byte input_byte) {
   //Brian Kernighan's algorithm
   //chosen for it's ease of understanding
    byte count; 
    for (count = 0; input_byte; count++)
    {
        input_byte &= input_byte - 1; 
    }
    return (count);
}

uint16_t BinaryProcessor::CombineAndShiftTwoBytes (byte input_byte_1, byte input_byte_2) {
    uint16_t output = ((uint16_t)input_byte_1 << 8) | input_byte_2; //combine into 16bit int
    return (output << 8); //shift
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


