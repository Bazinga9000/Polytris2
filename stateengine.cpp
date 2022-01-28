#pragma once
#include "headers.h"
#include "state.h"


StateEngine::StateEngine(sf::RenderWindow &window) {
	m_window = &window;

	m_running = true;


	movel = 71;
	mover = 72;
	rotl = 23;
	rotr = 25;
	softdrop = 74;
	harddrop = 73;
	hold = 2;
	das = 7;
	arr = 2;
	polyskin = 0;
	volume = 100;
	pause = static_cast<int>(sf::Keyboard::Escape);

	std::ifstream controlfile("settings.cfg");

	if (controlfile.is_open()) {
		controlfile >> movel >> mover >> rotl >> rotr >> softdrop >> harddrop >> hold >> das >> arr >> polyskin >> volume >> pause;
	}

	writeconfig();
	

}

void StateEngine::writeconfig() {
	std::ofstream ofile;
	ofile.open("settings.cfg");

	for (auto i : { movel, mover, rotl, rotr, softdrop, harddrop, hold, das, arr, polyskin, volume, pause }) {
		ofile << i << "\n";
	}
}

void StateEngine::change_state(State* state) {
	states.pop_back();
	states.push_back(state);
}

void StateEngine::push_state(State* state) {
	states.push_back(state);
}

void StateEngine::pop_state() {
	states.pop_back();
}

void StateEngine::draw() {
	states.back()->draw(this);
}

void StateEngine::events() {
	states.back()->handle_events(this);
}

void StateEngine::update() {
	states.back()->update(this);
}


/*
class StateEngine {
public:
	StateEngine(sf::RenderWindow &window);


	std::vector<State*> states = {};

	void change_state(State *state);
	void push_state(State *state);
	void pop_state();

	sf::RenderWindow *m_window;
	bool m_running;

	bool running() { return m_running; }
	void quit() { m_running = false; }

};
*/