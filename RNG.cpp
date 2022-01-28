#include "headers.h"
//#include <random>


std::random_device rd;
std::mt19937_64 rng(rd());


int randint(int min, int max) {
	std::uniform_int_distribution<int> dist(min,max);

	return dist(rng);

}

double randreal(double min, double max) {
	std::uniform_real_distribution<double> dist(min, max);

	return dist(rng);
}


void shuffle(std::vector<int> &item) {
	std::shuffle(item.begin(), item.end(), rng);
}