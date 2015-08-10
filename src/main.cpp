#include <iostream>
#include <SFML/Graphics.hpp>

#include "Particles/ParticleSystem.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Particles");
	window.setVerticalSyncEnabled(true);

	sf::RenderTexture renderTexture;
	renderTexture.create(WIDTH, HEIGHT);

	sf::Texture circleTexture, blobTexture;
	if (!circleTexture.loadFromFile("res/circleTexture.png"))
		std::cout << "Invalid path to texture." << std::endl;
	if (!blobTexture.loadFromFile("res/blobTexture.png"))
		std::cout << "Invalid path to texture." << std::endl;

	circleTexture.setSmooth(true);
	blobTexture.setSmooth(true);

	// Pointer to emitter position
	sf::Vector2f *position;

	// System
	auto system = std::make_unique<particles::MetaballParticleSystem>(10000, &blobTexture, &renderTexture);
	system->emitRate = 10000.0f / 4.0f / 10.f;
	system->color = sf::Color(20, 50, 100, 128);

	// Generators
	auto posGen = system->addGenerator<particles::DiskPositionGenerator>();
	posGen->center = sf::Vector2f(0, 0);
	posGen->radius = 100.f;

	position = &posGen->center;

	auto sizeGen = system->addGenerator<particles::SizeGenerator>();
	sizeGen->minStartSize = 10.0f;
	sizeGen->maxStartSize = 30.0f;
	sizeGen->minEndSize = 5.f;
	sizeGen->maxEndSize = 15.f;

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

		position->x = pos.x; position->y = pos.y;

		sf::Time elapsed = clock.restart();
		system->update(elapsed);

		window.clear();
		system->render(window);
		window.display();
	}

	return 0;
}