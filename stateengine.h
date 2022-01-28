#pragma once
#include "headers.h"

class State;

class StateEngine {
public:
	StateEngine(sf::RenderWindow &window);


	std::vector<State*> states = {};

	void change_state(State *state);
	void push_state(State *state);
	void pop_state();

	sf::RenderWindow* m_window;
	bool m_running = true;

	bool running() { return m_running; }
	void quit() { m_running = false; }

	void draw();
	void events();
	void update();


	//game stuff
	
	void writeconfig();
	
	int movel;
	int mover;
	int rotl;
	int rotr;
	int softdrop;
	int harddrop;
	int hold;
	int das;
	int arr;
	int polyskin;
	int volume;
	int pause;

};