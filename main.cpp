#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <ctime>

/*
  Name  : CRC-32
  Poly  : зеркальное отображение 0x04C11DB7    x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11
                       + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
  Init  : 0xFFFFFFFF
  Revert: true
  XorOut: 0xFFFFFFFF
  Check : 0xCBF43926 ("123456789")
  MaxLen: 268 435 455 байт (2 147 483 647 бит)
*/

uint32_t crcPoly = 0xEDB88320UL;
uint32_t crcTable[256];

// все возможные символы
std::vector<unsigned char> allSymbols;

// сгенерированный пароль
unsigned char * pass;
size_t passLen = 6;
uint32_t passCRC;

void crc32TableGen(){
    uint32_t crc;
    for(int i = 0; i < 256; i++){
        crc = i;
        for(int j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ crcPoly : crc >> 1;
        crcTable[i] = crc;
    }
}

uint32_t crc32Func(char *buf, uint32_t len){
    uint32_t crc;
    crc = 0xFFFFFFFFUL;
    while (len--)
        crc = crcTable[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);

    return crc ^ 0xFFFFFFFFUL;
}

unsigned char *passwordGen(size_t len){
    auto *password = new unsigned char[len + 1];
    for(int i = 0; i < len; i++){
        password[i] = allSymbols.at(std::rand() % allSymbols.size()); // NOLINT(cert-msc30-c, cert-msc50-cpp)
    }
    password[len] = '\0';
    return password;
}

unsigned char * bruteForce6(size_t len, int beg, int last){
    std::vector<int> symbPos;
    symbPos.resize(len , 0);

    auto *val = new unsigned char[len + 1];
    val[len] = '\0';

    symbPos[0] = beg;
    while(symbPos[0] <= last){
        if(symbPos[0] == allSymbols.size()) break;

        for(int i = symbPos.size() - 1; i > 0; i--){
            if(symbPos[i] == allSymbols.size()){
                symbPos[i] = 0;
                symbPos[i - 1]++;
            }
        }

        for(int i = 0; i < len; i++){
            val[i] = allSymbols[symbPos[i]];
        }

        if(strncmp((char*)val, (char*)pass, len) == 0){
            std::cout << val << std::endl;
        }

        if(/*val[1] == '!' && */val[2] == '!' && val[3] == '!' && val[4] == '!' && val[5] == '!'){
            std::cout << val << std::endl;
        }

        symbPos[len - 1]++;
    }
}

int main() {
    std::srand(std::time(nullptr)); // NOLINT(cert-msc51-cpp)
    crc32TableGen();

    // ascii https://www.rapidtables.com/code/text/ascii-table.html
    for(unsigned char i = '!'; i <= '~'; i++){
        allSymbols.push_back(i);
    }

    pass = passwordGen(passLen);
    std::cout << "Generated password: " << pass << std::endl;

    passCRC = crc32Func((char*)pass, passLen);
    std::cout << "CRC32 password: " << std::hex << passCRC << std::endl;

    bruteForce6(6, 0, 2);

    return 0;
}
