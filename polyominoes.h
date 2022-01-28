#ifndef POLYOMINOES_INC
#define POLYOMINOES_INC


typedef std::vector<std::vector<short>> poly_t;

struct Polyomino {
	poly_t shape;
	int polyid;
	int order;
	int color;
	
	int row = 0;
	int column = 0;
	
	int width;
	int height;

};

namespace poly {
	Polyomino randompoly(int order);

	poly_t getshapefromid(int order, int polyid);

	Polyomino getpolyfromid(int order, int polyid);

	int colorpoly(int order, int polyid);

	std::vector<double> spectrum(std::vector<int> side, int order, int power);
	std::vector<std::vector<double>> polyspectrum(poly_t poly);
	std::vector<int> similarity(poly_t poly);


	std::array<int, 2> getdims(poly_t poly);

	poly_t centershift(poly_t poly);

	poly_t rotate(poly_t poly, int rotations);

	poly_t shifttozero(poly_t poly);

	poly_t shift(poly_t poly, int dx, int dy);


}
#endif
