#pragma once
#include "headers.h"
#include "stateengine.h"

class State {
public:

	virtual void handle_events(StateEngine* game) = 0;
	virtual void update(StateEngine* game) = 0;
	virtual void draw(StateEngine* game) = 0;

	void ChangeState(StateEngine* game,
		State* state) {
		game->change_state(state);
	};
}; 