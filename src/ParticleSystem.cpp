#include "ParticleSystem.h"

namespace particles
{
	ParticleSystem::ParticleSystem(size_t maxCount, bool active) : m_particles(maxCount), m_vertices(sf::Points, maxCount), active(active)
	{
		m_count = maxCount;

		for (size_t i = 0; i < maxCount; ++i)
			m_particles.alive[i] = false;
	}

	void ParticleSystem::emit(int maxCount)
	{
		for (auto & em : m_emitters)
		{
			em->emit(maxCount, &m_particles);
		}
	}

	void ParticleSystem::update(float dt)
	{
		if (active)
		{
			for (auto & em : m_emitters)
			{
				em->emit(dt, &m_particles);
			}
		}

		for (size_t i = 0; i < m_count; ++i)
		{
			m_particles.acc[i] = { 0.0f, 0.0f };
		}

		for (auto & up : m_updaters)
		{
			up->update(dt, &m_particles);
		}

		for (size_t i = 0; i < m_count; ++i)
		{
			m_vertices[i].position = m_particles.pos[i];
			m_vertices[i].color = m_particles.col[i];
		}
	}

	void ParticleSystem::reset()
	{
		m_particles.countAlive = 0;
	}

	void ParticleSystem::draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		//states.transform *= getTransform();
		states.texture = nullptr;

		const sf::Vertex *ver = &m_vertices[0];
		target.draw(ver, m_particles.countAlive, sf::Points, states);
	}
}
