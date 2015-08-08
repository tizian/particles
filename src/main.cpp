#include <iostream>
#include <SFML/Graphics.hpp>

#include "ParticleSystem.h"

int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Particles");
	window.setVerticalSyncEnabled(true);

	sf::Texture circleTexture, blobTexture;
	if (!circleTexture.loadFromFile("res/circleTexture.png"))
		std::cout << "No circle texture!" << std::endl;
	if (!blobTexture.loadFromFile("res/blobTexture.png"))
		std::cout << "No blob texture!" << std::endl;

	// Pointer to emitter position
	sf::Vector2f *position;

	// System
	auto system = std::make_unique<particles::ParticleSystem>(10000, &blobTexture);
	system->emitRate = 10000.0f / 4.0f / 10.f;
	system->additiveBlendMode = true;

	// Generators
	auto posGen = system->addGenerator<particles::BoxPositionGenerator>();
	posGen->center = sf::Vector2f(0, 0);
	posGen->size = sf::Vector2f(20.0f, 5.0f);

	position = &posGen->center;

	auto sizeGen = system->addGenerator<particles::SizeGenerator>();
	sizeGen->minStartSize = 2.0f;
	sizeGen->maxStartSize = 10.0f;
	sizeGen->minEndSize = 0.f;
	sizeGen->maxEndSize = 2.f;

	auto colGen = system->addGenerator<particles::ColorGenerator>();
	colGen->minStartCol = sf::Color(150, 0, 180, 255);
	colGen->maxStartCol = sf::Color(220, 255, 220, 255);
	colGen->minEndCol = sf::Color(128, 0, 150, 0);
	colGen->maxEndCol = sf::Color(180, 128, 220, 0);

	auto velGen = system->addGenerator<particles::AngledVelocityGenerator>();
	velGen->minAngle = -20.f;
	velGen->maxAngle = 20.f;
	velGen->minStartVel = 100.f;
	velGen->maxStartVel = 100.0f;

	auto timeGen = system->addGenerator<particles::TimeGenerator>();
	timeGen->minTime = 1.0f;
	timeGen->maxTime = 5.0f;
	
	// Updaters
	auto timeUpdater = system->addUpdater<particles::TimeUpdater>();

	auto colorUpdater = system->addUpdater<particles::ColorUpdater>();

	//auto attractorUpdater = system->addUpdater<particles::AttractorUpdater>();
	//attractorUpdater->add(sf::Vector3f(640, 360, 1000.0f));

	//sf::Vector3f &attractor = attractorUpdater->get(0);

	auto eulerUpdater = system->addUpdater<particles::EulerUpdater>();
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
			system->emit(100);
		}

		sf::Time elapsed = clock.restart();
		system->update(elapsed);

		window.clear();
		window.draw(*system.get());
		window.display();
	}

	return 0;
}