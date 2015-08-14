#include <iostream>

#include <SFML/Graphics.hpp>
#include <AntTweakBar.h>

#include "AntTweakBarInput.h"

#include "Particles/ParticleSystem.h"

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
TwType TW_TYPE_POS_GEN_MODE;

enum EVelGenMode { Vector, Angle, EVelGenModeCount };
EVelGenMode velGenMode = EVelGenMode::Angle;
EVelGenMode lastVelGenMode = EVelGenMode::EVelGenModeCount;

TwEnumVal velGenModeEV[] = { { EVelGenMode::Vector, "Vector" }, { EVelGenMode::Angle, "Angle" } };
TwType TW_TYPE_VEL_GEN_MODE;

enum ESelectedTexture { Round, Blob, Star, NO_TEX };
ESelectedTexture selectedTex = ESelectedTexture::Round;
ESelectedTexture lastSelectedTex = ESelectedTexture::Round;

TwEnumVal texEV[] = { { ESelectedTexture::Round, "Circle" }, { ESelectedTexture::Blob, "Blob" }, { ESelectedTexture::Star, "Star" } };
TwType TW_TYPE_SELECTED_TEXTURE;

// Struct definitions for ATB
TwStructMember vector2fMembers[] = {
	{ "x", TW_TYPE_FLOAT, offsetof(sf::Vector2f, x), "" },
	{ "y", TW_TYPE_FLOAT, offsetof(sf::Vector2f, y), "" },
};
TwType TW_TYPE_VECTOR2F;

// Textures
std::unique_ptr<sf::Texture> circleTexture;
std::unique_ptr<sf::Texture> blobTexture;
std::unique_ptr<sf::Texture> starTexture;

// Render Texture used for metaball rendering
std::unique_ptr<sf::RenderTexture> renderTexture;

// Particle System
std::unique_ptr<particles::ParticleSystem> particleSystem;

// Emitter Position of Particle System
sf::Vector2f *position = nullptr;

std::shared_ptr<particles::ParticleGenerator> positionGenerator;
std::shared_ptr<particles::ParticleGenerator> velocityGenerator;

void configurePS(ERenderMode mode) {
	TwAddVarRW(bar, "PosGenMode", TW_TYPE_POS_GEN_MODE, &posGenMode, " group='Position' label='Generator Type' ");

	auto posGen = particleSystem->addGenerator<particles::PointPositionGenerator>();
	positionGenerator = posGen;
	position = &posGen->center;

	TwAddVarRW(bar, "VelGenMode", TW_TYPE_VEL_GEN_MODE, &velGenMode, " group='Velocity' label='Generator Type' ");

	auto velGen = particleSystem->addGenerator<particles::AngledVelocityGenerator>();
	velocityGenerator = velGen;

	velGen->minAngle = -20.f;
	velGen->maxAngle = 20.f;
	velGen->minStartVel = 100.f;
	velGen->maxStartVel = 100.f;

	if (!firstStart) {
		TwAddVarRW(bar, "velAngleMinAngle", TW_TYPE_FLOAT, &velGen->minAngle, " min=-360 max=360 group='Velocity' label='Min. angle [deg]' ");
		TwAddVarRW(bar, "velAngleMaxAngle", TW_TYPE_FLOAT, &velGen->maxAngle, " min=-360 max=360 group='Velocity' label='Max. angle [deg]' ");
		TwAddVarRW(bar, "velAngleMinVel", TW_TYPE_FLOAT, &velGen->minStartVel, " group='Velocity' label='Min. Start Velocity' ");
		TwAddVarRW(bar, "velAngleMaxVel", TW_TYPE_FLOAT, &velGen->maxStartVel, " group='Velocity' label='Max. Start Velocity' ");
	}
	firstStart = false;
	
	if (mode != ERenderMode::PointRendering) {
		auto sizeGen = particleSystem->addGenerator<particles::SizeGenerator>();
		sizeGen->minStartSize = 10.0f;
		sizeGen->maxStartSize = 30.0f;
		sizeGen->minEndSize = 5.f;
		sizeGen->maxEndSize = 15.f;

		TwAddVarRW(bar, "sizeGenMinStart", TW_TYPE_FLOAT, &sizeGen->minStartSize, " min=0 max=100 group='Size' label='Min. Start Size' ");
		TwAddVarRW(bar, "sizeGenMaxStart", TW_TYPE_FLOAT, &sizeGen->maxStartSize, " min=0 max=100 group='Size' label='Max. Start Size' ");
		TwAddVarRW(bar, "sizeGenMinEnd", TW_TYPE_FLOAT, &sizeGen->minEndSize, " min=0 max=100 group='Size' label='Min. End Size' ");
		TwAddVarRW(bar, "sizeGenMaxEnd", TW_TYPE_FLOAT, &sizeGen->maxEndSize, " min=0 max=100 group='Size' label='Max. End Size' ");
	}
	

	auto timeGen = particleSystem->addGenerator<particles::TimeGenerator>();
	timeGen->minTime = 1.0f;
	timeGen->maxTime = 5.0f;

	TwAddVarRW(bar, "timeGenMin", TW_TYPE_FLOAT, &timeGen->minTime, " min=0 max=360 group='Time' label='Min. Lifetime [s]' ");
	TwAddVarRW(bar, "timeGenMax", TW_TYPE_FLOAT, &timeGen->maxTime, " min=0 max=360 group='Time' label='Max. Lifetime [s]' ");

	if (mode != ERenderMode::MetaballRendering) {
		auto colGen = particleSystem->addGenerator<particles::ColorGenerator>();
		colGen->minStartCol = sf::Color(150, 0, 180, 255);
		colGen->maxStartCol = sf::Color(220, 255, 220, 255);
		colGen->minEndCol = sf::Color(128, 0, 150, 0);
		colGen->maxEndCol = sf::Color(180, 128, 220, 0);

		TwAddVarRW(bar, "colorGenMinStart", TW_TYPE_COLOR32, &colGen->minStartCol, "group='Color' label='Min. Start Color' ");
		TwAddVarRW(bar, "colorGenMinStartAlpha", TW_TYPE_UINT8, &colGen->minStartCol.a, " group='Color' label='Min. Start Color Alpha' ");
		TwAddVarRW(bar, "colorGenMaxStart", TW_TYPE_COLOR32, &colGen->maxStartCol, "group='Color' label='Max. Start Color' ");
		TwAddVarRW(bar, "colorGenMaxStartAlpha", TW_TYPE_UINT8, &colGen->maxStartCol.a, " group='Color' label='Max. Start Color Alpha' ");
		TwAddVarRW(bar, "colorGenMinEnd", TW_TYPE_COLOR32, &colGen->minEndCol, "group='Color' label='Min. End Color' ");
		TwAddVarRW(bar, "colorGenMinEndAlpha", TW_TYPE_UINT8, &colGen->minEndCol.a, " group='Color' label='Min. End Color Alpha' ");
		TwAddVarRW(bar, "colorGenMaxEnd", TW_TYPE_COLOR32, &colGen->maxEndCol, "group='Color' label='Max. End Color' ");
		TwAddVarRW(bar, "colorGenMaxEndAlpha", TW_TYPE_UINT8, &colGen->maxEndCol.a, " group='Color' label='Max. End Color Alpha' ");
	}

	// Updaters
	auto timeUpdater = particleSystem->addUpdater<particles::TimeUpdater>();
	auto colorUpdater = particleSystem->addUpdater<particles::ColorUpdater>();
	auto sizeUpdater = particleSystem->addUpdater<particles::SizeUpdater>();
	auto eulerUpdater = particleSystem->addUpdater<particles::EulerUpdater>();

	TwAddVarRW(bar, "eulerGlobalAcc", TW_TYPE_VECTOR2F, &eulerUpdater->globalAcceleration, "group='Physics' label='Global Acceleration' ");
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

			TwAddVarRW(bar, "posBoxSize", TW_TYPE_VECTOR2F, &posGen->size, "group='Position' label='Size' ");
		}
		break;
		case EPosGenMode::Circle: {
			auto posGen = particleSystem->addGenerator<particles::CirclePositionGenerator>();
			positionGenerator = posGen;
			position = &posGen->center;

			posGen->radius = { 70.f, 40.f };

			TwAddVarRW(bar, "posCircleRadius", TW_TYPE_VECTOR2F, &posGen->radius, "group='Position' label='Radius' ");

		}
		break;
		case EPosGenMode::Disk: {
			auto posGen = particleSystem->addGenerator<particles::DiskPositionGenerator>();
			positionGenerator = posGen;
			position = &posGen->center;

			posGen->radius = 150.f;

			TwAddVarRW(bar, "posDiskRadius", TW_TYPE_FLOAT, &posGen->radius, "group='Position' label='Radius' ");

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

			TwAddVarRW(bar, "velVectorMinStart", TW_TYPE_VECTOR2F, &velGen->minStartVel, " group='Velocity' label='Min. Start Velocity' ");
			TwAddVarRW(bar, "velVectorMaxStart", TW_TYPE_VECTOR2F, &velGen->maxStartVel, " group='Velocity' label='Max. Start Velocity' ");

		}
		break;
		case EVelGenMode::Angle: {
			auto velGen = particleSystem->addGenerator<particles::AngledVelocityGenerator>();
			velocityGenerator = velGen;

			velGen->minAngle = -20.f;
			velGen->maxAngle = 20.f;
			velGen->minStartVel = 100.f;
			velGen->maxStartVel = 100.0f;

			TwAddVarRW(bar, "velAngleMinAngle", TW_TYPE_FLOAT, &velGen->minAngle, " min=-360 max=360 group='Velocity' label='Min. angle [deg]' ");
			TwAddVarRW(bar, "velAngleMaxAngle", TW_TYPE_FLOAT, &velGen->maxAngle, " min=-360 max=360 group='Velocity' label='Max. angle [deg]' ");
			TwAddVarRW(bar, "velAngleMinVel", TW_TYPE_FLOAT, &velGen->minStartVel, " group='Velocity' label='Min. Start Velocity' ");
			TwAddVarRW(bar, "velAngleMaxVel", TW_TYPE_FLOAT, &velGen->maxStartVel, " group='Velocity' label='Max. Start Velocity' ");

		}
		break;
		default:
		break;
	}
}

void updatePosGen() {
	if (posGenMode != lastPosGenMode) {
		if (lastPosGenMode == EPosGenMode::Box) {
			TwRemoveVar(bar, "posBoxSize");
		}
		else if (lastPosGenMode == EPosGenMode::Circle) {
			TwRemoveVar(bar, "posCircleRadius");
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
			TwRemoveVar(bar, "velVectorMinStart");
			TwRemoveVar(bar, "velVectorMaxStart");
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
		else if (selectedTex == ESelectedTexture::Star) {
			((particles::TextureParticleSystem *)particleSystem.get())->setTexture(starTexture.get());
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
			posGenMode = EPosGenMode::Point;
			velGenMode = EVelGenMode::Angle;

			particleSystem.reset(new particles::PointParticleSystem(10000));
		}
		else if (renderMode == ERenderMode::TextureRendering) {
			posGenMode = EPosGenMode::Point;
			velGenMode = EVelGenMode::Angle;
			selectedTex = ESelectedTexture::Round;

			particleSystem.reset(new particles::TextureParticleSystem(10000, circleTexture.get()));
			((particles::TextureParticleSystem *)particleSystem.get())->additiveBlendMode = false;

			TwAddVarRW(bar, "BlendMode", TW_TYPE_BOOL8, &((particles::TextureParticleSystem *)particleSystem.get())->additiveBlendMode, " group='Texture' label='Additive Blend Mode' ");

			TwAddVarRW(bar, "SelectedTex", TW_TYPE_SELECTED_TEXTURE, &selectedTex, " group='Texture' label='Used Texture' ");
		}
		else if (renderMode == ERenderMode::MetaballRendering) {
			posGenMode = EPosGenMode::Point;
			velGenMode = EVelGenMode::Angle;

			particleSystem.reset(new particles::MetaballParticleSystem(10000, blobTexture.get(), renderTexture.get()));
			((particles::MetaballParticleSystem *)particleSystem.get())->color = sf::Color(20, 50, 100, 255);

			TwAddVarRW(bar, "threshold", TW_TYPE_FLOAT, &((particles::MetaballParticleSystem *)particleSystem.get())->threshold, " min=0.1 max=0.9 step=0.1 group='Metaball' label='Threshold' ");
			TwAddVarRW(bar, "color", TW_TYPE_COLOR32, &((particles::MetaballParticleSystem *)particleSystem.get())->color, " group='Metaball' label='Color' ");
			TwAddVarRW(bar, "colorAlpha", TW_TYPE_UINT8, &((particles::MetaballParticleSystem *)particleSystem.get())->color.a, " group='Metaball' label='Color Alpha' ");
		}

		particleSystem->emitRate = 10000.0f / 4.0f / 10.f;
		TwAddVarRW(bar, "EmitRate", TW_TYPE_FLOAT, &particleSystem->emitRate, " group='General' label='Particle Emit Rate' ");

		configurePS(renderMode);
	}
}

int main() {
	float dt = 1.f / 60.f;
	float a = 2.3f * dt;

	int n = static_cast<int>(a / dt);
	float rem = std::fmod(a, dt);

	std::cout << a << std::endl;
	std::cout << "n: " << n << std::endl;
	std::cout << "rem: " << rem << std::endl;
	std::cout << "real rem: " << a - n * dt << std::endl;

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

	TW_TYPE_POS_GEN_MODE = TwDefineEnum("PosGenModeType", posGenModeEV, 4);
	TW_TYPE_VEL_GEN_MODE = TwDefineEnum("VelGenModeType", velGenModeEV, 2);
	TW_TYPE_SELECTED_TEXTURE = TwDefineEnum("TexType", texEV, 3);
	TW_TYPE_VECTOR2F = TwDefineStruct("Vector2f", vector2fMembers, 2, sizeof(sf::Vector2f), NULL, NULL);

	circleTexture.reset(new sf::Texture());
	blobTexture.reset(new sf::Texture());
	starTexture.reset(new sf::Texture());
	if (!circleTexture->loadFromFile("res/circleTexture.png")) {
		std::cout << "Invalid path to texture." << std::endl;
		wrongDir = true;
	}
	if (!blobTexture->loadFromFile("res/blobTexture.png")) {
		std::cout << "Invalid path to texture." << std::endl;
		wrongDir = true;
	}
	if (!starTexture->loadFromFile("res/starTexture.png")) {
		std::cout << "Invalid path to texture." << std::endl;
		wrongDir = true;
	}

	if (wrongDir) {
		std::cerr << "Please use the 'particles' root directory as working directory." << std::endl;
		return 0;
	}

	circleTexture->setSmooth(true);
	blobTexture->setSmooth(true);
	starTexture->setSmooth(true);
	
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
