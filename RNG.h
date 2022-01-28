#ifndef RNG_INC
#define RNG_INC
#include <vector>

int randint(int min, int max);
double randreal(double min, double max);
void shuffle(std::vector<int> &item);
#endif