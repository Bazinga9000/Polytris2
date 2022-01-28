#pragma once
#include "headers.h"
#include "stateengine.h"
#include "state.h"

typedef std::vector<std::vector<int>> board_t;


struct soundholder {
	std::string name;
	sf::SoundBuffer buffer;

	soundholder(std::string n) {
		name = n;
		if (!buffer.loadFromFile("sounds/" + n + ".wav")) {
			std::cout << "Couldn't load sound " << n << "\n";
		}
	}
};

struct colortext {
	colortext(sf::String t, sf::Color c, int f) {
		text = t;
		color = c;
		framesleft = f;
	}

	sf::String text;
	sf::Color color;
	int framesleft;
};

class BasicMode: public State {
public:
	BasicMode(std::vector<bool> eorders, StateEngine* game);

	void handle_events(StateEngine* game);
	void update(StateEngine* game);
	void draw(StateEngine* game);

	void ChangeState(StateEngine* game,
		State* state) {
		game->change_state(state);
	};



	std::vector<std::string> soundnames;
	std::vector<soundholder> buffers;
	std::deque<sf::Sound> sounds;
	int volume;

	void playsound(std::string name);

	sf::Clock frameclock;
	sf::Clock elapsedclock;
	sf::Clock lock_clock;
	bool locking = false;
	bool soft_drop = false;
	double gravity_timer = 0;
	int framecount = 0;
	bool paused = false;

	Polyomino currentpoly;
	board_t board;

	int boardwidth;
	int boardheight;
	int maxheight;

	bool alive = true;


	int millisecondspassed;

	int score = 0;
	int level = 0;
	int lines = 0;
	int combo = -1;

	std::vector<std::vector<int>> bag;
	std::vector<int> orderbag;

	std::vector<int> orders;


	double gravity;
	sf::Time lockdelay;

	int dasstart;
	int arrtimer;
	int dasdirection;

	int das;
	int arr;
	

	std::deque<Polyomino> nextqueue;

	//graphical things
	sf::Texture polyimage;

	std::vector<sf::Color> polycolors = {
		sf::Color(0, 236, 236),
		sf::Color(0, 100, 236),
		sf::Color(229, 137, 27),
		sf::Color(236, 236, 0),
		sf::Color(32, 204, 32),
		sf::Color(146, 0, 219),
		sf::Color(236, 0, 0),
		sf::Color(0, 0, 172),
		sf::Color(236, 172, 0),
		sf::Color(236, 236, 236),
		sf::Color(130, 206, 236),
		sf::Color(236, 202, 111),
		sf::Color(142, 0, 0),
		sf::Color(0, 116, 0),
		sf::Color(130, 130, 0),
		sf::Color(236, 236, 117),
		sf::Color(126, 68, 0),
		sf::Color(236, 163, 53),
		sf::Color(33, 31, 31),
		sf::Color(0, 236, 142),
		sf::Color(89, 0, 236),
		sf::Color(230, 167, 236),
		sf::Color(154, 19, 165),
		sf::Color(169, 236, 140),
		sf::Color(246, 139, 116),
		sf::Color(214, 30, 222),
		sf::Color(254, 250, 54),
		sf::Color(54, 246, 201),
		sf::Color(68, 206, 245),
		sf::Color(128, 222, 30),
		sf::Color(54, 58, 254),
		sf::Color(246, 54, 99),
		sf::Color(245, 107, 68),
		sf::Color(118, 118, 118),
		sf::Color(33, 31, 31),

	};


	sf::Font font;

	double pixelscale;
	double barheight;

	Polyomino held;
	bool hasheld;
	bool holdblock;

	std::vector<bool> enabledorders;
	int maxorder;

	void draw(sf::RenderWindow &window);

	void drawboardpolyomino(int skin, sf::RenderWindow* window, Polyomino polyomino, double pixelscale, bool translucent, int xoffset = 0, int yoffset = 0);
	void drawnextpolyomino(int skin, sf::RenderWindow* window, Polyomino polyomino, double pixelscale, int x, int y);

	std::vector<colortext> dtext;

	std::array<int, 2> gettexture(int colorid);

	//game functions
	void flatten(Polyomino &poly);

	void imprint(Polyomino poly);

	bool canmove(Polyomino polyomino, int dx, int dy);

	void movepoly(Polyomino &polyomino, int dx, int dy);

	bool rotate(Polyomino &polyomino, int numrots);

	void newpoly();

	Polyomino getpoly();

	void clearlines();

	void harddrop(Polyomino &poly);

	void fillbags();

	std::wstring namepoly(Polyomino &poly);
};