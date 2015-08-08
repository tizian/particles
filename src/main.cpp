#include <iostream>
#include <SFML/Graphics.hpp>

#include "ParticleSystem.h"

/*
TODO
----
* Textures -> Use quads when textures are used
* Scale parameter, adjustable with updaters
* Metablob rendering with Additive BlendMode
* Nicer way to generate Emitters? (Maybe just create some presets)
* Template functions to add Emitters?

*/

int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(512, 256), "Particles");
	window.setVerticalSyncEnabled(true);
	
	particles::ParticleSystem system(10000, false);

	sf::Vector2f *position;

	auto particleEmitter = std::make_shared<particles::ParticleEmitter>();
	system.addEmitter(particleEmitter);
	{
		particleEmitter->emitRate = 10000.0f / 4.0f;

		auto posGen = std::make_shared<particles::DiskPositionGenerator>();
		particleEmitter->addGenerator(posGen);
		posGen->center = sf::Vector2f(256, 128);
		posGen->radius = 20.f;

		position = &posGen->center;

		auto colGen = std::make_shared<particles::ColorGenerator>();
		particleEmitter->addGenerator(colGen);
		colGen->minStartCol = sf::Color(200, 0, 200, 255);
		colGen->maxStartCol = sf::Color(255, 255, 255, 255);
		colGen->minEndCol = sf::Color(128, 0, 150, 0);
		colGen->maxEndCol = sf::Color(200, 128, 255, 0);
		
		auto velGen = std::make_shared<particles::AngledVelocityGenerator>();
		particleEmitter->addGenerator(velGen);
		velGen->minAngle = -15.f;
		velGen->maxAngle = 15.f;
		velGen->minStartVel = 50.f;
		velGen->maxStartVel = 50.0f;

		auto timeGen = std::make_shared<particles::TimeGenerator>();
		particleEmitter->addGenerator(timeGen);
		timeGen->minTime = 3.0f;
		timeGen->maxTime = 5.0f;
	}
	

	auto timeUpdater = std::make_shared<particles::TimeUpdater>();
	system.addUpdater(timeUpdater);

	auto colorUpdater = std::make_shared<particles::ColorUpdater>();
	system.addUpdater(colorUpdater);

	//auto attractorUpdater = std::make_shared<particles::AttractorUpdater>();
	//system.addUpdater(attractorUpdater);
	//attractorUpdater->add(sf::Vector3f(256, 128, 1000.0f));

	//sf::Vector3f &attractor = attractorUpdater->get(0);

	auto eulerUpdater = std::make_shared<particles::EulerUpdater>();
	system.addUpdater(eulerUpdater);
	//eulerUpdater->globalAcceleration = sf::Vector2f(0.0f, 1000.0f);
	

	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		sf::Vector2i mouse = sf::Mouse::getPosition(window);
		sf::Vector2f pos = window.mapPixelToCoords(mouse);
		//attractor.x = pos.x; attractor.y = pos.y;
		position->x = pos.x; position->y = pos.y;

		bool pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
		if (pressed)
		{
			system.emit(100);
		}

		sf::Time elapsed = clock.restart();
		system.update(elapsed.asSeconds());

		window.clear();
		window.draw(system);
		window.display();
	}

	return 0;
}