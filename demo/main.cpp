#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <imgui.h>
#include <imgui-sfml.h>
#include "imgui-custom.h"

#include <Particles/ParticleSystem.h>

#include <iostream>
 
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

const int MAX_PARTICLE_COUNT = 100000;

enum ParticleSystemMode {
	Points,
	Texture,
	Spritesheet,
	AnimatedSpritesheet,
	Metaball
};

enum SpawnerMode {
	Point,
	Box,
	Circle,
	Disk
};

enum VelocityGeneratorMode {
	Angled,
	Vector,
	Aimed
};

void initParticleSystem();
void setSpawnMode();
void setVelocityGeneratorMode();
void gui();

// Particle System
particles::ParticleSystem *particleSystem;

// Particle Spawner
particles::ParticleSpawner *spawner;

// Particle Generators
particles::ParticleGenerator *velocityGenerator;
particles::TimeGenerator *timeGenerator;
particles::RotationGenerator *rotationGenerator;
particles::ColorGenerator *colorGenerator;
particles::SizeGenerator *sizeGenerator;

// Particle Updaters
particles::TimeUpdater *timeUpdater;
particles::RotationUpdater *rotationUpdater;
particles::ColorUpdater *colorUpdater;
particles::SizeUpdater *sizeUpdater;
particles::EulerUpdater *eulerUpdater;

// Global State
ParticleSystemMode particleSystemMode = ParticleSystemMode::Texture;
SpawnerMode spawnerMode = SpawnerMode::Point;
VelocityGeneratorMode velocityGeneratorMode = VelocityGeneratorMode::Angled;

sf::Texture *circleTexture;
sf::Texture *blobTexture;
sf::Texture *starTexture;
sf::Texture *spritesheetTexture;

void initParticleSystem() {
	if (particleSystem) delete particleSystem;

	switch (particleSystemMode) {
	case Points:
		particleSystem = new particles::PointParticleSystem(MAX_PARTICLE_COUNT);
		break;
	case Spritesheet:
	case AnimatedSpritesheet:
		particleSystem = new particles::SpriteSheetParticleSystem(MAX_PARTICLE_COUNT, spritesheetTexture);
		break;
	case Metaball:
		particleSystem = new particles::MetaballParticleSystem(MAX_PARTICLE_COUNT, blobTexture, WINDOW_WIDTH, WINDOW_HEIGHT);
		break;
	case Texture:
	default:
		particleSystem = new particles::TextureParticleSystem(MAX_PARTICLE_COUNT, circleTexture);
		break;
	}

	particleSystem->emitRate = 160.f;

	setSpawnMode();
	setVelocityGeneratorMode();

	timeGenerator = particleSystem->addGenerator<particles::TimeGenerator>();
	timeGenerator->minTime = 1.f;
	timeGenerator->maxTime = 5.f;

	colorGenerator = particleSystem->addGenerator<particles::ColorGenerator>();
	colorGenerator->minStartCol = sf::Color(16, 124, 167, 255);
	colorGenerator->maxStartCol = sf::Color(30, 150, 255, 255);
	colorGenerator->minEndCol = sf::Color(57, 0, 150, 0);
	colorGenerator->maxEndCol = sf::Color(235, 128, 220, 0);

	sizeGenerator = particleSystem->addGenerator<particles::SizeGenerator>();
	sizeGenerator->minStartSize = 20.f;
	sizeGenerator->maxStartSize = 60.f;
	sizeGenerator->minEndSize = 10.f;
	sizeGenerator->maxEndSize = 30.f;

	rotationGenerator = particleSystem->addGenerator<particles::RotationGenerator>();
	rotationGenerator->minStartAngle = -20.f;
	rotationGenerator->maxStartAngle = -20.f;
	rotationGenerator->minEndAngle = 90.f;
	rotationGenerator->maxEndAngle = 90.f;

	timeUpdater = particleSystem->addUpdater<particles::TimeUpdater>();
	colorUpdater = particleSystem->addUpdater<particles::ColorUpdater>();
	sizeUpdater = particleSystem->addUpdater<particles::SizeUpdater>();
	rotationUpdater = particleSystem->addUpdater<particles::RotationUpdater>();
	eulerUpdater = particleSystem->addUpdater<particles::EulerUpdater>();

	if (particleSystemMode == ParticleSystemMode::Spritesheet) {
		auto texCoordGen = particleSystem->addGenerator<particles::TexCoordsRandomGenerator>();
		texCoordGen->texCoords.push_back(sf::IntRect(0, 0, 8, 8));
		texCoordGen->texCoords.push_back(sf::IntRect(8, 0, 8, 8));
		texCoordGen->texCoords.push_back(sf::IntRect(16, 0, 8, 8));
		texCoordGen->texCoords.push_back(sf::IntRect(24, 0, 8, 8));
	}
	else if (particleSystemMode == ParticleSystemMode::AnimatedSpritesheet) {
		auto texCoordGen = particleSystem->addGenerator<particles::TexCoordsGenerator>();
		texCoordGen->texCoords = sf::IntRect(0, 0, 8, 8);

		auto animationUpdater = particleSystem->addUpdater<particles::AnimationUpdater>();
		animationUpdater->frames.push_back(sf::IntRect(0, 0, 8, 8));
		animationUpdater->frames.push_back(sf::IntRect(8, 0, 8, 8));
		animationUpdater->frames.push_back(sf::IntRect(16, 0, 8, 8));
		animationUpdater->frames.push_back(sf::IntRect(24, 0, 8, 8));

		animationUpdater->frameTime = 0.8f;
		animationUpdater->looped = true;
	}
	else if (particleSystemMode == ParticleSystemMode::Metaball) {
		auto ps = dynamic_cast<particles::MetaballParticleSystem *>(particleSystem);
		ps->color = sf::Color(20, 50, 100, 255);
	}
}

void setSpawnMode() {
	if (spawner) particleSystem->removeSpawner(spawner);

	switch (spawnerMode) {
	case Box: {
		auto boxSpawner = particleSystem->addSpawner<particles::BoxSpawner>();
		boxSpawner->size = sf::Vector2f(160.f, 160.f);
		spawner = boxSpawner;
	}
	break;
	
	case Circle: {
		auto circleSpawner = particleSystem->addSpawner<particles::CircleSpawner>();
		circleSpawner->radius = sf::Vector2f(70.f, 40.f);
		spawner = circleSpawner;
	}
	break;

	case Disk: {
		auto diskSpawner = particleSystem->addSpawner<particles::DiskSpawner>();
		diskSpawner->radius = 100.f;
		spawner = diskSpawner;
	}
	break;
	
	case Point:
	default: {
		spawner = particleSystem->addSpawner<particles::PointSpawner>();
	}
	break;
	}
}

void setVelocityGeneratorMode() {
	if (velocityGenerator) particleSystem->removeGenerator(velocityGenerator);

	switch (velocityGeneratorMode) {
	case Vector: {
		auto vectorGenerator = particleSystem->addGenerator<particles::VectorVelocityGenerator>();
		vectorGenerator->minStartVel = sf::Vector2f(20.f, -20.f);
		vectorGenerator->maxStartVel = sf::Vector2f(40.f, -40.f);
		velocityGenerator = vectorGenerator;
	}
	break;
	
	case Aimed: {
		auto aimedGenerator = particleSystem->addGenerator<particles::AimedVelocityGenerator>();
		aimedGenerator->goal = sf::Vector2f(0.5f * WINDOW_WIDTH, 0.5f * WINDOW_HEIGHT);
		aimedGenerator->minStartSpeed = 100.f;
		aimedGenerator->maxStartSpeed = 150.f;
		velocityGenerator = aimedGenerator;
	}
	break;
				
	case Angled:
	default: {
		auto angledGenerator = particleSystem->addGenerator<particles::AngledVelocityGenerator>();
		angledGenerator->minAngle = -20.f;
		angledGenerator->maxAngle = 20.f;
		angledGenerator->minStartSpeed = 100.f;
		angledGenerator->maxStartSpeed = 150.f;
		velocityGenerator = angledGenerator;
	}
	break;
	}
}

void gui() {
	ImGui::SetNextWindowSize(ImVec2(380, 630), ImGuiSetCond_FirstUseEver);

	ImGui::Begin("Particles Demo");

	if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen)) {
		const char* particleSystemItems[] = { "Points", "Textured", "Spritesheet", "Animated Spritesheet", "Metaball" };
		static int particleSystemItem = 1;
		if (ImGui::Combo("Render Mode", &particleSystemItem, particleSystemItems, 5)) {
			particleSystemMode = static_cast<ParticleSystemMode>(particleSystemItem);
			initParticleSystem();
		}

		ImGui::SliderFloat("emit rate", &particleSystem->emitRate, 0.f, 1500.f);
		if (particleSystemMode == ParticleSystemMode::Texture || particleSystemMode == ParticleSystemMode::Spritesheet || particleSystemMode == ParticleSystemMode::AnimatedSpritesheet) {
			auto ps = dynamic_cast<particles::TextureParticleSystem *>(particleSystem);
			ImGui::Checkbox("Additive blending", &ps->additiveBlendMode);
		}

		if (particleSystemMode == ParticleSystemMode::Texture) {
			auto ps = dynamic_cast<particles::TextureParticleSystem *>(particleSystem);

			const char* textureItems[] = { "Circle", "Blob", "Star" };
			static int textureItem = 0;
			if (ImGui::Combo("Texture", &textureItem, textureItems, 3)) {
				if (textureItem == 0) {
					ps->setTexture(circleTexture);
				}
				else if (textureItem == 1) {
					ps->setTexture(blobTexture);
				}
				if (textureItem == 2) {
					ps->setTexture(starTexture);
				}
			}
		}

		if (particleSystemMode == ParticleSystemMode::Metaball) {
			auto ps = dynamic_cast<particles::MetaballParticleSystem *>(particleSystem);
			ImGui::ColorEdit("Color", &ps->color);
			ImGui::SliderFloat("Threshold", &ps->threshold, 0.f, 0.999f);
		}
	}

	if (ImGui::CollapsingHeader("Particle Spawner")) {
		const char* spawnItems[] = { "Point", "Box", "Circle", "Disk" };
		static int spawnItem = 0;
		if (ImGui::Combo("Spawner Mode", &spawnItem, spawnItems, 4)) {
			spawnerMode = static_cast<SpawnerMode>(spawnItem);
			setSpawnMode();
		}

		if (spawnerMode == SpawnerMode::Point) {
			// nop
		}
		else if (spawnerMode == SpawnerMode::Box) {
			auto sp = dynamic_cast<particles::BoxSpawner *>(spawner);
			ImGui::SliderFloat2("size", &sp->size, 0.f, 500.f);
		}
		else if (spawnerMode == SpawnerMode::Circle) {
			auto sp = dynamic_cast<particles::CircleSpawner *>(spawner);
			ImGui::SliderFloat2("size", &sp->radius, 0.f, 500.f);
		}
		else if (spawnerMode == SpawnerMode::Disk) {
			auto sp = dynamic_cast<particles::DiskSpawner *>(spawner);
			ImGui::SliderFloat("size", &sp->radius, 0.f, 500.f);
		}
	}

	if (ImGui::CollapsingHeader("Velocity Generator")) {
		const char* velItems[] = { "Angled", "Vector", "Aimed" };
		static int velItem = 0;
		if (ImGui::Combo("Velocity Mode", &velItem, velItems, 3)) {
			velocityGeneratorMode = static_cast<VelocityGeneratorMode>(velItem);
			setVelocityGeneratorMode();
		}

		if (velocityGeneratorMode == VelocityGeneratorMode::Angled) {
			auto gen = dynamic_cast<particles::AngledVelocityGenerator *>(velocityGenerator);
			ImGui::SliderFloat("min angle", &gen->minAngle, -180.f, 180.f);
			ImGui::SliderFloat("max angle", &gen->maxAngle, -180.f, 180.f);
			ImGui::SliderFloat("min speed", &gen->minStartSpeed, 0.f, 500.f);
			ImGui::SliderFloat("max speed", &gen->maxStartSpeed, 0.f, 500.f);
		}
		else if (velocityGeneratorMode == VelocityGeneratorMode::Vector) {
			auto gen = dynamic_cast<particles::VectorVelocityGenerator *>(velocityGenerator);
			ImGui::SliderFloat2("min", &gen->minStartVel, 0.f, 500.f);
			ImGui::SliderFloat2("max", &gen->maxStartVel, 0.f, 500.f);
		}
		else if (velocityGeneratorMode == VelocityGeneratorMode::Aimed) {
			auto gen = dynamic_cast<particles::AimedVelocityGenerator *>(velocityGenerator);
			ImGui::SliderFloat2("min", &gen->goal, 0.f, 500.f);
			ImGui::SliderFloat("min speed", &gen->minStartSpeed, 0.f, 500.f);
			ImGui::SliderFloat("max speed", &gen->maxStartSpeed, 0.f, 500.f);
		}
	}

	if (ImGui::CollapsingHeader("Time Generator")) {
		ImGui::SliderFloat("min", &timeGenerator->minTime, 0.f, 60.f);
		ImGui::SliderFloat("max", &timeGenerator->maxTime, 0.f, 60.f);
	}

	if (particleSystemMode != ParticleSystemMode::Points && ImGui::CollapsingHeader("Size Generator")) {
		ImGui::SliderFloat("min start size", &sizeGenerator->minStartSize, 0.f, 100.f);
		ImGui::SliderFloat("max start size", &sizeGenerator->maxStartSize, 0.f, 100.f);
		ImGui::SliderFloat("min end size", &sizeGenerator->minEndSize, 0.f, 100.f);
		ImGui::SliderFloat("max end size", &sizeGenerator->maxEndSize, 0.f, 100.f);
	}

	if (particleSystemMode != ParticleSystemMode::Points && ImGui::CollapsingHeader("Rotation Generator")) {
		ImGui::SliderFloat("min start angle", &rotationGenerator->minStartAngle, -180.f, 180.f);
		ImGui::SliderFloat("max start angle", &rotationGenerator->maxStartAngle, -180.f, 180.f);
		ImGui::SliderFloat("min end angle", &rotationGenerator->minEndAngle, -180.f, 180.f);
		ImGui::SliderFloat("max end angle", &rotationGenerator->maxEndAngle, -180.f, 180.f);
	}

	if (particleSystemMode != ParticleSystemMode::Metaball && ImGui::CollapsingHeader("Color Generator")) {
		ImGui::ColorEdit("min start", &colorGenerator->minStartCol);
		ImGui::ColorEdit("max start", &colorGenerator->maxStartCol);
		ImGui::ColorEdit("min end", &colorGenerator->minEndCol);
		ImGui::ColorEdit("max end", &colorGenerator->maxEndCol);
	}

	if (ImGui::CollapsingHeader("Euler Updater")) {
		ImGui::SliderFloat2("gravity", &eulerUpdater->globalAcceleration, 0.f, 200.f);
	}

	ImGui::End();
}

int main() {
	circleTexture = new sf::Texture();
	blobTexture = new sf::Texture();
	starTexture = new sf::Texture();
	spritesheetTexture = new sf::Texture();
	circleTexture->loadFromFile("res/circleTexture.png");
	blobTexture->loadFromFile("res/blobTexture.png");
	starTexture->loadFromFile("res/starTexture.png");
	spritesheetTexture->loadFromFile("res/spritesheetTexture.png");
	circleTexture->setSmooth(true);
	blobTexture->setSmooth(true);
	starTexture->setSmooth(true);
	spritesheetTexture->setSmooth(false);

	initParticleSystem();

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Particles Demo");
	window.setVerticalSyncEnabled(true);

	ImGui::SFML::Init(window);
 
	sf::Clock clock;
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
 
			if (event.type == sf::Event::Closed ||
				(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
				window.close();
			}
		}

		sf::Vector2i mouse = sf::Mouse::getPosition(window);
		sf::Vector2f pos = window.mapPixelToCoords(mouse);

		spawner->center = pos;

		sf::Time dt = clock.restart();
		ImGui::SFML::Update(dt);
		particleSystem->update(dt);

		gui();
 
		window.clear();

		particleSystem->render(window);
		ImGui::Render();

		window.display();
	}
 
	ImGui::SFML::Shutdown();
	delete particleSystem;
}