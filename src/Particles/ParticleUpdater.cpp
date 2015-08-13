#include "Particles/ParticleUpdater.h"

namespace particles
{
	inline float dot(const sf::Vector2f &a, const sf::Vector2f &b) {
		return a.x * b.x + a.y * b.y;
	}

	inline float lerpFloat(float a, float b, float alpha) {
		return a * (1.0f - alpha) + b * alpha;
	}

	inline sf::Color lerpColor(const sf::Color &c1, const sf::Color &c2, float alpha) {
		sf::Uint8 r = (sf::Uint8)(c1.r * (1.0f - alpha) + c2.r * alpha);
		sf::Uint8 g = (sf::Uint8)(c1.g * (1.0f - alpha) + c2.g * alpha);
		sf::Uint8 b = (sf::Uint8)(c1.b * (1.0f - alpha) + c2.b * alpha);
		sf::Uint8 a = (sf::Uint8)(c1.a * (1.0f - alpha) + c2.a * alpha);

		return sf::Color(r, g, b, a);
	}

	void EulerUpdater::update(ParticleData *data, float dt) {
		const int endId = data->countAlive;

		for (int i = 0; i < endId; ++i) {
			data->acc[i] += dt * globalAcceleration;
		}

		for (int i = 0; i < endId; ++i) {
			data->vel[i] += dt * data->acc[i];
		}

		for (int i = 0; i < endId; ++i) {
			data->pos[i] += dt * data->vel[i];
		}
	}


	void FloorUpdater::update(ParticleData *data, float dt) {
		const int endId = data->countAlive;

		for (int i = 0; i < endId; ++i) {
			if (data->pos[i].y < floorY) {
				sf::Vector2f force = data->acc[i];
				float normalFactor = dot(force, sf::Vector2f(0.0f, 1.0f));
				if (normalFactor < 0.0f) {
					force -= sf::Vector2f(0.0f, 1.0f) * normalFactor;
				}

				float velFactor = dot(data->vel[i], sf::Vector2f(0.0f, 1.0f));
				data->vel[i] -= sf::Vector2f(0.0f, 1.0f) * (1.0f + bounceFactor) * velFactor;
				data->acc[i] = force;
			}
		}
	}


	void AttractorUpdater::update(ParticleData *data, float dt) {
		const int endId = data->countAlive;
		const int numAttractors = m_attractors.size();
		sf::Vector2f off;
		float dist;

		for (int i = 0; i < endId; ++i) {
			for (int j = 0; j < numAttractors; ++j) {
				off.x = m_attractors[j].x - data->pos[i].x;
				off.y = m_attractors[j].y - data->pos[i].y;
				dist = dot(off, off);
				dist = m_attractors[j].z / dist;

				data->acc[i] += off * dist;
			}
		}
	}


	void SizeUpdater::update(ParticleData *data, float dt) {
		const int endId = data->countAlive;

		for (int i = 0; i < endId; ++i) {
			float a = data->time[i].z;
			data->size[i].x = lerpFloat(data->size[i].y, data->size[i].z, a);
		}
	}


	void ColorUpdater::update(ParticleData *data, float dt) {
		const int endId = data->countAlive;

		for (int i = 0; i < endId; ++i) {
			float a = data->time[i].z;
			data->col[i] = lerpColor(data->startCol[i], data->endCol[i], a);
		}
	}


	void TimeUpdater::update(ParticleData *data, float dt) {
		int endId = data->countAlive;

		if (endId == 0) return;

		for (int i = 0; i < endId; ++i) {
			data->time[i].x -= dt;
			data->time[i].z = 1.0f - (data->time[i].x / data->time[i].y);

			if (data->time[i].x < 0.0f) {
				data->kill(i);
				if (data->countAlive < data->count) {
					endId = data->countAlive;
				}
				else {
					endId = data->count;
				}
			}
		}
	}
}
