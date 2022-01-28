#include "cascademode.h"
#include "headers.h"
#include <chrono>
#include <thread>

CascadeMode::CascadeMode(std::vector<bool> eorders, StateEngine* game) {

	if (std::count(eorders.begin(),eorders.end(),false) == 12) return;

	soundnames = { "piece0","piece1","piece2","piece3","piece4","piece5","piece6","piece7","piece8","piece9","piece10",
				   "gameover","harddrop","hold","holdfail","levelup","move","movefail","pause","rotate","rotfail","softdrop",
				   "step","erase1","erase2","erase3","erase4" };



	for (auto i : soundnames) {
		buffers.push_back(soundholder(i));
	}

	volume = game->volume;


	enabledorders = eorders;

	//1,2,3,4,5,6,8,9,10,12,15,18,20,24,30,36,40,45,60,72,90,120,180,360
	//1,2,3,4,5,6,8,9,10,12,15,16,18,20,24,30,36,40,45,48,60,72,80,90,120,144,180,240,360,720

	int widths[12] = { 10,10,10,10,12,15,18,20,24,30,36,40 };
	int heights[12] = { 20,20,20,20,24,30,36,40,48,60,72,80 };

	int size = 0;
	maxorder = 0;
	for (int i = 0; i < 12; i++) {
		if (enabledorders[i]) {
			size = i;
			orders.push_back(i + 1);
			if (i + 1 > maxorder) {
				maxorder = i + 1;
			}
		}
	}

	fillbags();

	shuffle(orderbag);

	boardwidth = widths[size];
	boardheight = heights[size];
	maxheight = heights[size] * 2;
	barheight = 0;

	millisecondspassed = 0;

	elapsedclock.restart();

	pixelscale = std::min(360.0f / boardwidth, 720.0f / boardheight) / 8.0f;
	for (int i = 0; i < maxheight; i++) {
		std::vector<int> newrow;

		for (int j = 0; j < boardwidth; j++) {
			newrow.push_back(-1);
		}

		board.push_back(newrow);
	}


	gravity = 1 / 64;
	lockdelay = sf::milliseconds(400);
	das = game->das;
	arr = game->arr;




	if (!polyimage.loadFromFile("poly.png"))
	{
		std::cout << "Error Loading Texture poly.png";
		// error...
	}


	if (!font.loadFromFile("font.ttf"))
	{
		std::cout << "Error Loading Font font.ttf";
		// error...
	}

	nextqueue = {};
	held;
	hasheld = false;
	holdblock = false;
	newpoly(game);
}





//THE IMPORTANT SHIT


void CascadeMode::draw(StateEngine* game) {
	
	game->m_window->clear();

	//gray half
	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(360, 720));
	rectangle.setFillColor({ 128, 128, 128, 255 });
	rectangle.setPosition(sf::Vector2f(360, 0));
	game->m_window->draw(rectangle);

	//board
	for (int i = boardheight; i < maxheight; i++) {
		for (int j = 0; j < boardwidth; j++) {

			double x = (j * pixelscale * 8);
			double y = ((i - boardheight) * pixelscale * 8);

			if (board[i][j] != -1) { //actually draw the grid block

				int txture = 0;

				/*
				auto colors = gettexture(board[i][j]);

				if (!alive) colors = { 7,5 };

				sf::Sprite bsprite;
				bsprite.setTexture(bricks);
				bsprite.setTextureRect(sf::IntRect(colors[0] * 8, colors[1] * 8, 8, 8));
				*/

				int color = board[i][j];

				if (!alive) {
					color = 33;
				}

				sf::Sprite bsprite;
				bsprite.setTexture(polyimage);
				bsprite.setTextureRect(sf::IntRect(color * 8, game->polyskin * 8, 8, 8));


				bsprite.setPosition(sf::Vector2f(x, y));
				bsprite.setScale(sf::Vector2f(pixelscale, pixelscale));
				game->m_window->draw(bsprite);
			}
			else {

				/*
				//draw gridline
				sf::RectangleShape rectangle;
				rectangle.setSize(sf::Vector2f(pixelscale * 8, pixelscale * 8));
				rectangle.setOutlineColor(sf::Color::White);
				rectangle.setOutlineThickness(1);
				rectangle.setFillColor({ 0, 0, 0, 0 });
				rectangle.setPosition(sf::Vector2f(x,y));
				game->m_window->draw(rectangle);
				*/
			}
		}
	}

	//bar

	rectangle.setSize(sf::Vector2f(15, 720 * barheight));
	if (barheight <= 0.33) {
		rectangle.setFillColor(sf::Color(128,0,0));
	} else if (barheight <= 0.66) {
		rectangle.setFillColor(sf::Color(128,128,0));
	} else {
		rectangle.setFillColor(sf::Color(0,128,0));
	}
	rectangle.setPosition(sf::Vector2f(360,720*(1-barheight)));
	game->m_window->draw(rectangle);

	rectangle.setSize(sf::Vector2f(15, 720));
	rectangle.setFillColor({ 0, 0, 0, 0});
	rectangle.setOutlineColor(sf::Color::White);
	rectangle.setOutlineThickness(1);
	rectangle.setPosition(sf::Vector2f(360, 0));

	game->m_window->draw(rectangle);

	//polyominoes
	drawboardpolyomino(game->polyskin, game->m_window, currentpoly, pixelscale, false);


	
	Polyomino ghostpoly = currentpoly;
	harddrop(ghostpoly);
	drawboardpolyomino(game->polyskin, game->m_window, ghostpoly, pixelscale, true);


	sf::Text text;
	text.setFont(font);


	//level
	text.setString("Level: " + std::to_string(level));
	text.setCharacterSize(24);
	text.setPosition(400, 50);
	text.setFillColor(sf::Color::Black);
	text.setStyle(sf::Text::Bold); //| sf::Text::Underlined);
	game->m_window->draw(text);


	//score
	text.setPosition(400, text.getPosition().y + text.getCharacterSize());
	text.setString("Score: " + std::to_string(score));
	game->m_window->draw(text);

	//lines
	text.setPosition(400, text.getPosition().y + text.getCharacterSize());
	text.setString("Lines: " + std::to_string(lines));
	game->m_window->draw(text);

	//gravity
	text.setPosition(400, text.getPosition().y + text.getCharacterSize());
	text.setString("Gravity: " + std::to_string(gravity) + " G");
	game->m_window->draw(text);

	//lockdelay
	text.setPosition(400, text.getPosition().y + text.getCharacterSize());
	text.setString("Lock Delay: " + std::to_string(lockdelay.asMilliseconds()) + "ms");
	game->m_window->draw(text);

	//time
	text.setPosition(400, text.getPosition().y + text.getCharacterSize());

	int milliseconds = millisecondspassed % 1000;
	int seconds = ((millisecondspassed - milliseconds) / 1000);
	int minutes = (seconds - (seconds % 60)) / 60;
	seconds = seconds % 60;

	std::string msstring = std::to_string(milliseconds);
	std::string sstring = std::to_string(seconds);
	std::string mstring = std::to_string(minutes);

	msstring = std::string(3 - msstring.length(), '0') + msstring;
	sstring = std::string(2 - sstring.length(), '0') + sstring;
	
	if (minutes <= 100) {
		mstring = std::string(2 - mstring.length(), '0') + mstring;
	}

	text.setString("Elapsed Time: " + mstring + ":" + sstring + "." + msstring);
	text.setCharacterSize(24);
	text.setFillColor(sf::Color::Black);
	text.setStyle(sf::Text::Bold); //| sf::Text::Underlined);
	game->m_window->draw(text);


	int coordinate = 250; //allocate room for held
						  //heldpoly

	text.setString("Hold");
	text.setPosition(400, coordinate);
	game->m_window->draw(text);

	coordinate += text.getCharacterSize() + 15;

	if (hasheld) {
		held.shape = poly::shifttozero(held.shape);
		drawnextpolyomino(game->polyskin, game->m_window, held, 1.125f, 400, coordinate);
		coordinate += (8 * 1.125f * (held.height + 1));
	}
	else {
		coordinate = 300;

	}

	text.setString("Next");
	text.setPosition(400, coordinate);
	game->m_window->draw(text);

	coordinate += text.getCharacterSize() + 10;


	//nextpolyomino
	for (Polyomino next : nextqueue) {
		next.shape = poly::shifttozero(next.shape);
		drawnextpolyomino(game->polyskin, game->m_window, next, 1.125f, 400, coordinate);
		coordinate += (8 * 1.125f * (next.height + 1));
	}

	/*	Code to view center of rotation
	sf::RectangleShape rect;
	rect.setFillColor(sf::Color::Black);
	rect.setOutlineColor(sf::Color::White);
	rect.setOutlineThickness(1);
	rect.setSize(sf::Vector2f(pixelscale * 2, pixelscale * 2));
	rect.setPosition(sf::Vector2f((currentpoly.column + 0.5) * (pixelscale * 8), (currentpoly.row - boardheight + 0.5) * (pixelscale * 8)));
	game->m_window->draw(rect);
	*/

	//dtext

	int xc = 400;
	text.setCharacterSize(25);
	for (auto i : dtext) {
		text.setOutlineColor(sf::Color(0, 0, 0));
		text.setOutlineThickness(2);
		text.setPosition(xc, 600);
		text.setString(i.text);
		text.setFillColor(i.color);
		game->m_window->draw(text);
		xc += text.getLocalBounds().width;
	}

	game->m_window->display();

}

void CascadeMode::handle_events(StateEngine* game) {

	
	sf::Event event;
	while (game->m_window->pollEvent(event))
	{
		switch (event.type) {

		case sf::Event::Closed:
			game->quit();
			break;

		case sf::Event::KeyPressed:
			if (alive == false && event.key.code == sf::Keyboard::Return) {
				game->pop_state();

			} else if (alive) {
				if (event.key.code == game->pause) {
					paused = !paused;
				} 

				if (paused) break;
				else if (event.key.code == game->harddrop) {
					harddrop(currentpoly);
					playsound("harddrop");
					newpoly(game);
				}
				else if (event.key.code == game->movel) {
					movepoly(currentpoly, -1, 0);
					dasstart = framecount;
					dasdirection = -1;
					playsound("move");

				}
				else if (event.key.code == game->mover) {
					movepoly(currentpoly, 1, 0);
					dasstart = framecount;
					dasdirection = 1;
					playsound("move");

				}
				else if (event.key.code == game->rotr) {
					if (rotate(currentpoly, 1)) {
						playsound("rotate");
					}
					else {
						playsound("rotfail");
					}

				}
				else if (event.key.code == game->rotl) {
					if (rotate(currentpoly, 3)) {
						playsound("rotate");
					}
					else {
						playsound("rotfail");
					}
				}
				else if (event.key.code == game->hold) {
					if (!holdblock) {
						holdblock = true;
						if (hasheld) {
							std::swap(currentpoly, held);

						}
						else {
							hasheld = true;
							held = currentpoly;
							currentpoly.shape = {};
							newpoly(game);

						}

						held = poly::getpolyfromid(held.shape.size(), held.polyid);
						held.row = boardheight - held.height;
						held.column = (boardwidth - (held.width / 2)) / 2;
						currentpoly.shape = poly::centershift(currentpoly.shape);
						flatten(held);
						playsound("hold");

					}
					else {
						playsound("holdfail");
					}
				}
				else if (event.key.code == game->softdrop) {
					soft_drop = true;
				}
			}
			break;

		case sf::Event::KeyReleased:
			if (event.key.code == game->softdrop) {
				soft_drop = false;
			}

			break;
		case sf::Event::TextEntered:
			if (event.text.unicode < 128) {
				//ascii shit goes here
				break;

			}
			break;


		default:
			break;
		}

	}

	bool leftpressed = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(game->movel));
	bool rightpressed = sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(game->mover));

	dasdirection = 0;
	if (leftpressed) { dasdirection = -1; }
	if (rightpressed) { dasdirection = 1; }
	if (!leftpressed && !rightpressed) dasstart = -1;


	}



void CascadeMode::update(StateEngine* game) {
	//remove finished sounds
	if (sounds.size() > 0) {
		while (sounds.front().getStatus() == sf::Sound::Stopped) {
			sounds.pop_front();
			if (sounds.size() == 0) break;
		}
	}

	if (paused) {
		dtext = { colortext("Paused",sf::Color::White,1) };
	}

	if ((frameclock.getElapsedTime() > sf::seconds(0.0166666666)) && !paused) {
		//std::cout << std::to_string(clock.getElapsedTime().asMilliseconds()) << " " << gravity << " " << gravity_timer << soft_drop << "\n";
		frameclock.restart();

		//update dtext
		int i = 0;
		while (i != dtext.size()) {
			dtext[i].framesleft -= 1;

			if (dtext[i].framesleft == 0) {
				dtext.erase(dtext.begin() + i);
			}
			else { ++i; }
		}

		framecount++;
		

		//DAS and ARR
		if (alive) {
			if (framecount - dasstart > das && dasstart != -1) {
				arrtimer += 1;
				while (arrtimer >= arr) {
					if (!canmove(currentpoly, dasdirection, 0)) {
						arrtimer = 0;
						break;
					}
					playsound("move");
					movepoly(currentpoly, dasdirection, 0);
					arrtimer -= arr;
				}
			}
			else {
				arrtimer = 0;
			}

			//GRAVITY
			millisecondspassed = elapsedclock.getElapsedTime().asMilliseconds();
			gravity_timer += gravity;

			if (soft_drop) {
				movepoly(currentpoly, 0, 1);
			}


			else if (gravity_timer >= 1) {
				//std::cout << "Gravity ticked after " << framecount << " frames.\n";
				//framecount = 0;
				while (gravity_timer >= 1) {
					if (canmove(currentpoly, 0, 1)) {
						movepoly(currentpoly, 0, 1);
					}
					else {
						gravity_timer = gravity_timer - static_cast<int>(gravity_timer);
						break;
					}
					gravity_timer -= 1;
				}

				gravity_timer = 0;
			}

			if (canmove(currentpoly, 0, 1) == false) {
				if (locking == false) {
					lock_clock.restart();
					locking = true;
				}
			}
			else {
				locking = false;
				lock_clock.restart();
			}

			if (locking && (lock_clock.getElapsedTime() > lockdelay)) {
				locking = false;
				newpoly(game);
			}
		}
	}
}


//draw helper functions
void CascadeMode::drawboardpolyomino(int skin, sf::RenderWindow* window, Polyomino polyomino, double pixelscale, bool translucent, int xoffset, int yoffset) {


	for (int i = 0; i < polyomino.order; ++i) {
		std::vector<short> block = polyomino.shape[i];

		sf::Sprite bsprite;
		bsprite.setTexture(polyimage);
		bsprite.setTextureRect(sf::IntRect(polyomino.color * 8, skin * 8, 8, 8));
		bsprite.setPosition(sf::Vector2f((block[0] + polyomino.column + xoffset) * (pixelscale * 8), (block[1] + polyomino.row + yoffset - boardheight) * (pixelscale * 8)));
		if (translucent) {
			bsprite.setColor(sf::Color(255, 255, 255, 128));
		}
		bsprite.setScale(sf::Vector2f(pixelscale, pixelscale));

		window->draw(bsprite);

	}


	/* uncomment for center of rotation to be visible */


}

void CascadeMode::drawnextpolyomino(int skin, sf::RenderWindow* window, Polyomino polyomino, double pixelscale, int x, int y) {
	for (int i = 0; i < polyomino.order; ++i) {

		std::vector<short> block = polyomino.shape[i];

		sf::Sprite bsprite;		
		bsprite.setTexture(polyimage);
		bsprite.setTextureRect(sf::IntRect(polyomino.color * 8, skin * 8, 8, 8));
		bsprite.setPosition(sf::Vector2f(x + (block[0]) * (pixelscale * 8), y + (block[1]) * (pixelscale * 8)));
		bsprite.setScale(sf::Vector2f(pixelscale, pixelscale));

		window->draw(bsprite);

	}

}

void CascadeMode::playsound(std::string name) {
	for (auto i = 0; i < soundnames.size(); i++) {
		if (soundnames[i] == name) {
			sounds.push_back(sf::Sound());
			sounds.back().setBuffer(buffers[i].buffer);
			sounds.back().setVolume(volume);
			sounds.back().play();
		}
	}
}


//obsolete don't use
std::array<int, 2> CascadeMode::gettexture(int colorid) {
	std::array<int, 2> colors;

	if (colorid <= 6) {
		colors[1] = 0;
		colors[0] = colorid;
	}
	else if (colorid <= 13) {
		colors[1] = 1;
		colors[0] = colorid - 7;
	}
	else if (colorid <= 20) {
		colors[1] = 2;
		colors[0] = colorid - 14;
	}
	else if (colorid <= 28) {
		colors[1] = 3;
		colors[0] = colorid - 21;
	}
	else if (colorid <= 32) {
		colors[1] = 4;
		colors[0] = colorid - 29;
	}
	else if (colorid <= 36) {
		colors[1] = 5;
		colors[0] = colorid - 33;
	}
	else if (colorid == 98) {
		colors[1] = 5;
		colors[0] = 5;
	}
	else {
		colors[1] = 5;
		colors[0] = 7;
	}

	return colors;
}

//game functions


Polyomino CascadeMode::getpoly() {

	//int order = orders[randint(0, orders.size() - 1)];
	//Polyomino poly = poly::randompoly(order);

	int order = orderbag.back();
	orderbag.pop_back();
	if (orderbag.size() == 0) {
		for (auto i : orders) {
			orderbag.push_back(i - 1);
		}
		shuffle(orderbag);
	}

	int polyid = bag[order].back();
	bag[order].pop_back();
	if (bag[order].size() == 0) {
		for (int i = 0; i < polylengths[order]; i++) {
			bag[order].push_back(i);
		}
		shuffle(bag[order]);
	}


	Polyomino poly = poly::getpolyfromid(order + 1, polyid);

	poly.row = boardheight;
	poly.column = (boardwidth - (poly.width / 2)) / 2;
	poly.shape = poly::centershift(poly.shape);
	flatten(poly);

	return poly;
}

void CascadeMode::imprint(Polyomino poly) {
	int color = poly.color;



	for (int i = 0; i < poly.shape.size(); i++) {

		int r = poly.shape[i][1] + poly.row;
		int c = poly.shape[i][0] + poly.column;
		board[r][c] = color;
	}
}

bool CascadeMode::canmove(Polyomino polyomino, int dx, int dy) {
	for (int i = 0; i < polyomino.shape.size(); i++) {

		int x = polyomino.shape[i][0] + polyomino.column + dx;
		int y = polyomino.shape[i][1] + polyomino.row + dy;

		if (x < 0 || y < 0 || x >= boardwidth || y >= maxheight) { return false; }

		if (board[y][x] != -1) { return false; }

	}

	return true;

}

void CascadeMode::movepoly(Polyomino &polyomino, int dx, int dy) {
	if (canmove(polyomino, dx, dy)) {
		polyomino.row += dy;
		polyomino.column += dx;
	}
}


void CascadeMode::flatten(Polyomino &poly) {
	int bestrot = 0;
	int minheight = 99999;
	for (int i = 1; i < 5; i++) {
		poly.shape = poly::rotate(poly.shape, 1);
		auto dims = poly::getdims(poly.shape);

		poly.width = dims[0];
		poly.height = dims[1];

		if (poly.height < minheight) {
			bestrot = i;
			minheight = poly.height;
		}
	}

	poly.shape = poly::rotate(poly.shape, bestrot);
	auto dims = poly::getdims(poly.shape);

	poly.width = dims[0];
	poly.height = dims[1];
}

bool CascadeMode::rotate(Polyomino &polyomino, int numrots) {

	std::vector<std::vector<int>> kicks;
	kicks.push_back({ 0,0 });

	if (polyomino.shape.size() < 4) {
		kicks.push_back({ 1,0 });
		kicks.push_back({ -1,0 });
		kicks.push_back({ 0,1 });
		kicks.push_back({ 0,-1 });
	}


	if (polyomino.shape.size() != 1) {
		for (int i = 1; i < (polyomino.shape.size() - 2); i++) {
			std::vector<int> kick = { i,0 }; //right
			kicks.push_back(kick);

			kick = { -i,0 }; //left
			kicks.push_back(kick);

			kick = { 0,i }; //down
			kicks.push_back(kick);

			kick = { i, i }; //diagonal right down
			kicks.push_back(kick);

			kick = { -i, i }; //diagonal left down
			kicks.push_back(kick);

			kick = { 0, (i + 1) }; //very down
			kicks.push_back(kick);

			kick = { i, (i + 1) }; //diagonal right very down
			kicks.push_back(kick);

			kick = { -i, (i + 1) }; //diagonal left very down
			kicks.push_back(kick);

			kick = { 0,-i }; //up
			kicks.push_back(kick);


			/*
			kick = { -i,0 }; //left
			kicks.push_back(kick);

			kick = { -i,0 }; //left
			kicks.push_back(kick);
			*/

		}
	}


	if (polyomino.order == 4 && polyomino.polyid == 6) {
		kicks.push_back({2,0});
		kicks.push_back({-2,0 });
	}

	/*
	for (auto elem : kicks) {
	std::cout << elem[0] << " " << elem[1] << "\n";
	}
	*/

	int rotations = numrots % 4;


	for (int i = 0; i < kicks.size(); i++) {
		Polyomino newpoly = polyomino;

		newpoly.shape = poly::rotate(newpoly.shape, rotations);

		if (canmove(newpoly, kicks[i][0], kicks[i][1]) == true) {
			//std::cout << "Kick " << kicks[i][0] << " " << kicks[i][1] << " Succeeded\n";

			polyomino.shape = newpoly.shape;
			movepoly(polyomino, kicks[i][0], kicks[i][1]);

			auto dims = poly::getdims(polyomino.shape);

			polyomino.height = dims[0];
			polyomino.width = dims[1];

			return true;
		}

		//std::cout << "Kick " << kicks[i][0] << " " << kicks[i][1] << " Failed\n";
	}

	auto dims = poly::getdims(polyomino.shape);

	polyomino.height = dims[0];
	polyomino.width = dims[1];

	return false;

}

void CascadeMode::newpoly(StateEngine *game) {
	clearlines(game);
	while (nextqueue.size() < 6) {
		nextqueue.push_back(getpoly());
	}
	currentpoly = nextqueue.front();
	nextqueue.pop_front();

	playsound("piece" + std::to_string(nextqueue.front().color % 11));

	holdblock = false;

	level++;

	gravity = std::pow(2, level / 60.0) / 60;

	barheight = std::min(gravity / boardheight, 1.0);

	lockdelay = sf::milliseconds(667);

	//formula blessed to me by WA that gives me the level instant gravity will be reached at, given the height
	//what the fuck WA
	int instantgravitythreshold = static_cast<int>(60 * std::log(60 * (boardheight)) / (std::log(2)));

	if (level > instantgravitythreshold) {
		lockdelay = sf::milliseconds(static_cast<int>(667.0f / (1 + (3.0f / 256.0f) * (level - instantgravitythreshold))));
	}


	currentpoly.row = boardheight - currentpoly.height;

	if (canmove(currentpoly, 0, 0) == false) {
		std::cout << "\n-------------------\n";
		std::cout << "       Game Over     \n";
		std::cout << " Level: " << level << "\n";
		std::cout << " Score: " << score << "\n";
		std::cout << " Lines: " << lines << "\n";
		std::cout << " Gravity: " << gravity << "G\n";
		std::cout << " Lock Delay: " << std::to_string(lockdelay.asMilliseconds()) << "ms\n";
		std::cout << "\n-------------------\n";
		playsound("gameover");
		alive = false;
		return;
	}

	if (canmove(currentpoly, 0, 1)) currentpoly.row++;
}

void CascadeMode::harddrop(Polyomino &poly) {
	while (canmove(poly, 0, 1)) {
		poly.row++;
	}
}

void CascadeMode::clearlines(StateEngine *game) {
	//check for spin
	bool spin = !(
		canmove(currentpoly, 0, 1) ||
		canmove(currentpoly, 1, 0) ||
		canmove(currentpoly, 0, -1) ||
		canmove(currentpoly, -1, 0)
		);


	imprint(currentpoly);

	/*

	1) init drawing variables

	while loopcleared isnt 0
		check for full lines
		increment counters
		clear full lines
		update drawings
		drop everything down cascade style

	increment scores/levelbonuses/etc etc


	*/

	std::vector<std::string> names = {
		"Spin ","Single ","Double ","Triple ","Tesseris ","Pentris ","Hextris ","Heptris ","Octris ","Ennetris ","Decatris ",
		"Hendecatris ","Dodecatris ","Triskaidecatris ","Tetrakaidecatris ","Pentakaidecatris ","Hexakaidecatris ","Heptakaidecatris ",
		"Octakaidecatris ","Enneakaidecatris ","Icosatris "
	};


	std::vector<sf::Color> dtextcolors = {
		sf::Color(64, 46, 255),
		sf::Color(97, 40, 249),
		sf::Color(131, 35, 243),
		sf::Color(164, 31, 237),
		sf::Color(197, 26, 231),
		sf::Color(225, 22, 221),
		sf::Color(220, 18, 177),
		sf::Color(214, 14, 134),
		sf::Color(208, 10, 92),
		sf::Color(202, 6, 51),
		sf::Color(196, 3, 11),
		sf::Color(191, 28, 0)
	};

	int linescleared = 0;
	int loopcleared = -1;

	while (loopcleared != 0) {
		loopcleared = 0;
		std::vector<int> flag;

		for (int i = 0; i < boardwidth; i++) {
			flag.push_back(-2); //any row with only -2 in it will be deleted
		}

		int emptycount = 0;
		for (int i = 0; i < maxheight; i++) {
			emptycount = 0;

			for (int j = 0; j < board[i].size(); j++) {
				if (board[i][j] == -1) { emptycount++; }
			}

			if (emptycount == 0) {
				linescleared++;
				loopcleared++;
				for (int j = 0; j < board[i].size(); j++) {
					board[i][j] = -2; //transform row into flag vector
				}
			}
		}


		board.erase(std::remove(board.begin(), board.end(), flag), board.end());

		for (int i = 0; i < loopcleared; i++) {
			std::vector<int> newrow;

			for (int j = 0; j < boardwidth; j++) {
				newrow.push_back(-1);
			}
			
			board.insert(board.begin(), newrow);
		}





		if (loopcleared != 0) {
			

			playsound("erase" + std::to_string(loopcleared));

			for (int q = 0; q < maxheight; q++) {
				for (int i = maxheight - 2; i >= 0; i--) {
					for (int j = 0; j < boardwidth; j++) {
						if (board[i][j] != -1 && board[i+1][j] == -1) {
							board[i+1][j] = board[i][j];
							board[i][j] = -1;
						}
					}
				}
			}
			std::string name;
				if (linescleared <= 20) {
				name = names[linescleared];
			} else {
				name = std::to_string(linescleared) + "tris";
			}

			if (linescleared > 0 && !spin) {
				sf::Color c = dtextcolors[std::min(11,linescleared - 1)];
				dtext = { colortext(name,c,150) };
			}
		
			else if (spin) {
				sf::Color c = polycolors[currentpoly.color];
				dtext = { colortext(sf::String(namepoly(currentpoly)) + sf::String("-" + name),c,150) };
			}


			if (combo >= 1) {
				dtext.push_back(colortext("(" + std::to_string(combo) + " Combo)", sf::Color(196, 100, 3), 150));
			}

			int to = currentpoly.order;
			currentpoly.order = 0;
			draw(game);
			currentpoly.order = to;

			std::this_thread::sleep_for(std::chrono::milliseconds(250));

		}


	}
	int scoredelta = 0;
	double scoremult = 1;

	if (linescleared <= 4 && linescleared != 0) {
		int mults[4] = {100,300,500,800 };
		scoredelta = mults[linescleared - 1];
	}
	else if (linescleared != 0) {
		scoredelta = 100 * std::pow(2, linescleared);
	}


	scoredelta *= ((level / 10) + 1);

	combo += 1;
	if (linescleared == 0) combo = -1;
	if (combo >= 1) scoremult += (combo/2.0);


	if (spin) {
		scoredelta += 50;
		
		if (currentpoly.order > 4) {
			scoremult += 2 * (currentpoly.order - 3) + 1;
		}

	}


	lines += linescleared;
	score += scoredelta;

	int levelbonuses[] = {0, 1, 2, 4, 6, 9, 13, 18, 24, 31, 39, 50, 62, 77, 93, 112, 134, 159, 187, 218, 252, 292, 335, 384, 436, 494, 558, 628, 704, 786, 874, 972, 1076, 1190, 1310, 1440, 1580, 1730, 1890, 2060, 2240, 2435 };

	int leveldelta = levelbonuses[linescleared];

	level += leveldelta;
}

std::wstring CascadeMode::namepoly(Polyomino &poly) {
	if (poly.order > 6) return std::to_wstring(poly.order);
	std::vector<std::wstring> names = {
		L"I",
		L"I",
		L"LI",
		L"TOJSLZI",
		L"QKFXPTYRUJSVNWHLZI",
		L"DðA$αOPδUYЬ%XQτþЯ&ΠЪφREБ§ЧTμλψИKBF@ЦρGη?CLZVH{>Γ<W\\ζMξ}N/JSI"
	};

	return std::wstring(1,names[poly.order - 1][poly.polyid]) + std::to_wstring(poly.order);
}


void CascadeMode::fillbags() {
	bag = {};
	orderbag = {};
	for (int i = 0; i < 12; i++) {
		std::vector<int> b = {};
		if (enabledorders[i]) {
			orderbag.push_back(i);
			for (int j = 0; j < polylengths[i]; j++) {
				b.push_back(j);
			}
			shuffle(b);
		}
		bag.push_back(b);
	}
}