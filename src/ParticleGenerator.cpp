#include "ParticleGenerator.h"

#include "ParticleSystem.h"

namespace particles
{
	inline float randomFloat(float low, float high)
	{
		return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
	}

	inline int randomInt(int low, int high)
	{
		return (rand() % (int)(high - low + 1)) + low;
	}

	inline sf::Color randomColor(const sf::Color &low, const sf::Color &high)
	{
		sf::Uint8 r = (rand() % (int)(high.r - low.r + 1)) + low.r;
		sf::Uint8 g = (rand() % (int)(high.g - low.g + 1)) + low.g;
		sf::Uint8 b = (rand() % (int)(high.b - low.b + 1)) + low.b;
		sf::Uint8 a = (rand() % (int)(high.a - low.a + 1)) + low.a;

		return sf::Color(r, g, b, a);
	}

	inline sf::Vector2f randomVector2f(const sf::Vector2f &low, const sf::Vector2f &high)
	{
		float x = low.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high.x - low.x)));
		float y = low.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high.y - low.y)));

		return sf::Vector2f(x, y);
	}

	#ifndef M_PI
	#define M_PI 3.14159265358979323846f
	#endif

	void BoxPositionGenerator::generate(float dt, ParticleData *p, size_t startId, size_t endId)
	{
		const sf::Vector2f &pos = m_system->position;

		sf::Vector2f posMin{ pos.x - maxStartPosOffset.x, pos.y - maxStartPosOffset.y };
		sf::Vector2f posMax{ pos.x + maxStartPosOffset.x, pos.y + maxStartPosOffset.y };

		for (size_t i = startId; i < endId; ++i)
		{
			p->pos[i] = randomVector2f(posMin, posMax);
		}
	}


	void CirclePositionGenerator::generate(float dt, ParticleData *p, size_t startId, size_t endId)
	{
		const sf::Vector2f &center = m_system->position;

		for (size_t i = startId; i < endId; ++i)
		{
			float phi = randomFloat(0.0f, M_PI * 2.0f);
			p->pos[i] = { center.x + radius.x * std::sin(phi), center.y + radius.y * std::cos(phi) };
		}
	}

	void DiskPositionGenerator::generate(float dt, ParticleData *p, size_t startId, size_t endId)
	{
		const sf::Vector2f &center = m_system->position;

		for (size_t i = startId; i < endId; ++i)
		{
			float phi = randomFloat(0.0f, M_PI * 2.0f);
			float r = std::sqrt(randomFloat(0.0f, radius));
			p->pos[i] = { center.x + r * std::sin(phi), center.y + r * std::cos(phi) };
		}
	}


	void ColorGenerator::generate(float dt, ParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			p->startCol[i] = randomColor(minStartCol, maxStartCol);
			p->endCol[i] = randomColor(minEndCol, maxEndCol);
		}
	}


	void VelocityGenerator::generate(float dt, ParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			p->vel[i] = randomVector2f(minStartVel, maxStartVel);
		}
	}


	void TimeGenerator::generate(float dt, ParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			p->time[i].x = p->time[i].y = randomFloat(minTime, maxTime);
			p->time[i].z = (float)0.0;
		}
	}
}
