#include <iostream>
#include <cstdint>
#include <string>
#include <ctime>
#include <exception>
#include <cstdio>

#define BUFFER_SIZE 400
#define HEADER_SIZE 3

using namespace std;

typedef uint8_t byte;

//For the sake of doing this task i assumed that the architecture settings meant 64bit ints for amd64
// and 32bit ints for armv7E and that armv7E has flipped (flipped after being flipped as they should be) bytes in CRC
//Feel free to correct me in person
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
byte* CompileMessage(int& message_length, byte* header, int header_lenght, byte* buffer, int buffer_lenght, byte output_buf1, uint16_t output_buf23);
uint16_t GenerateCrc(byte* msg, int msg_lenght, Architecture flag);

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

    try {
        int err;
        err = fopen_s(&f, "output.dat", "wb");
         if (err!=0) {
           throw runtime_error("Could not open file!");
        }
    }
    catch (runtime_error& e) {
        cerr << "Error: " << e.what();
        exit(1);
    }

    byte header[HEADER_SIZE] = { 0xAA,0xBB,0x01 };

    for (int i = 0; i < BUFFER_SIZE;) {
        fwrite(header,sizeof(byte), sizeof(header), f);
        
        fwrite(&buffer[i], sizeof(byte), sizeof(buffer[i]), f);
        byte byte1 = BinaryProcessor::GetParsedBinary(buffer[i]);
        fwrite(&byte1, sizeof(byte), sizeof(byte1), f);
      

        fwrite(&buffer[i+1], sizeof(byte), sizeof(buffer[i+1])+sizeof(buffer[i + 2]), f);       
        uint16_t byte23 = BinaryProcessor::CombineAndShiftTwoBytes(buffer[i+1], buffer[i + 2]);
        fwrite(&byte23, sizeof(uint16_t), sizeof(byte23), f);
        
        byte* message;
        int message_length;
        message = CompileMessage(message_length,header, HEADER_SIZE, &buffer[i],3, byte1, byte23);

        uint16_t crc = GenerateCrc(message,message_length, k_architecture_flag);
        fwrite(&crc, sizeof(uint16_t), sizeof(crc), f);
        
        free(message);
        i += 3;
    }

    fclose(f);
    free(buffer);

    return(0);
}

byte* CompileMessage(int& message_length, byte* header, int header_lenght, byte* buffer, int buffer_lenght, byte output_buf1, uint16_t output_buf23) {
    message_length = header_lenght + buffer_lenght + 3;
    byte* msg;
    msg = (byte*)malloc(message_length * sizeof(byte));

    int i;
    for (i = 0; i < header_lenght; i++) {
        msg[i] = header[i];
    }
    msg[i] = buffer[0];
    i++;
    msg[i] = output_buf1;
    i++;
    msg[i] = buffer[1];
    i++;
    msg[i] = buffer[2];
    i++;
    msg[i] = (uint8_t)output_buf23;
    i++;
    msg[i] = (uint8_t)(output_buf23 >> 8);

    return (msg);

}

uint16_t GenerateCrc (byte*msg,int msg_lenght,Architecture flag) {
   
    //Copyright (c) 2019 Tiago Ventura
    
    // Permission is hereby granted, free of charge, to any person obtaining a copy
    //of this software and associated documentation files(the "Software"), to deal
    //in the Software without restriction, including without limitation the rights
    //to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
    //copies of the Software, and to permit persons to whom the Software is
    //furnished to do so, subject to the following conditions :

    //THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    //IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    //FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
    //AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    //LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    //OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    //SOFTWARE.
    
    uint16_t crc = 0xFFFF;
    char bit = 0;
    int i;

    for (i = 0; i < msg_lenght; i++)
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
        return (0); //this case should never occur
    }
}

Architecture CStringToArchitecture(char cstring[]) {
    string flag = cstring;

    if (flag==string("amd64")) {
        return(k_amd64);
    }
    if (flag == string("armv7E")) {
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


