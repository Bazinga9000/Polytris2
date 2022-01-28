#include "headers.h"


/*
0 - T
1 - O
2 - J
3 - S
4 - L
5 - Z
6 - I



0 - Q
1 - K
2 - F
3 - X
4 - P
5 - T
6 - Y
7 - R
8 - U
9 - J
10 - S
11 - V
12 - N
13 - W
14 - H
15 - L
16 - Z
17 - I

*/


namespace poly {
	Polyomino randompoly(int order)
	{
		int length = polylengths[order - 1];

		int polyid = randint(0, length - 1);

		return getpolyfromid(order, polyid);

	}


	int colorpoly(int order, int polyid) {
		std::vector<int> colors;
		switch (order) {
		case 1:
			colors = { 0 };
			break;

		case 2:
			colors = { 0 };
			break;

		case 3:
			colors = { 2,0 };
			break;

		case 4:
			colors = { 5,3,1,4,2,6,0 };
			break;

		case 5:
			colors = { 15,22,7,20,14,16,21,8,17,10,23,18,12,19,13,11,24,9,99 };
			break;

		default:
			colors = { 5,3,1,4,2,6,0,15,22,7,20,14,16,21,8,17,10,23,18,12,19,13,11,24,9 };
			auto sim = similarity(shifttozero(getshapefromid(order, polyid)));
			if (sim.size() == 1) {
				return colors[sim[0]];
			}
			else if (sim.size() == 2) {
				switch (sim[0]) {
				case 7:
					return 25;
				case 8:
					return 26;
				case 9:
					return 27;
				case 3:
					return 28;
				case 2:
					return 29;
				case 19:
					return 30;
				case 16:
					
					return 31;
				case 17:
					return 32;
				default:
					std::cout << sim[0] << " " << sim[1] << "\n";
					return 32;
				}
			} else {
				if (polyid == polylengths[order-1] - 1) {
					return 9;
				} else {
					return 3;
				}
			}
		}

		return colors[polyid];
	}


	std::vector<double> spectrum(std::vector<int> side, int order, int power) {
		std::vector<double> spec;
		double mean = (std::accumulate(side.begin(), side.end(), 0.0) / side.size());
		for (int n = 1; n <= power; n++) {
			double sum = 0;
			for (int q = 0; q < side.size(); q++) {
				double valuea = std::pow(q + 1 - (side.size() + 1) / 2.0, n);
				double valueb = side[q] - mean;
				sum += valuea * valueb;
			}
			sum /= (order * std::tgamma(n + 1)); 
			spec.push_back(sum);
		}
		return spec;
	}

	
	std::vector<std::vector<double>> polyspectrum(poly_t poly) {
		std::vector<int> minx = {};
		std::vector<int> miny = {};
		std::vector<int> maxx = {};
		std::vector<int> maxy = {};

		int order = poly.size();

		int mx = 0;
		int my = 0;

		for (auto block : poly) {
			if (block[0] > mx) mx = block[0];
			if (block[1] > my) my = block[1];
		}

		minx.resize(my+1, 9999);
		miny.resize(mx+1, 9999);
		maxx.resize(my+1);
		maxy.resize(mx+1);

		for (auto block : poly) {
			if (minx[block[1]] > block[0]) minx[block[1]] = block[0]; 
			if (miny[block[0]] > block[1]) miny[block[0]] = block[1]; 
			if (maxx[block[1]] < block[0]) maxx[block[1]] = block[0]; 
			if (maxy[block[0]] < block[1]) maxy[block[0]] = block[1]; 

		}

		for (int i = 0; i < maxx.size(); i++) { maxx[i] *= -1; }
		for (int i = 0; i < maxy.size(); i++) { maxy[i] *= -1; }
		std::reverse(maxy.begin(), maxy.end());
		std::reverse(minx.begin(), minx.end());

		return {
			spectrum(minx,order,4),
			spectrum(miny,order,4),
			spectrum(maxx,order,4),
			spectrum(maxy,order,4)
		};

	}

	std::vector<int> similarity(poly_t poly) {
		std::vector<std::vector<std::vector<double>>> testspectrums;
		std::vector<double> distances;

		for (auto p : poly4) {
			poly_t v;
			for (int i = 0; i < 4; i++) {
				v.push_back({ p[i][0],p[i][1] });
			}

			testspectrums.push_back(polyspectrum(v));
		}
		for (auto p : poly5) {
			poly_t v;
			for (int i = 0; i < 5; i++) {
				v.push_back({ p[i][0],p[i][1] });
			}

			testspectrums.push_back(polyspectrum(v));
		}


		auto spectrum = polyspectrum(poly);

		for (auto sp : testspectrums) {
			double sums[4] = { 0,0,0,0 };
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					for (int perm = 0; perm < 4; perm++) {
						sums[perm] += std::pow(spectrum[(i + perm) % 4][j] - sp[i][j], 2);
					}
				}
			}
			double min = 99999;
			for (int i = 0; i < 4; i++) {
				if (min > sums[i]) min = sums[i];
			}
			distances.push_back(min);
		}


		std::vector<int> minimums;

		double minimum = 99999;
		for (auto n : distances) {
			if (n < minimum) minimum = n;
		}

		for (int i = 0; i < distances.size(); i++) {
			if (std::abs(distances[i] - minimum) < 1e-10) minimums.push_back(i);
		}

		return minimums;
	}

	std::array<int, 2> getdims(poly_t poly) {
		int width_max = 0;
		int width_min = 9999;
		int height_max = 0;
		int height_min = 9999;

		for (int i = 0; i < poly.size(); i++) {
			std::vector<short> brick = poly[i];

			if (brick[0] > width_max) { width_max = brick[0]; }
			if (brick[0] < width_min) { width_min = brick[0]; }
			if (brick[1] > height_max) { height_max = brick[1]; }
			if (brick[1] < height_min) { height_min = brick[1]; }
		}

		return { width_max - width_min + 1, height_max - height_min + 1 };


	}

	poly_t shifttozero(poly_t poly) {
		int width_min = 9999;
		int height_min = 9999;

		for (int i = 0; i < poly.size(); i++) {
			std::vector<short> brick = poly[i];

			if (brick[0] < width_min) { width_min = brick[0]; }
			if (brick[1] < height_min) { height_min = brick[1]; }
		}

		poly_t newpoly = poly;

		for (int i = 0; i < poly.size(); i++) {
			newpoly[i][0] -= width_min;
			newpoly[i][1] -= height_min;
		}

		return newpoly;
	}

	poly_t centershift(poly_t poly) {
		poly_t newpoly = shifttozero(poly);



		std::sort(newpoly.begin(), newpoly.end());

		int trix[2] = { 0,-1 };
		int triy[2] = { 0,0 };

		//manual overrides for tetromino to make it EXACTLY like SRS
		//have fun with your tspins zwei
		int tetrax[7] = { -1,0,0,-1,-1,0,-1 };
		int tetray[7] = { 0,0,-1,0,0,-1,0 };
		
		//manual overrides for pentomino because the current system is weird
		//have fun with your T/F/R/U/W spins
		int pentax[18] = {-1,-2,-1,-1,-1,-1,-1,-1,0,0,-1,-1,0,-1,-2,-2,-1,-2};
		int pentay[18] = {0,0,-1,-1,0,-1,0,-1,-1,-2,-1,-1,-1,-1,-1,0,-1,0};

		if (newpoly.size() == 3) {
			for (int i = 0; i < 2; i++) {
				auto testpoly = getpolyfromid(3, i).shape;
				if (newpoly == testpoly) {
					int shiftcol = trix[i];
					int shiftrow = triy[i];
					newpoly = shift(newpoly, shiftcol, shiftrow);
					return newpoly;
				}
			}
		}
		if (newpoly.size() == 4) {
			for (int i = 0; i < 7; i++) {
				auto testpoly = getpolyfromid(4, i).shape;
				if (newpoly == testpoly) {
					int shiftcol = tetrax[i];
					int shiftrow = tetray[i];
					newpoly = shift(newpoly, shiftcol, shiftrow);
					return newpoly;
				}
			}
		}

		if (newpoly.size() == 5) {
			for (int i = 0; i < 18; i++) {
				auto testpoly = getpolyfromid(5, i).shape;
				if (newpoly == testpoly) {
					int shiftcol = pentax[i];
					int shiftrow = pentay[i];
					newpoly = shift(newpoly, shiftcol, shiftrow);
					return newpoly;
				}
			}
		}

		
		auto dims = poly::getdims(newpoly);

		int shiftl = dims[0] / 2;
		int shifth = dims[1] / 2;

		newpoly = poly::shift(newpoly, -shiftl, -shifth);
		return newpoly;
	}

	poly_t rotate(poly_t poly, int numrots) {

		poly_t newpoly = poly;

		//this is the only polyomino that breaks the rotation system, but it doesn't rotate anyways :P
		poly_t square = { { 0,0 },{ 0,1 },{ 1,0 },{ 1,1 } };
		if (newpoly == square) { return newpoly; }

		for (int rots = 0; rots < numrots; rots++) {
			for (int i = 0; i < newpoly.size(); i++) {
				double nw = newpoly[i][0];
				double nh = newpoly[i][1];
				std::swap(nw, nh);
				nw *= -1;
				newpoly[i][0] = static_cast<short>(nw);
				newpoly[i][1] = static_cast<short>(nh);

			}
		}

		std::sort(newpoly.begin(), newpoly.end());
		return newpoly;

	}

	poly_t shift(poly_t poly, int dx, int dy) {
		poly_t newpoly = poly;

		for (int i = 0; i < poly.size(); i++) {
			newpoly[i][0] += dx;
			newpoly[i][1] += dy;
		}

		return newpoly;
	}

	poly_t getshapefromid(int order, int polyid) {
		poly_t poly;

		for (int brick = 0; brick < order; ++brick) {
			std::vector<short> block;

			switch (order) {
			case 1: {
				block.push_back(poly1[polyid][brick][0]);
				block.push_back(poly1[polyid][brick][1]);
				break;
			}
			case 2: {
				block.push_back(poly2[polyid][brick][0]);
				block.push_back(poly2[polyid][brick][1]);
				break;
			}
			case 3: {
				block.push_back(poly3[polyid][brick][0]);
				block.push_back(poly3[polyid][brick][1]);
				break;
			}
			case 4: {
				block.push_back(poly4[polyid][brick][0]);
				block.push_back(poly4[polyid][brick][1]);
				break;
			}
			case 5: {
				block.push_back(poly5[polyid][brick][0]);
				block.push_back(poly5[polyid][brick][1]);
				break;
			}
			case 6: {
				block.push_back(poly6[polyid][brick][0]);
				block.push_back(poly6[polyid][brick][1]);
				break;
			}
			case 7: {
				block.push_back(poly7[polyid][brick][0]);
				block.push_back(poly7[polyid][brick][1]);
				break;
			}
			case 8: {
				block.push_back(poly8[polyid][brick][0]);
				block.push_back(poly8[polyid][brick][1]);
				break;
			}
			case 9: {
				block.push_back(poly9[polyid][brick][0]);
				block.push_back(poly9[polyid][brick][1]);
				break;
			}
			case 10: {
				block.push_back(poly10[polyid][brick][0]);
				block.push_back(poly10[polyid][brick][1]);
				break;
			}
			case 11: {
				block.push_back(poly11[polyid][brick][0]);
				block.push_back(poly11[polyid][brick][1]);
				break;
			}
			case 12: {
				block.push_back(poly12[polyid][brick][0]);
				block.push_back(poly12[polyid][brick][1]);
				break;
			}
			}

			poly.push_back(block);
		}

		return poly;
	}

	Polyomino getpolyfromid(int order, int polyid) {
		int length = polylengths[order - 1];

		poly_t poly = getshapefromid(order, polyid);

		auto dims = getdims(poly);

		Polyomino polystruct;

		polystruct.shape = poly;
		polystruct.order = order;
		polystruct.polyid = polyid;
		polystruct.color = colorpoly(order, polyid);
		polystruct.width = dims[0];
		polystruct.height = dims[1];

		return polystruct;
	}
}