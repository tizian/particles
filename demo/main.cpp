/*
    demo/main.cpp -- Demo application
    
    Copyright (c) 2016 Tizian Zeltner

    Particles is provided under the MIT License.
    See the LICENSE.txt file for the conditions of the license. 
*/

#include <Particles/ParticleSystem.h>

#include <AntTweakBar.h>
#include "AntTweakBarInput.h"

#include <iostream>
#include <memory>

// Window size
const int WIDTH = 1280;
const int HEIGHT = 720;

bool firstStart = true;

// AntTweakBar
TwBar *bar = nullptr;

// Enums for application and ATB
enum ERenderMode { PointRendering, TextureRendering, MetaballRendering, SpritesheetRendering, AnimatedSpritesheetRendering };
ERenderMode renderMode = ERenderMode::PointRendering;
ERenderMode lastRenderMode = ERenderMode::TextureRendering;

TwEnumVal renderModeEV[] = {
	{ ERenderMode::PointRendering, "Point" },
	{ ERenderMode::TextureRendering, "Texture" },
	{ ERenderMode::MetaballRendering, "Metaballs" },
	{ ERenderMode::SpritesheetRendering, "Spritesheet" },
	{ ERenderMode::AnimatedSpritesheetRendering, "AnimatedSpritesheet" }
};
TwType renderModeType;

enum EPosGenMode { Point, Box, Circle, Disk, EPosGenModeCount };
EPosGenMode posGenMode = EPosGenMode::Point;
EPosGenMode lastPosGenMode = EPosGenMode::EPosGenModeCount;

TwEnumVal posGenModeEV[] = {
	{ EPosGenMode::Point, "Point" },
	{ EPosGenMode::Box, "Box" },
	{ EPosGenMode::Circle, "Circle" },
	{ EPosGenMode::Disk, "Disk" }
};
TwType TW_TYPE_POS_GEN_MODE;

enum EVelGenMode { Vector, Angle, Aimed, EVelGenModeCount };
EVelGenMode velGenMode = EVelGenMode::Angle;
EVelGenMode lastVelGenMode = EVelGenMode::EVelGenModeCount;

TwEnumVal velGenModeEV[] = {
	{ EVelGenMode::Vector, "Vector" },
	{ EVelGenMode::Angle, "Angle" },
	{ EVelGenMode::Aimed, "Aimed" }
};
TwType TW_TYPE_VEL_GEN_MODE;

enum ESelectedTexture { Round, Blob, Star, NO_TEX };
ESelectedTexture selectedTex = ESelectedTexture::Round;
ESelectedTexture lastSelectedTex = ESelectedTexture::Round;

TwEnumVal texEV[] = {
	{ ESelectedTexture::Round, "Circle" },
	{ ESelectedTexture::Blob, "Blob" },
	{ ESelectedTexture::Star, "Star" }
};
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
std::unique_ptr<sf::Texture> spritesheetTexture;

// Particle System
std::unique_ptr<particles::ParticleSystem> particleSystem;

// Emitter Position of Particle System
sf::Vector2f *position = nullptr;

particles::ParticleGenerator *positionGenerator;
particles::ParticleGenerator *velocityGenerator;

void configureVelGen(EVelGenMode mode);
void configurePosGen(EPosGenMode mode);

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
	velGen->minStartSpeed = 100.f;
	velGen->maxStartSpeed = 100.f;

	if (!firstStart) {
		TwAddVarRW(bar, "velAngleMinAngle", TW_TYPE_FLOAT, &velGen->minAngle, " min=-360 max=360 group='Velocity' label='Min. Angle [deg]' ");
		TwAddVarRW(bar, "velAngleMaxAngle", TW_TYPE_FLOAT, &velGen->maxAngle, " min=-360 max=360 group='Velocity' label='Max. Angle [deg]' ");
		TwAddVarRW(bar, "velAngleMinSpeed", TW_TYPE_FLOAT, &velGen->minStartSpeed, " group='Velocity' label='Min. Start Speed' ");
		TwAddVarRW(bar, "velAngleMaxSpeed", TW_TYPE_FLOAT, &velGen->maxStartSpeed, " group='Velocity' label='Max. Start Speed' ");
	}
	firstStart = false;
	
	if (mode != ERenderMode::PointRendering) {
		auto sizeGen = particleSystem->addGenerator<particles::SizeGenerator>();
		sizeGen->minStartSize = 20.f;
		sizeGen->maxStartSize = 60.f;
		sizeGen->minEndSize = 10.f;
		sizeGen->maxEndSize = 30.f;

		TwAddVarRW(bar, "sizeGenMinStart", TW_TYPE_FLOAT, &sizeGen->minStartSize, " min=0 max=100 group='Size' label='Min. Start Size' ");
		TwAddVarRW(bar, "sizeGenMaxStart", TW_TYPE_FLOAT, &sizeGen->maxStartSize, " min=0 max=100 group='Size' label='Max. Start Size' ");
		TwAddVarRW(bar, "sizeGenMinEnd", TW_TYPE_FLOAT, &sizeGen->minEndSize, " min=0 max=100 group='Size' label='Min. End Size' ");
		TwAddVarRW(bar, "sizeGenMaxEnd", TW_TYPE_FLOAT, &sizeGen->maxEndSize, " min=0 max=100 group='Size' label='Max. End Size' ");

		auto rotGen = particleSystem->addGenerator<particles::RotationGenerator>();
		rotGen->minStartAngle = -20.f;
		rotGen->maxStartAngle = -20.f;
		rotGen->minEndAngle = 90.f;
		rotGen->maxEndAngle = 90.f;

		TwAddVarRW(bar, "rotGenMinStart", TW_TYPE_FLOAT, &rotGen->minStartAngle, " min=-360 max=360 group='Rotation' label='Min. Start Angle [deg]' ");
		TwAddVarRW(bar, "rotGenMaxStart", TW_TYPE_FLOAT, &rotGen->maxStartAngle, " min=-360 max=360 group='Rotation' label='Max. Start Angle [deg]' ");
		TwAddVarRW(bar, "rotGenMinEnd", TW_TYPE_FLOAT, &rotGen->minEndAngle, " min=-360 max=360 group='Rotation' label='Min. End Angle [deg]' ");
		TwAddVarRW(bar, "rotGenMaxEnd", TW_TYPE_FLOAT, &rotGen->maxEndAngle, " min=-360 max=360 group='Rotation' label='Max. End Angle [deg]' ");
	}
	
	auto timeGen = particleSystem->addGenerator<particles::TimeGenerator>();
	timeGen->minTime = 1.f;
	timeGen->maxTime = 5.f;

	TwAddVarRW(bar, "timeGenMin", TW_TYPE_FLOAT, &timeGen->minTime, " min=0 max=360 group='Time' label='Min. Lifetime [s]' ");
	TwAddVarRW(bar, "timeGenMax", TW_TYPE_FLOAT, &timeGen->maxTime, " min=0 max=360 group='Time' label='Max. Lifetime [s]' ");

	if (mode != ERenderMode::MetaballRendering) {
		auto colGen = particleSystem->addGenerator<particles::ColorGenerator>();
		colGen->minStartCol = sf::Color(16, 124, 167, 255);
		colGen->maxStartCol = sf::Color(30, 150, 255, 255);
		colGen->minEndCol = sf::Color(57, 0, 150, 0);
		colGen->maxEndCol = sf::Color(235, 128, 220, 0);

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
	auto rotUpdater = particleSystem->addUpdater<particles::RotationUpdater>();

	auto collisionUpdater = particleSystem->addUpdater<particles::VerticalCollisionUpdater>();
	collisionUpdater->pos = HEIGHT;

	auto eulerUpdater = particleSystem->addUpdater<particles::EulerUpdater>();

	TwAddVarRW(bar, "eulerGlobalAcc", TW_TYPE_VECTOR2F, &eulerUpdater->globalAcceleration, "group='Physics' label='Global Acceleration' ");
	TwAddVarRW(bar, "pos", TW_TYPE_FLOAT, &collisionUpdater->pos, "group='Physics' label='Collider Position' ");

	if (mode == ERenderMode::SpritesheetRendering) {
		auto texCoordGen = particleSystem->addGenerator<particles::TexCoordsRandomGenerator>();
		texCoordGen->texCoords.push_back(sf::IntRect(0, 0, 8, 8));
		texCoordGen->texCoords.push_back(sf::IntRect(8, 0, 8, 8));
		texCoordGen->texCoords.push_back(sf::IntRect(16, 0, 8, 8));
		texCoordGen->texCoords.push_back(sf::IntRect(24, 0, 8, 8));
	}
	else if (mode == ERenderMode::AnimatedSpritesheetRendering) {
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

			posGen->size = { 160.f, 60.f };

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
			velGen->minStartSpeed = 100.f;
			velGen->maxStartSpeed = 100.f;

			TwAddVarRW(bar, "velAngleMinAngle", TW_TYPE_FLOAT, &velGen->minAngle, " min=-360 max=360 group='Velocity' label='Min. angle [deg]' ");
			TwAddVarRW(bar, "velAngleMaxAngle", TW_TYPE_FLOAT, &velGen->maxAngle, " min=-360 max=360 group='Velocity' label='Max. angle [deg]' ");
			TwAddVarRW(bar, "velAngleMinSpeed", TW_TYPE_FLOAT, &velGen->minStartSpeed, " group='Velocity' label='Min. Start Speed' ");
			TwAddVarRW(bar, "velAngleMaxSpeed", TW_TYPE_FLOAT, &velGen->maxStartSpeed, " group='Velocity' label='Max. Start Speed' ");

		}
		break;
		case EVelGenMode::Aimed: {
			auto velGen = particleSystem->addGenerator<particles::AimedVelocityGenerator>();
			velocityGenerator = velGen;

			velGen->goal = sf::Vector2f(0.5f * WIDTH, 0.5f * HEIGHT);
			velGen->minStartSpeed = 100.f;
			velGen->maxStartSpeed = 100.f;

			TwAddVarRW(bar, "velAimedGoal", TW_TYPE_VECTOR2F, &velGen->goal, " group='Velocity' label='Goal' ");
			TwAddVarRW(bar, "velAimedMinSpeed", TW_TYPE_FLOAT, &velGen->minStartSpeed, " group='Velocity' label='Min. Start Speed' ");
			TwAddVarRW(bar, "velAimedMaxSpeed", TW_TYPE_FLOAT, &velGen->maxStartSpeed, " group='Velocity' label='Max. Start Speed' ");
		}
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
			TwRemoveVar(bar, "velAngleMinSpeed");
			TwRemoveVar(bar, "velAngleMaxSpeed");
		}
		else if (lastVelGenMode == EVelGenMode::Aimed) {
			TwRemoveVar(bar, "velAimedGoal");
			TwRemoveVar(bar, "velAimedMinSpeed");
			TwRemoveVar(bar, "velAimedMaxSpeed");
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

		renderModeType = TwDefineEnum("RenderModeType", renderModeEV, 5);
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

			particleSystem.reset(new particles::MetaballParticleSystem(10000, blobTexture.get(), WIDTH, HEIGHT));
			((particles::MetaballParticleSystem *)particleSystem.get())->color = sf::Color(20, 50, 100, 255);

			TwAddVarRW(bar, "threshold", TW_TYPE_FLOAT, &((particles::MetaballParticleSystem *)particleSystem.get())->threshold, " min=0.1 max=0.9 step=0.1 group='Metaball' label='Threshold' ");
			TwAddVarRW(bar, "color", TW_TYPE_COLOR32, &((particles::MetaballParticleSystem *)particleSystem.get())->color, " group='Metaball' label='Color' ");
			TwAddVarRW(bar, "colorAlpha", TW_TYPE_UINT8, &((particles::MetaballParticleSystem *)particleSystem.get())->color.a, " group='Metaball' label='Color Alpha' ");
		}
		else if (renderMode == ERenderMode::SpritesheetRendering || renderMode == ERenderMode::AnimatedSpritesheetRendering) {
			posGenMode = EPosGenMode::Point;
			velGenMode = EVelGenMode::Angle;

			particleSystem.reset(new particles::SpriteSheetParticleSystem(10000, spritesheetTexture.get()));
		}

		particleSystem->emitRate = 160.f;
		TwAddVarRW(bar, "EmitRate", TW_TYPE_FLOAT, &particleSystem->emitRate, " group='General' label='Particle Emit Rate' ");

		configurePS(renderMode);
	}
}

int main() {
	// create the window
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Particles");
	window.setVerticalSyncEnabled(true);

	TwInit(TW_OPENGL, NULL);
	TwWindowSize(WIDTH, HEIGHT);

	bar = TwNewBar("Particles");

	TwDefine(" GLOBAL help='2D Particle Test Application' ");
	TwDefine(" Particles size='300 500' ");

	TW_TYPE_POS_GEN_MODE = TwDefineEnum("PosGenModeType", posGenModeEV, 4);
	TW_TYPE_VEL_GEN_MODE = TwDefineEnum("VelGenModeType", velGenModeEV, 3);
	TW_TYPE_SELECTED_TEXTURE = TwDefineEnum("TexType", texEV, 3);
	TW_TYPE_VECTOR2F = TwDefineStruct("Vector2f", vector2fMembers, 2, sizeof(sf::Vector2f), NULL, NULL);

	circleTexture = std::unique_ptr<sf::Texture>(new sf::Texture());
	blobTexture = std::unique_ptr<sf::Texture>(new sf::Texture());
	starTexture = std::unique_ptr<sf::Texture>(new sf::Texture());
	spritesheetTexture = std::unique_ptr<sf::Texture>(new sf::Texture());
	if (!circleTexture->loadFromFile("res/circleTexture.png")) {
		std::cout << "Invalid path to texture." << std::endl;
	}
	if (!blobTexture->loadFromFile("res/blobTexture.png")) {
		std::cout << "Invalid path to texture." << std::endl;
	}
	if (!starTexture->loadFromFile("res/starTexture.png")) {
		std::cout << "Invalid path to texture." << std::endl;
	}
	if (!spritesheetTexture->loadFromFile("res/spritesheet.png")) {
		std::cout << "Invalid path to texture." << std::endl;
	}

	circleTexture->setSmooth(true);
	blobTexture->setSmooth(true);
	starTexture->setSmooth(true);
	spritesheetTexture->setSmooth(false);
	
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
