#include <iostream>
#include <SFML/Graphics.hpp>

#include "ParticleSystem.h"

int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(512, 256), "Particles");
	window.setVerticalSyncEnabled(true);
	
	particles::ParticleSystem system(10000);
	system.position = sf::Vector2f(256, 128);

	auto particleEmitter = std::make_shared<particles::ParticleEmitter>();
	system.addEmitter(particleEmitter);
	{
		particleEmitter->emitRate = 10000.0f / 4.0f;

		auto posGen = std::make_shared<particles::CirclePositionGenerator>();
		particleEmitter->addGenerator(posGen);
		posGen->radius = sf::Vector2f(50, 25);

		auto colGen = std::make_shared<particles::ColorGenerator>();
		particleEmitter->addGenerator(colGen);
		colGen->minStartCol = sf::Color(200, 0, 200, 255);
		colGen->maxStartCol = sf::Color(255, 255, 255, 255);
		colGen->minEndCol = sf::Color(128, 0, 150, 0);
		colGen->maxEndCol = sf::Color(200, 128, 255, 0);
		
		auto velGen = std::make_shared<particles::VelocityGenerator>();
		particleEmitter->addGenerator(velGen);
		//velGen->minStartVel = sf::Vector2f(-3, -5.0f);
		//velGen->maxStartVel = sf::Vector2f(3, -5.0f);

		auto timeGen = std::make_shared<particles::TimeGenerator>();
		particleEmitter->addGenerator(timeGen);
		timeGen->minTime = 3.0f;
		timeGen->maxTime = 5.0f;
	}
	

	auto timeUpdater = std::make_shared<particles::TimeUpdater>();
	system.addUpdater(timeUpdater);

	auto colorUpdater = std::make_shared<particles::ColorUpdater>();
	system.addUpdater(colorUpdater);

	auto attractorUpdater = std::make_shared<particles::AttractorUpdater>();
	system.addUpdater(attractorUpdater);
	attractorUpdater->add(sf::Vector3f(256, 128, 1000.0f));

	sf::Vector3f &attractor = attractorUpdater->get(0);

	auto eulerUpdater = std::make_shared<particles::EulerUpdater>();
	system.addUpdater(eulerUpdater);
	eulerUpdater->globalAcceleration = sf::Vector2f(0.0f, 1000.0f);
	
	// create a clock to track the elapsed time
	sf::Clock clock;

	// run the main loop
	while (window.isOpen())
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// make the particle system emitter follow the mouse
		sf::Vector2i mouse = sf::Mouse::getPosition(window);
		sf::Vector2f pos = window.mapPixelToCoords(mouse);
		//attractor.x = pos.x; attractor.y = pos.y;
		system.position = pos;
		//particles.setEmitter();

		// update it
		sf::Time elapsed = clock.restart();
		system.update(elapsed.asSeconds());

		// draw it
		window.clear();
		window.draw(system);
		window.display();
	}

	return 0;
}