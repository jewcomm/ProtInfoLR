#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <ctime>
#include <chrono>
#include <thread>

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

#define PURE_PASS false

uint32_t crcPoly = 0xEDB88320UL;
uint32_t crcTable[256];

// все возможные символы
std::vector<unsigned char> allSymbols;

// сгенерированный пароль
unsigned char * pass;
size_t passLen = 6;
uint32_t passCRC;

// глобальная переменная для завершения потоков, если нашли пароль
bool passFounded = false;


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
    for(size_t i = 0; i < len; i++){
        password[i] = allSymbols.at(std::rand() % allSymbols.size()); // NOLINT(cert-msc30-c, cert-msc50-cpp)
    }
    password[len] = '\0';
    return password;
}

// 31513 ms
void bruteForceU(size_t len, unsigned char start, unsigned char end){
    auto *val = new unsigned char[len + 1];
    val[len] = '\0';

    for(size_t i = 1; i < len; i++) val[i] = allSymbols[0];

    val[0] = start;

    while(val[0] <= end && !passFounded){
        for(int i = 0; i < allSymbols.size(); i++){
            val[len - 1] = allSymbols[i];
#if PURE_PASS
            if(strncmp((char*)val, (char*)pass, len) == 0){
                passFounded = true;
                std::cout << val << std::endl;
                return;
            }
#else
            if(passCRC == crc32Func((char*)val, len)){
                std::cout << std::dec << "Founded pass: " << val << std::endl;
                std::cout << std::hex << "CRC founded pass: " << crc32Func((char*)val, len) << std::endl;
            }
#endif
        }
        // костыль, надо переделать
        for(int i = len - 2; i >= 0; i--){
            if(val[i] < allSymbols.back()) {
                val[i]++;
                break;
            }
            if(i != 0) val[i] = allSymbols.front();
            else val[i]++;
        }
    }
    return;
}

void mThreadsRun(size_t count){
    size_t step = allSymbols.size() / count;
    std::vector<std::thread> threads;
    threads.push_back(std::thread(bruteForceU, passLen, allSymbols.front(), allSymbols.at(step)));
    for(size_t i = 1; i < count - 1; i++){
        threads.push_back(std::thread(bruteForceU, passLen, allSymbols.at(i * step + 1), allSymbols.at((i + 1) * step)));
    }
    threads.push_back(std::thread(bruteForceU, passLen, allSymbols.at((count - 1) * step + 1), allSymbols.back()));
    for(size_t i = 0 ; i < threads.size(); i++){
        threads.at(i).join();
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

    auto begin = std::chrono::steady_clock::now();

    mThreadsRun(6);
    auto end = std::chrono::steady_clock::now();

    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    std::cout << std::dec << "The time: " << elapsed_ms.count() << " ms\n";

    return 0;
}
