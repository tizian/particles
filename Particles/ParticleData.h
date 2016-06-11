/*
    Particles/ParticleData.h -- Class to define all particle state
    
    Copyright (c) 2016 Tizian Zeltner

    Particles is provided under the MIT License.
    See the LICENSE.txt file for the conditions of the license. 
*/

#pragma once

#include <SFML/Graphics.hpp>

namespace particles {

class ParticleData {
public:
    explicit ParticleData(int maxCount);
    ~ParticleData();

    ParticleData(const ParticleData &) = delete;
    ParticleData &operator=(const ParticleData &) = delete;

    void kill(int id);
    void swapData(int id1, int id2);

public:
    sf::Vector2f *pos;        // Current position
    sf::Vector2f *vel;        // Current velocity
    sf::Vector2f *acc;        // Current acceleration
    sf::Vector3f *time;       // x: remaining time to live,   y: time to live,    z: interpolation value in [0, 1] of lifetime
    sf::Vector3f *size;       // x: current size,             y: start size,      z: end size
	sf::Vector3f *angle;	  // x: current angle,            y: start rotation,  z: end rotation
    sf::Color    *col;        // Current color
    sf::Color    *startCol;   // Start color
    sf::Color    *endCol;     // End color
	sf::IntRect  *texCoords;  // Texture coordinates inside spritesheet
	int          *frame;	  // Frame index for animation
	float        *frameTimer; // Accumulator for animation

    int           count;
    int           countAlive;
};

}