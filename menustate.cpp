#include "menustate.h"
#include "headers.h"
#include "gamemodes.h"

Menu::Menu(StateEngine* game) 
	: state_basic({ 0,0,0,0,0,0,0,0,0,0,0,0 }, game),
	  state_cascade({ 0,0,0,0,0,0,0,0,0,0,0,0 }, game) {


	enabledorders = { 0,0,0,1,0,0,0,0,0,0,0,0 };
	
	//to be overridden when a game is begun
	//bootstate = BasicMode({ 0,0,0,0,0,0,0,0,0,0,0,0 }, game);

	//Build The Menu Tree

	//Root Nodes
	menu_node basicsurvival("Basic Survival", "Survive against the doubling gravity for as long as you can.",BasicSurvival);
	menu_node endurance("Endurance Modes", "See how long you can survive.");
	menu_node marathon("Marathon Modes", "Reach a certain criteria, be it score or lines.");
	menu_node race("Race Modes", "Race against the clock.");
	menu_node master("Master Modes", "Prove your might.");
	menu_node esoteric("Esoteric Modes", "Random modes with interesting challenges.");
	menu_node settings("Settings", "Configure the game to your liking.");
	menu_node allmodes("All Modes", "Every mode avaliable in Polytris.");
	menu_node quit("Quit", "Exit Polytris.",QUIT);


	//Endurance
	menu_node cascade("Cascade", "Each block falls individually. Clear massive amounts of lines with one piece.",Cascade);

	endurance.children = { basicsurvival, cascade};
	//Race

	//Master

	//Esoteric

	//All Modes
	for (auto m : { endurance,race,master,esoteric }) {
		for (auto gm : m.children) {
			allmodes.children.push_back(gm);
		}
	}


	//Settings
	menu_node controls("Controls", "Change in-game controls.");
	auto ml = menu_node("Move Left", "", MOVE_LEFT);
	auto mr = menu_node("Move Right", "", MOVE_RIGHT);
	auto rl = menu_node("Rotate Left", "", ROTATE_LEFT);
	auto rr = menu_node("Rotate Right", "", ROTATE_RIGHT);
	auto sd = menu_node("Soft Drop", "", SOFT_DROP);
	auto hd = menu_node("Hard Drop", "", HARD_DROP);
	auto hl = menu_node("Hold", "", HOLD);
	auto pa = menu_node("Pause", "", PAUSE);
	controls.children = { ml,mr,rl,rr,sd,hd,hl,pa };


	menu_node tuning("Game Tuning", "Change how the game plays.");
	auto das = menu_node("DAS", "How long an arrow must be held to auto-move.",DAS);
	auto arr = menu_node("ARR", "How long the interval is in between auto-moves.",ARR);
	tuning.children = { das,arr };


	menu_node appearance("Appearance", "Change how the game looks and sounds.");
	auto pskin = menu_node("Polyomino Skin", "The look of the polyominoes",POLYSKIN);
	auto volume = menu_node("Volume", "The loudness of sounds.", VOLUME);
	appearance.children = { volume,pskin };

	settings.children = { controls,tuning,appearance };

	//Setup
	modepos = { 1 };
	root.children = { basicsurvival,endurance,race,master,esoteric,allmodes,settings,quit };
	

	if (!polyimage.loadFromFile("poly.png"))
	{
		std::cout << "Error Loading Texture poly.png";
		// error...
	}
	maxpolyskin = (polyimage.getSize().y)/8;

}



void Menu::handle_events(StateEngine* game) {
	sf::Event event;
	while (game->m_window->pollEvent(event))
	{
		switch (event.type) {

		case sf::Event::Closed:
			game->quit();
			break;

		case sf::Event::KeyPressed:

			if (inputmode == SINGLEKEY) {
				settingcodes[custominputtarget] = event.key.code;
				send_settings(game);
				inputmode = NONE;
			}
			else if (inputmode == NUMERIC) {
				if (event.key.code == sf::Keyboard::Return) {
					inputmode = NONE;
				}
				int code = static_cast<int>(event.key.code);
				if (event.text.unicode > 25 && event.text.unicode < 36) {
					settingcodes[custominputtarget] *= 10;
					settingcodes[custominputtarget] += event.text.unicode - 26;
					if (custominputtarget != 10) {
						settingcodes[custominputtarget] = std::min(99, settingcodes[custominputtarget]);
					} else {
						settingcodes[custominputtarget] = std::min(100, settingcodes[custominputtarget]);
					}
					send_settings(game);

					std::cout << "code" << settingcodes[custominputtarget] << "\n";
				}

				if (event.key.code == sf::Keyboard::BackSpace) {
					settingcodes[custominputtarget] /= 10;
					send_settings(game);
				}
			}
			else {
				int size = (getcurrentnode()->children.size() + 1);
				switch (event.key.code) {
				case sf::Keyboard::Down:
					modepos.back() = (modepos.back() + 1) % size;
					break;

				case sf::Keyboard::Up:
					modepos.back() = (modepos.back() - 1) % size;
					if (modepos.back() == -1) modepos.back() = size - 1;
					break;

				case sf::Keyboard::X:
					if (modepos.back() != 0) {
						menu_node* currentnode = getcurrentnode();
						if (getpointingnode()->has_children()) {
							modepos.push_back(1);
						}
						else {
							if (getpointingnode()->mode != NO_MODE) {
								boot_mode(game, getpointingnode()->mode);
							}
							else if (getpointingnode()->setting != NOTHING) {
								int ind = static_cast<int>(getpointingnode()->setting);
								switch (getpointingnode()->setting) {

									case MOVE_LEFT:
									case MOVE_RIGHT:
									case ROTATE_LEFT:
									case ROTATE_RIGHT:
									case SOFT_DROP:
									case HARD_DROP:
									case HOLD:
									case PAUSE:
										inputmode = SINGLEKEY;
										custominputtarget = ind;
										break;
									case DAS:
									case ARR:
									case VOLUME:
										inputmode = NUMERIC;
										custominputtarget = ind;
										break;

								}
							}
						}
					}
					else {
						if (std::count(enabledorders.begin(), enabledorders.end(), false) != 11 || !enabledorders[enabledorderpos]) {
							enabledorders[enabledorderpos] = !enabledorders[enabledorderpos];
						}
					}
					break;
				case sf::Keyboard::Z:
					if (modepos.size() > 1) {
						modepos.pop_back();
					}
					break;
				case sf::Keyboard::Left:
					if (modepos.back() == 0) {
						enabledorderpos = (enabledorderpos - 1) % 12;
						if (enabledorderpos == -1) enabledorderpos = 11;
					}
					else {
						switch (getpointingnode()->setting) {
						case DAS:
							settingcodes[7] = std::min(99,std::max(settingcodes[7] - 1, 0));
							break;
						case ARR:
							settingcodes[8] = std::min(99,std::max(settingcodes[8] - 1, 0));
							break;
						case POLYSKIN:
							settingcodes[9] = (settingcodes[9] - 1);
							if (settingcodes[9] == -1) settingcodes[9] = maxpolyskin - 1;
							break;
						case VOLUME:
							settingcodes[10] -= 1;
							if (settingcodes[10] < 0) settingcodes[10] = 100;
							break;
						}
						send_settings(game);

					}
					break;
				case sf::Keyboard::Right:
					if (modepos.back() == 0) {
						enabledorderpos = (enabledorderpos + 1) % 12;
					}
					else {
						switch (getpointingnode()->setting) {
						case DAS:
							settingcodes[7] += 1;
							if (settingcodes[7] > 99) settingcodes[7] = 99;
							break;
						case ARR:
							settingcodes[8] += 1;
							if (settingcodes[8] > 99) settingcodes[8] = 99;
							break;
						case POLYSKIN:
							settingcodes[9] += 1;
							settingcodes[9] %= maxpolyskin;
							break;
						case VOLUME:
							settingcodes[10] += 1;
							if (settingcodes[10] > 100) settingcodes[10] = 100;
						}
						send_settings(game);

					}
					break;
				}
			}
		}
	}
}



void Menu::draw(StateEngine* game) {
	sf::Font font;
	if (!font.loadFromFile("font.ttf"))
	{
		std::cout << "Error Loading Font";
		// error...
	}

	game->m_window->clear();


	sf::Text text;
	text.setFont(font);

	int width = game->m_window->getSize().x;

	//title
	text.setString("Polytris");
	text.setCharacterSize(50);
	text.setPosition((width - text.getLocalBounds().width)/2,10);
	text.setFillColor(sf::Color::White);
	text.setStyle(sf::Text::Bold); //| sf::Text::Underlined);
	game->m_window->draw(text);

	text.setCharacterSize(30);
	text.setStyle(sf::Text::Regular);

	int position = 100;


	//Enabled Orders
	text.setString("Enabled Orders: ");
	text.setPosition(50, position);
	game->m_window->draw(text);


	int xpos = 50 + text.getLocalBounds().width;
	for (int i = 0; i < 12; i++) {
		text.setString(std::to_string(i + 1));
		text.setPosition(xpos, position);
		if (i == enabledorderpos && modepos.back() == 0) {
			text.setStyle(sf::Text::Underlined);
		} else {
			text.setStyle(sf::Text::Regular);
		}


		if (enabledorders[i]) {
			text.setFillColor(sf::Color::Green);
		} else {
			text.setFillColor(sf::Color::Red);
		}

		game->m_window->draw(text);
		xpos += text.getLocalBounds().width;
		text.setString(" ");
		xpos += text.getLocalBounds().width;


	}
	text.setString("12");
	position += text.getLocalBounds().height + 10;
	text.setStyle(sf::Text::Regular);

	//Children
	std::string desc;
	bool drawpolypreview = false;
	for (int i = 0; i < getcurrentnode()->children.size(); i++) {
		auto n = getcurrentnode()->children[i];
	
		text.setString(n.name);

		if (n.setting != NOTHING) {
			int ind = static_cast<int>(n.setting);
			switch (n.setting) {
			case MOVE_LEFT:
			case MOVE_RIGHT:
			case ROTATE_LEFT:
			case ROTATE_RIGHT:
			case SOFT_DROP:
			case HARD_DROP:
			case HOLD:
			case PAUSE:
				text.setString(n.name + " - " + getKeyName(static_cast<sf::Keyboard::Key>(settingcodes[ind])));
				break;
			case DAS:
			case ARR:
			case VOLUME:
				text.setString(n.name + " - " + std::to_string(settingcodes[ind]));
				break;
			case POLYSKIN:
				text.setString(n.name + " - " + std::to_string(settingcodes[ind]));
				drawpolypreview = true;
				break;
			}
		}

		text.setPosition(50, position);
		if (modepos.back() == i + 1) {
			text.setFillColor(sf::Color::Green);
			desc = n.description;
		} else {
			text.setFillColor(sf::Color::White);
		}
		position += text.getLocalBounds().height + 10;
		game->m_window->draw(text);

	}

	//Description
	position += text.getLocalBounds().height;
	text.setString(desc);
	text.setCharacterSize(12);
	text.setPosition(50, position);
	text.setStyle(sf::Text::Italic);
	text.setFillColor(sf::Color::White);
	game->m_window->draw(text);

	position += text.getLocalBounds().height;
	//keyboardinputmode

	if (inputmode != NONE) {
		switch (inputmode) {
		case SINGLEKEY:
			text.setString("Input Key for " + settingnames[custominputtarget]);
			break;
		case NUMERIC:
			text.setString("Input Value for " + settingnames[custominputtarget]);
			break;
		}
		
		text.setCharacterSize(30);
		text.setPosition(50, position);
		text.setStyle(sf::Text::Italic);
		text.setFillColor(sf::Color::White);
		game->m_window->draw(text);
	}


	//polypreview
	if (drawpolypreview) {
		position += 50;
		sf::Sprite bsprite;
		
		for (int i = 0; i < 5; i++) {
			bsprite.setTexture(polyimage);
			bsprite.setTextureRect(sf::IntRect(i*7*8, settingcodes[9] * 8, 8 * 7, 8));

			bsprite.setPosition(sf::Vector2f(50, position));
			bsprite.setScale(3, 3);
			game->m_window->draw(bsprite);
			position += 24;

		}
	}


	game->m_window->display();
}


void Menu::update(StateEngine* game) {
	settingcodes = refresh_settings(game);
}



void Menu::boot_mode(StateEngine* game, Mode m) {
	switch (m) {
	case BasicSurvival: {
		state_basic = BasicMode(enabledorders, game);
		game->push_state(&this->state_basic);
		break;
	}
	case Cascade: {
		state_cascade = CascadeMode(enabledorders, game);
		game->push_state(&this->state_cascade);
		break;
	}
	case QUIT: {
		game->quit();
	}
	default:
		break;
	}


}


menu_node* Menu::getcurrentnode() {
	menu_node* n = &root;
	for (int i = 0; i < modepos.size() - 1; i++) {
		n = &n->children[modepos[i] - 1];
	}
	return n;
}


menu_node* Menu::getpointingnode() {
	menu_node* n = getcurrentnode();
	return &n->children[modepos.back() - 1];
}