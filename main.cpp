#include <iostream>
#include <list>
#include <cmath>

uint64_t MAX_NUMBER = 100'000;

using namespace std;

list<uint64_t>::iterator foundBiggerSquare(list<uint64_t> *vec, list<uint64_t>::iterator iter);

uint32_t sieveOfErastosthenes(list<uint64_t> *result, uint64_t maximum){
    if(!result->empty()) return (uint32_t)-1;

    for(uint64_t i = 2; i < maximum+1; i++){
        result->push_back(i);
    }

    for(auto base = result->begin(); base != result->end(); base = next(base)){
        for(auto canditate = (*next(base) <= 4? next(base): foundBiggerSquare(result, base));
            canditate != result->end();
            canditate = (*canditate % *base == 0) ? result->erase(canditate) : next(canditate));
    }
    return result->size();
}

list<uint64_t>::iterator foundBiggerSquare(list<uint64_t> *vec, list<uint64_t>::iterator iter){
    uint64_t square = pow(*iter, 2);
    for(;iter != vec->end(); iter= next(iter)){
        if(*iter >= square) return iter;
    }
    return vec->end();
}

__uint128_t ipowMod(__uint128_t base, __uint128_t exp, __uint128_t mod){
    if (mod == 1) return 0;
    __uint128_t res = 1;
    base = base % mod;
    while(exp > 0)
    {
        if(exp % 2 == 1)
        {
            res = (res * base) % mod;
        }
        exp = exp / 2;
        base = (base * base) % mod;
    }
    return res;
}

int main() {
    srand(time(nullptr)); // NOLINT(cert-msc30-c, cert-msc51-cpp)
    list<uint64_t> primeNumbers;
    unsigned long res = sieveOfErastosthenes(&primeNumbers, MAX_NUMBER);

    uint64_t p = 0;
    uint64_t q;

    auto iterP = prev(primeNumbers.end());

    /// при расчете чисел до 100 000
    /// выбирается число р = 99839
    /// тогда при формуле p = 2q + 1
    /// q = 49919, и является простым числом
    /// работает корректно
    for(; iterP != primeNumbers.begin() && p == 0; iterP = prev(iterP)){
        for(auto iterQ = prev(iterP); iterQ != primeNumbers.begin(); iterQ = prev(iterQ)){
            if(*iterP == (2 * (*iterQ)) + 1) {
                p = *iterP;
                q = *iterQ;
                break;
            }
        }
    }

    // пусть изначальное значение g будет взято от p - 10000 до р - 20000
    uint64_t g = p - 10000 - rand() % 10000; // NOLINT(cert-msc30-c, cert-msc50-cpp)

    for(; g > 0; g--){
        if(ipowMod(g, q, p) != 1) break;
    }

    // пусть значение x будет от p - 20000 до р - 30000
    uint64_t x = p - 20000 - rand() % 10000; // NOLINT(cert-msc30-c, cert-msc50-cpp)

    // вычисляем у (секретное число абонента)
    __uint128_t y = ipowMod(g, x, p);

    // подготовительная часть завершена
    // зашифруем что то
    // т.к. М должно быть меньше p то сгенерируем его рандомно, верхняя граница p - 30000
    uint64_t M = (uint64_t)(rand() * rand() * rand()) % (p - 30000); // NOLINT(cert-msc30-c, cert-msc50-cpp)

    cout << "Generated message: " << M << endl;

    //пусть значение x будет от p - 40000 до р - 50000
    uint64_t k = p - 40000 - rand() % 10000;

    uint64_t r = ipowMod(g, k, p);
    uint64_t e = M *  ipowMod(y, k, p) % p;

    cout << "R: " << r << endl;
    cout << "e: " << e << endl;

    uint32_t newM = e *  ipowMod(r, p - 1 - x, p) % p;

    cout << "Decoding message: " << newM << endl;

    return 0;

}
