#include <iostream>
#include <list>
#include <cmath>

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

uint64_t findInBound(list<uint64_t>*source, uint64_t min, uint64_t max){
    if(source->empty() || (*prev(source->end()) < min)) return (uint64_t)-1;

    auto mMin = source->begin();

    while(*mMin < min) mMin = next(mMin);

    auto mMax = mMin;

    int count = 0;
    while(source->end() != next(mMax) && *next(mMax) < max){
        count++;
        mMax = next(mMax);
    }

    int randVal = rand() % count; // NOLINT(cert-msc30-c, cert-msc50-cpp)

    while (randVal){
        mMin = next(mMin);
        randVal--;
    }

    return *mMin;
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
    list<uint64_t> naturShisla;
    unsigned long res = sieveOfErastosthenes(&naturShisla, 100000);

    uint64_t p = 0;
    uint64_t q;

    auto ptrP = prev(naturShisla.end());

    /// при расчете чисел до 100 000
    /// выбирается число р = 99839
    /// тогда при формуле p = 2q + 1
    /// q = 49919, и является простым числом
    /// работает корректно
    for(; ptrP != naturShisla.begin() && p == 0; ptrP = prev(ptrP)){
        for(auto ptrQ = prev(ptrP); ptrQ != naturShisla.begin(); ptrQ = prev(ptrQ)){
            if(*ptrP == (2 * (*ptrQ)) + 1) {
                p = *ptrP;
                q = *ptrQ;
                break;
            }
        }
    }

    uint64_t g = p - 1 - rand() % 1000; // NOLINT(cert-msc30-c, cert-msc50-cpp)

    for(; g > 0; g--){
        if(ipowMod(g, q, p) != 1) break;
    }

    uint64_t x;
    // выполняется если x == g
    // соотвественно, х != g
    while((x = p - 1 - (rand() % 5000)) == g); // NOLINT(cert-msc30-c, cert-msc50-cpp)

    __uint128_t y = ipowMod(g, x, p);

    // подготовительная часть завершена
    // зашифруем что то

    uint64_t M = (uint64_t)(rand() * rand() * rand()) % 90000; // NOLINT(cert-msc30-c, cert-msc50-cpp)


    cout << "Generated message: " << M << endl;

    uint64_t k;
    // выполняется если k == g
    // соотвественно, k != g
    while((k = p - 2 - (rand() % 5000)) == g || k == x); // NOLINT(cert-msc30-c, cert-msc50-cpp)

    uint64_t r = ipowMod(g, k, p);
    uint64_t e = M *  ipowMod(y, k, p) % p;

    cout << "R: " << r << endl;
    cout << "e: " << e << endl;

    uint32_t newM = e *  ipowMod(r, p - 1 - x, p) % p;

    cout << "Decoding message: " << newM << endl;

    return 0;

}
