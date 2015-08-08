#include <iostream>
#include <SFML/Graphics.hpp>

#include "ParticleSystem.h"

int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(512, 256), "Particles");
	window.setVerticalSyncEnabled(true);

	ParticleSystem system(10000);

	auto particleEmitter = std::make_shared<ParticleEmitter>();
	{
		particleEmitter->emitRate = 10000.0f / 4.0f;

		auto posGen = std::make_shared<CirclePositionGenerator>();
		posGen->center = sf::Vector2f(256, 128);
		posGen->radius = sf::Vector2f(50, 25);
		particleEmitter->addGenerator(posGen);

		auto colGen = std::make_shared<BasicColorGenerator>();
		colGen->minStartCol = sf::Color(200, 0, 200, 255);
		colGen->maxStartCol = sf::Color(255, 255, 255, 255);
		colGen->minEndCol = sf::Color(128, 0, 150, 0);
		colGen->maxEndCol = sf::Color(200, 128, 255, 0);
		particleEmitter->addGenerator(colGen);

		auto velGen = std::make_shared<BasicVelocityGenerator>();
		//velGen->minStartVel = sf::Vector2f(-3, -5.0f);
		//velGen->maxStartVel = sf::Vector2f(3, -5.0f);
		particleEmitter->addGenerator(velGen);

		auto timeGen = std::make_shared<BasicTimeGenerator>();
		timeGen->minTime = 3.0f;
		timeGen->maxTime = 5.0f;
		particleEmitter->addGenerator(timeGen);
	}
	system.addEmitter(particleEmitter);

	auto timeUpdater = std::make_shared<BasicTimeUpdater>();
	system.addUpdater(timeUpdater);

	auto colorUpdater = std::make_shared<BasicColorUpdater>();
	system.addUpdater(colorUpdater);

	auto attractorUpdater = std::make_shared<AttractorUpdater>();
	attractorUpdater->add(sf::Vector3f(256, 128, 1000.0f));
	system.addUpdater(attractorUpdater);

	auto eulerUpdater = std::make_shared<EulerUpdater>();
	eulerUpdater->globalAcceleration = sf::Vector2f(0.0f, 1000.0f);
	system.addUpdater(eulerUpdater);

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
		//particles.setEmitter(window.mapPixelToCoords(mouse));

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