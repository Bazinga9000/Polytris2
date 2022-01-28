#include "headers.h"
#include "stateengine.h"
#include "basicmode.h"
#include "menustate.h"
int main()
{

	//credits

	std::cout << "-----===POLYTRIS===-----" << std::endl;
	std::cout << "Programmed by Bazinga_9000" << std::endl << std::endl;
	std::cout << "Polyomino sprites made by superjustinbros" << std::endl << "on Spriter's Resource" << std::endl << std::endl;
	std::cout << "-----==============-----" << std::endl;
	

	//create window
	sf::RenderWindow window(sf::VideoMode(720, 720), "Polytris");
	//window.setVisible(true);

	sf::Image icon;

	if (!icon.loadFromFile("icon.png"))
	{
		std::cout << "Error Loading Icon";
		// error...
	}

	window.setIcon(90, 90, icon.getPixelsPtr());
	window.setKeyRepeatEnabled(false);


	StateEngine game(window);
	Menu m = Menu(&game);
	game.push_state(&m);
	while (game.running()) {
		game.events();
		game.update();
		game.draw();
	
	}

}
