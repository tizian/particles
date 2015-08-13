#include <iostream>

#include <SFML/Graphics.hpp>
#include <AntTweakBar.h>

#include "AntTweakBarInput.h"

#include "Particles/ParticleSystem.h"

using namespace std;

// Window size
const int WIDTH = 1280;
const int HEIGHT = 720;

bool firstStart = true;

// AntTweakBar
TwBar *bar = nullptr;

// Enums for application and ATB
enum ERenderMode { PointRendering, TextureRendering, MetaballRendering };
ERenderMode renderMode = ERenderMode::PointRendering;
ERenderMode lastRenderMode = ERenderMode::TextureRendering;

TwEnumVal renderModeEV[] = { { ERenderMode::PointRendering, "Point" }, { ERenderMode::TextureRendering, "Texture" }, { ERenderMode::MetaballRendering, "Metaballs" } };
TwType renderModeType;

enum EPosGenMode { Point, Box, Circle, Disk, EPosGenModeCount };
EPosGenMode posGenMode = EPosGenMode::Point;
EPosGenMode lastPosGenMode = EPosGenMode::EPosGenModeCount;

TwEnumVal posGenModeEV[] = { { EPosGenMode::Point, "Point" }, { EPosGenMode::Box, "Box" }, { EPosGenMode::Circle, "Circle" }, { EPosGenMode::Disk, "Disk" } };
TwType posGenModeType;

enum EVelGenMode { Vector, Angle, EVelGenModeCount };
EVelGenMode velGenMode = EVelGenMode::Angle;
EVelGenMode lastVelGenMode = EVelGenMode::EVelGenModeCount;

TwEnumVal velGenModeEV[] = { { EVelGenMode::Vector, "Vector" }, { EVelGenMode::Angle, "Angle" } };
TwType velGenModeType;

enum ESelectedTexture { Round, Blob, NO_TEX };
ESelectedTexture selectedTex = ESelectedTexture::Round;
ESelectedTexture lastSelectedTex = ESelectedTexture::Round;

TwEnumVal texEV[] = { { ESelectedTexture::Round, "Circle" }, { ESelectedTexture::Blob, "Blob" } };
TwType texType;

// Textures
std::unique_ptr<sf::Texture> circleTexture;
std::unique_ptr<sf::Texture> blobTexture;

// Render Texture used for metaball rendering
std::unique_ptr<sf::RenderTexture> renderTexture;

// Particle System
std::unique_ptr<particles::ParticleSystem> particleSystem;

// Emitter Position of Particle System
sf::Vector2f *position = nullptr;

std::shared_ptr<particles::ParticleGenerator> positionGenerator;
std::shared_ptr<particles::ParticleGenerator> velocityGenerator;

void configurePS(ERenderMode mode) {
	posGenModeType = TwDefineEnum("PosGenModeType", posGenModeEV, 4);
	TwAddVarRW(bar, "PosGenMode", posGenModeType, &posGenMode, " label='Generator Type' group='Position' ");

	auto posGen = particleSystem->addGenerator<particles::PointPositionGenerator>();
	positionGenerator = posGen;
	position = &posGen->center;

	velGenModeType = TwDefineEnum("VelGenModeType", velGenModeEV, 2);
	TwAddVarRW(bar, "VelGenMode", velGenModeType, &velGenMode, " label='Generator Type' group='Velocity' ");

	auto velGen = particleSystem->addGenerator<particles::AngledVelocityGenerator>();
	velocityGenerator = velGen;

	velGen->minAngle = -20.f;
	velGen->maxAngle = 20.f;
	velGen->minStartVel = 100.f;
	velGen->maxStartVel = 100.f;

	if (!firstStart) {
		TwAddVarRW(bar, "velAngleMinAngle", TW_TYPE_FLOAT, &velGen->minAngle, " min=-360 max=360 group='Velocity' label='min. angle [deg]' ");
		TwAddVarRW(bar, "velAngleMaxAngle", TW_TYPE_FLOAT, &velGen->maxAngle, " min=-360 max=360 group='Velocity' label='max. angle [deg]' ");
		TwAddVarRW(bar, "velAngleMinVel", TW_TYPE_FLOAT, &velGen->minStartVel, " group='Velocity' label='min. Start Velocity' ");
		TwAddVarRW(bar, "velAngleMaxVel", TW_TYPE_FLOAT, &velGen->maxStartVel, " group='Velocity' label='max. Start Velocity' ");
	}
	firstStart = false;
	
	if (mode != ERenderMode::PointRendering) {
		auto sizeGen = particleSystem->addGenerator<particles::SizeGenerator>();
		sizeGen->minStartSize = 10.0f;
		sizeGen->maxStartSize = 30.0f;
		sizeGen->minEndSize = 5.f;
		sizeGen->maxEndSize = 15.f;

		TwAddVarRW(bar, "sizeGenMinStart", TW_TYPE_FLOAT, &sizeGen->minStartSize, " min=0 max=100 group='Size' label='min. Start Size' ");
		TwAddVarRW(bar, "sizeGenMaxStart", TW_TYPE_FLOAT, &sizeGen->maxStartSize, " min=0 max=100 group='Size' label='max. Start Size' ");
		TwAddVarRW(bar, "sizeGenMinEnd", TW_TYPE_FLOAT, &sizeGen->minEndSize, " min=0 max=100 group='Size' label='min. End Size' ");
		TwAddVarRW(bar, "sizeGenMaxEnd", TW_TYPE_FLOAT, &sizeGen->maxEndSize, " min=0 max=100 group='Size' label='max. End Size' ");
	}
	

	auto timeGen = particleSystem->addGenerator<particles::TimeGenerator>();
	timeGen->minTime = 1.0f;
	timeGen->maxTime = 5.0f;

	TwAddVarRW(bar, "timeGenMin", TW_TYPE_FLOAT, &timeGen->minTime, " min=0 max=360 group='Time' label='min. Lifetime [s]' ");
	TwAddVarRW(bar, "timeGenMax", TW_TYPE_FLOAT, &timeGen->maxTime, " min=0 max=360 group='Time' label='max. Lifetime [s]' ");

	if (mode != ERenderMode::MetaballRendering) {
		auto colGen = particleSystem->addGenerator<particles::ColorGenerator>();
		colGen->minStartCol = sf::Color(150, 0, 180, 255);
		colGen->maxStartCol = sf::Color(220, 255, 220, 255);
		colGen->minEndCol = sf::Color(128, 0, 150, 0);
		colGen->maxEndCol = sf::Color(180, 128, 220, 0);

		TwAddVarRW(bar, "colorGenMinStart", TW_TYPE_COLOR32, &colGen->minStartCol, "group='Color' label='min. Start Color' ");
		TwAddVarRW(bar, "colorGenMaxStart", TW_TYPE_COLOR32, &colGen->maxStartCol, "group='Color' label='max. Start Color' ");
		TwAddVarRW(bar, "colorGenMinEnd", TW_TYPE_COLOR32, &colGen->minEndCol, "group='Color' label='min. End Color' ");
		TwAddVarRW(bar, "colorGenMaxEnd", TW_TYPE_COLOR32, &colGen->maxEndCol, "group='Color' label='max. End Color' ");
	}

	// Updaters
	auto timeUpdater = particleSystem->addUpdater<particles::TimeUpdater>();
	auto colorUpdater = particleSystem->addUpdater<particles::ColorUpdater>();
	auto sizeUpdater = particleSystem->addUpdater<particles::SizeUpdater>();
	auto eulerUpdater = particleSystem->addUpdater<particles::EulerUpdater>();

	TwAddVarRW(bar, "eulerGlobalAccX", TW_TYPE_FLOAT, &eulerUpdater->globalAcceleration.x, "group='Physics' label='Global Acceleration (x)' ");
	TwAddVarRW(bar, "eulerGlobalAccY", TW_TYPE_FLOAT, &eulerUpdater->globalAcceleration.y, "group='Physics' label='Global Acceleration (y)' ");
}

void configurePosGen(EPosGenMode mode) {
	switch(mode) {
		case EPosGenMode::Point: {
			auto posGen = particleSystem->addGenerator<particles::PointPositionGenerator>();
			positionGenerator = posGen;
			position = &posGen->center;
		}
		break;
		case EPosGenMode::Box: {
			auto posGen = particleSystem->addGenerator<particles::BoxPositionGenerator>();
			positionGenerator = posGen;
			position = &posGen->center;

			posGen->size = { 80.f, 30.f };

			TwAddVarRW(bar, "posBoxSizeX", TW_TYPE_FLOAT, &posGen->size.x, "group='Position' label='size (x)' ");
			TwAddVarRW(bar, "posBoxSizeY", TW_TYPE_FLOAT, &posGen->size.y, "group='Position' label='size (y)' ");
		}
		break;
		case EPosGenMode::Circle: {
			auto posGen = particleSystem->addGenerator<particles::CirclePositionGenerator>();
			positionGenerator = posGen;
			position = &posGen->center;

			posGen->radius = { 70.f, 40.f };

			TwAddVarRW(bar, "posCircleRadiusX", TW_TYPE_FLOAT, &posGen->radius.x, "group='Position' label='radius (x)' ");
			TwAddVarRW(bar, "posCircleRadiusY", TW_TYPE_FLOAT, &posGen->radius.y, "group='Position' label='radius (y)' ");

		}
		break;
		case EPosGenMode::Disk: {
			auto posGen = particleSystem->addGenerator<particles::DiskPositionGenerator>();
			positionGenerator = posGen;
			position = &posGen->center;

			posGen->radius = 150.f;

			TwAddVarRW(bar, "posDiskRadius", TW_TYPE_FLOAT, &posGen->radius, "group='Position' label='radius' ");

		}
		break;
		default:
		break;
	}
}

void configureVelGen(EVelGenMode mode) {
	switch (mode) {
		case EVelGenMode::Vector: {
			auto velGen = particleSystem->addGenerator<particles::VelocityGenerator>();
			velocityGenerator = velGen;

			velGen->minStartVel = { 20.f, -20.f };
			velGen->maxStartVel = { 40.f, -40.f };

			TwAddVarRW(bar, "velVectorMinStartX", TW_TYPE_FLOAT, &velGen->minStartVel.x, " group='Velocity' label='min. Start Velocity (x)' ");
			TwAddVarRW(bar, "velVectorMinStartY", TW_TYPE_FLOAT, &velGen->minStartVel.y, " group='Velocity' label='min. Start Velocity (y)' ");
			TwAddVarRW(bar, "velVectorMaxStartX", TW_TYPE_FLOAT, &velGen->maxStartVel.x, " group='Velocity' label='max. Start Velocity (x)' ");
			TwAddVarRW(bar, "velVectorMaxStartY", TW_TYPE_FLOAT, &velGen->maxStartVel.y, " group='Velocity' label='max. Start Velocity (y)' ");

		}
		break;
		case EVelGenMode::Angle: {
			auto velGen = particleSystem->addGenerator<particles::AngledVelocityGenerator>();
			velocityGenerator = velGen;

			velGen->minAngle = -20.f;
			velGen->maxAngle = 20.f;
			velGen->minStartVel = 100.f;
			velGen->maxStartVel = 100.0f;

			TwAddVarRW(bar, "velAngleMinAngle", TW_TYPE_FLOAT, &velGen->minAngle, " min=-360 max=360 group='Velocity' label='min. angle [deg]' ");
			TwAddVarRW(bar, "velAngleMaxAngle", TW_TYPE_FLOAT, &velGen->maxAngle, " min=-360 max=360 group='Velocity' label='max. angle [deg]' ");
			TwAddVarRW(bar, "velAngleMinVel", TW_TYPE_FLOAT, &velGen->minStartVel, " group='Velocity' label='min. Start Velocity' ");
			TwAddVarRW(bar, "velAngleMaxVel", TW_TYPE_FLOAT, &velGen->maxStartVel, " group='Velocity' label='max. Start Velocity' ");

		}
		break;
		default:
		break;
	}
}

void updatePosGen() {
	if (posGenMode != lastPosGenMode) {
		if (lastPosGenMode == EPosGenMode::Box) {
			TwRemoveVar(bar, "posBoxSizeX");
			TwRemoveVar(bar, "posBoxSizeY");
		}
		else if (lastPosGenMode == EPosGenMode::Circle) {
			TwRemoveVar(bar, "posCircleRadiusX");
			TwRemoveVar(bar, "posCircleRadiusY");
		}
		else if (lastPosGenMode == EPosGenMode::Disk) {
			TwRemoveVar(bar, "posDiskRadius");
		}

		lastPosGenMode = posGenMode;

		configurePosGen(posGenMode);
	}
}

void updateVelGen() {
	if (velGenMode != lastVelGenMode) {
		if (lastVelGenMode == EVelGenMode::Vector) {
			TwRemoveVar(bar, "velVectorMinStartX");
			TwRemoveVar(bar, "velVectorMinStartY");
			TwRemoveVar(bar, "velVectorMaxStartX");
			TwRemoveVar(bar, "velVectorMaxStartY");
		}
		else if (lastVelGenMode == EVelGenMode::Angle) {
			TwRemoveVar(bar, "velAngleMinAngle");
			TwRemoveVar(bar, "velAngleMaxAngle");
			TwRemoveVar(bar, "velAngleMinVel");
			TwRemoveVar(bar, "velAngleMaxVel");
		}

		lastVelGenMode = velGenMode;

		configureVelGen(velGenMode);
	}
}

void updateTex() {
	if (selectedTex != lastSelectedTex) {
		lastSelectedTex = selectedTex;

		if (selectedTex == ESelectedTexture::Round) {
			((particles::TextureParticleSystem *)particleSystem.get())->setTexture(circleTexture.get());
		}
		else if (selectedTex == ESelectedTexture::Blob) {
			((particles::TextureParticleSystem *)particleSystem.get())->setTexture(blobTexture.get());
		}
	}
}

void updateRenderMode() {
	if (renderMode != lastRenderMode) {
		lastRenderMode = renderMode;

		TwRemoveAllVars(bar);

		renderModeType = TwDefineEnum("RenderModeType", renderModeEV, 3);
		TwAddVarRW(bar, "RenderMode", renderModeType, &renderMode, " group='General' label='Render Mode' ");

		if (renderMode == ERenderMode::PointRendering) {
			particleSystem.reset(new particles::PointParticleSystem(10000));
		}
		else if (renderMode == ERenderMode::TextureRendering) {
			particleSystem.reset(new particles::TextureParticleSystem(10000, circleTexture.get()));
			((particles::TextureParticleSystem *)particleSystem.get())->additiveBlendMode = false;

			TwAddVarRW(bar, "BlendMode", TW_TYPE_BOOL8, &((particles::TextureParticleSystem *)particleSystem.get())->additiveBlendMode, " group='Texture' label='Additive Blend Mode' ");

			texType = TwDefineEnum("TexType", texEV, 2);
			TwAddVarRW(bar, "SelectedTex", texType, &selectedTex, " group='Texture' label='Used Texture' ");
		}
		else if (renderMode == ERenderMode::MetaballRendering) {
			particleSystem.reset(new particles::MetaballParticleSystem(10000, blobTexture.get(), renderTexture.get()));
			((particles::MetaballParticleSystem *)particleSystem.get())->color = sf::Color(20, 50, 100, 255);

			TwAddVarRW(bar, "Threshold", TW_TYPE_FLOAT, &((particles::MetaballParticleSystem *)particleSystem.get())->threshold, " min=0.1 max=0.9 step=0.1 group='Metaball' ");
			TwAddVarRW(bar, "Color", TW_TYPE_COLOR32, &((particles::MetaballParticleSystem *)particleSystem.get())->color, " group='Metaball' ");
		}

		particleSystem->emitRate = 10000.0f / 4.0f / 10.f;
		TwAddVarRW(bar, "EmitRate", TW_TYPE_FLOAT, &particleSystem->emitRate, " group='General' label='Particle Emit Rate' ");

		configurePS(renderMode);
	}
}

int main() {
	bool wrongDir = false;

	// create the window
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Particles");
	window.setVerticalSyncEnabled(true);

	renderTexture.reset(new sf::RenderTexture());
	renderTexture->create(WIDTH, HEIGHT);

	TwInit(TW_OPENGL, NULL);
	TwWindowSize(WIDTH, HEIGHT);

	bar = TwNewBar("Particles");

	TwDefine(" GLOBAL help='2D Particle Test Application' ");
	TwDefine(" Particles size='300 500' ");

	circleTexture.reset(new sf::Texture());
	blobTexture.reset(new sf::Texture());
	if (!circleTexture->loadFromFile("res/circleTexture.png")) {
		std::cout << "Invalid path to texture." << std::endl;
		wrongDir = true;
	}
	if (!blobTexture->loadFromFile("res/blobTexture.png")) {
		std::cout << "Invalid path to texture." << std::endl;
		wrongDir = true;
	}

	if (wrongDir) {
		std::cerr << "Please use the 'particles' root directory as working directory." << std::endl;
		return 0;
	}

	circleTexture->setSmooth(true);
	blobTexture->setSmooth(true);
	
	sf::Clock clock;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			int handled = TwEventSFML_Custom(&event);
			if (!handled) {
				if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
					window.close();
				}
			}	
		}

		sf::Vector2i mouse = sf::Mouse::getPosition(window);
		sf::Vector2f pos = window.mapPixelToCoords(mouse);

		if (position) {
			position->x = pos.x; position->y = pos.y;
		}

		updateRenderMode();
		updatePosGen();
		updateVelGen();
		updateTex();

		sf::Time elapsed = clock.restart();
		particleSystem->update(elapsed);

		window.clear();
		particleSystem->render(window);
		TwDraw();
		window.display();
	}

	TwTerminate();

	return 0;
}
