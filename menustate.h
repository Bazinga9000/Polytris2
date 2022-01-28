#pragma once
#include "headers.h"
#include "state.h"
#include "gamemodes.h"
#include "stateengine.h"

typedef std::vector<std::vector<int>> board_t;

enum Mode {
	QUIT = -99,
	NO_MODE = -1,
	BasicSurvival,
	Cascade,
	NUM_MODES,
};


enum SettingsValue {
	NOTHING = -1,
	MOVE_LEFT = 0,
	MOVE_RIGHT,
	ROTATE_LEFT,
	ROTATE_RIGHT,
	SOFT_DROP,
	HARD_DROP,
	HOLD,
	DAS,
	ARR,
	POLYSKIN,
	VOLUME,
	PAUSE
};

enum InputMode {
	NONE = 0,
	SINGLEKEY,
	NUMERIC
};

struct menu_node {
	menu_node() {
		name = "";
		description = "";
	}

	menu_node(std::string n, std::string d) {
		name = n;
		description = d;
	};

	menu_node(std::string n, std::string d, Mode m) {
		name = n;
		description = d;
		mode = m;
	}

	menu_node(std::string n, std::string d, SettingsValue sv) {
		name = n;
		description = d;
		setting = sv;
	}

	bool has_children() {
		return (children.size() != 0);
	}


	menu_node getchildfromname(std::string name) {
		for (auto i : children) {
			if (i.name == name) {
				return i;
			}
		}
		std::cout << "Child of " << this->name << " with name " << name << " not found.";
	}

	std::string name;
	std::string description;
	std::vector<menu_node> children;
	Mode mode = NO_MODE;

	SettingsValue setting = NOTHING;
};

class Menu: public State {
public:
	Menu(StateEngine* game);

	void handle_events(StateEngine* game);
	void update(StateEngine* game);
	void draw(StateEngine* game);

	void ChangeState(StateEngine* game,
		State* state) {
		game->change_state(state);
	};



	int enabledorderpos = 0;
	std::vector<int> modepos;
	std::vector<bool> enabledorders = {false,false,false,false,false,false,false,false,false,false,false,false};

	menu_node root{ "This is root.", "You shouldn't be able to see this." };

	//std::vector<menu_node> location;

	void boot_mode(StateEngine* game, Mode m);

	std::vector<std::string> settingnames = { "Move Left","Move Right","Rotate Left","Rotate Right","Soft Drop","Hard Drop","Hold","DAS","ARR","Polyomino Skin",
											  "Volume","Pause"};
	std::vector<int> settingcodes;


	/*
	To person viewing this code:
	each gamemode has its own state in memory
	if you can find a better way to do this then you are a better programmer than I
	sorry.
	*/

	BasicMode state_basic;
	CascadeMode state_cascade;

	menu_node* getcurrentnode();
	menu_node* getpointingnode();
	
	InputMode inputmode = NONE;
	int custominputtarget = 0;

	sf::Texture polyimage;
	int maxpolyskin;

	void send_settings(StateEngine* game) {
		game->movel = settingcodes[0];
		game->mover = settingcodes[1];
		game->rotl = settingcodes[2];
		game->rotr = settingcodes[3];
		game->softdrop = settingcodes[4];
		game->harddrop = settingcodes[5];
		game->hold = settingcodes[6];
		game->das = settingcodes[7];
		game->arr = settingcodes[8];
		game->polyskin = settingcodes[9];
		game->volume = settingcodes[10];
		game->pause = settingcodes[11];

		game->writeconfig();
	}

	std::vector<int> refresh_settings(StateEngine* game) {
		return { game->movel,game->mover,game->rotl,game->rotr,game->softdrop,game->harddrop,game->hold,
				 game->das, game->arr, game->polyskin, game->volume, game->pause};
	}




	std::string getKeyName(const sf::Keyboard::Key key) {
		switch (key) {
		default:
		case sf::Keyboard::Unknown:
			return "Unknown";
		case sf::Keyboard::A:
			return "A";
		case sf::Keyboard::B:
			return "B";
		case sf::Keyboard::C:
			return "C";
		case sf::Keyboard::D:
			return "D";
		case sf::Keyboard::E:
			return "E";
		case sf::Keyboard::F:
			return "F";
		case sf::Keyboard::G:
			return "G";
		case sf::Keyboard::H:
			return "H";
		case sf::Keyboard::I:
			return "I";
		case sf::Keyboard::J:
			return "J";
		case sf::Keyboard::K:
			return "K";
		case sf::Keyboard::L:
			return "L";
		case sf::Keyboard::M:
			return "M";
		case sf::Keyboard::N:
			return "N";
		case sf::Keyboard::O:
			return "O";
		case sf::Keyboard::P:
			return "P";
		case sf::Keyboard::Q:
			return "Q";
		case sf::Keyboard::R:
			return "R";
		case sf::Keyboard::S:
			return "S";
		case sf::Keyboard::T:
			return "T";
		case sf::Keyboard::U:
			return "U";
		case sf::Keyboard::V:
			return "V";
		case sf::Keyboard::W:
			return "W";
		case sf::Keyboard::X:
			return "X";
		case sf::Keyboard::Y:
			return "Y";
		case sf::Keyboard::Z:
			return "Z";
		case sf::Keyboard::Num0:
			return "Num0";
		case sf::Keyboard::Num1:
			return "Num1";
		case sf::Keyboard::Num2:
			return "Num2";
		case sf::Keyboard::Num3:
			return "Num3";
		case sf::Keyboard::Num4:
			return "Num4";
		case sf::Keyboard::Num5:
			return "Num5";
		case sf::Keyboard::Num6:
			return "Num6";
		case sf::Keyboard::Num7:
			return "Num7";
		case sf::Keyboard::Num8:
			return "Num8";
		case sf::Keyboard::Num9:
			return "Num9";
		case sf::Keyboard::Escape:
			return "Escape";
		case sf::Keyboard::LControl:
			return "LControl";
		case sf::Keyboard::LShift:
			return "LShift";
		case sf::Keyboard::LAlt:
			return "LAlt";
		case sf::Keyboard::LSystem:
			return "LSystem";
		case sf::Keyboard::RControl:
			return "RControl";
		case sf::Keyboard::RShift:
			return "RShift";
		case sf::Keyboard::RAlt:
			return "RAlt";
		case sf::Keyboard::RSystem:
			return "RSystem";
		case sf::Keyboard::Menu:
			return "Menu";
		case sf::Keyboard::LBracket:
			return "LBracket";
		case sf::Keyboard::RBracket:
			return "RBracket";
		case sf::Keyboard::SemiColon:
			return "SemiColon";
		case sf::Keyboard::Comma:
			return "Comma";
		case sf::Keyboard::Period:
			return "Period";
		case sf::Keyboard::Quote:
			return "Quote";
		case sf::Keyboard::Slash:
			return "Slash";
		case sf::Keyboard::BackSlash:
			return "BackSlash";
		case sf::Keyboard::Tilde:
			return "Tilde";
		case sf::Keyboard::Equal:
			return "Equal";
		case sf::Keyboard::Dash:
			return "Dash";
		case sf::Keyboard::Space:
			return "Space";
		case sf::Keyboard::Return:
			return "Return";
		case sf::Keyboard::BackSpace:
			return "BackSpace";
		case sf::Keyboard::Tab:
			return "Tab";
		case sf::Keyboard::PageUp:
			return "PageUp";
		case sf::Keyboard::PageDown:
			return "PageDown";
		case sf::Keyboard::End:
			return "End";
		case sf::Keyboard::Home:
			return "Home";
		case sf::Keyboard::Insert:
			return "Insert";
		case sf::Keyboard::Delete:
			return "Delete";
		case sf::Keyboard::Add:
			return "Add";
		case sf::Keyboard::Subtract:
			return "Subtract";
		case sf::Keyboard::Multiply:
			return "Multiply";
		case sf::Keyboard::Divide:
			return "Divide";
		case sf::Keyboard::Left:
			return "Left";
		case sf::Keyboard::Right:
			return "Right";
		case sf::Keyboard::Up:
			return "Up";
		case sf::Keyboard::Down:
			return "Down";
		case sf::Keyboard::Numpad0:
			return "Numpad0";
		case sf::Keyboard::Numpad1:
			return "Numpad1";
		case sf::Keyboard::Numpad2:
			return "Numpad2";
		case sf::Keyboard::Numpad3:
			return "Numpad3";
		case sf::Keyboard::Numpad4:
			return "Numpad4";
		case sf::Keyboard::Numpad5:
			return "Numpad5";
		case sf::Keyboard::Numpad6:
			return "Numpad6";
		case sf::Keyboard::Numpad7:
			return "Numpad7";
		case sf::Keyboard::Numpad8:
			return "Numpad8";
		case sf::Keyboard::Numpad9:
			return "Numpad9";
		case sf::Keyboard::F1:
			return "F1";
		case sf::Keyboard::F2:
			return "F2";
		case sf::Keyboard::F3:
			return "F3";
		case sf::Keyboard::F4:
			return "F4";
		case sf::Keyboard::F5:
			return "F5";
		case sf::Keyboard::F6:
			return "F6";
		case sf::Keyboard::F7:
			return "F7";
		case sf::Keyboard::F8:
			return "F8";
		case sf::Keyboard::F9:
			return "F9";
		case sf::Keyboard::F10:
			return "F10";
		case sf::Keyboard::F11:
			return "F11";
		case sf::Keyboard::F12:
			return "F12";
		case sf::Keyboard::F13:
			return "F13";
		case sf::Keyboard::F14:
			return "F14";
		case sf::Keyboard::F15:
			return "F15";
		case sf::Keyboard::Pause:
			return "Pause";
		}
	}
};



