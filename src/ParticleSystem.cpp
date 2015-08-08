#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(size_t maxCount) : m_particles(maxCount), m_vertices(sf::Points, maxCount)
{
	m_count = maxCount;

	for (size_t i = 0; i < maxCount; ++i)
		m_particles.alive[i] = false;
}

void ParticleSystem::update(float dt)
{
	for (auto & em : m_emitters)
	{
		em->emit(dt, &m_particles);
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
	states.transform *= getTransform();
	states.texture = nullptr;

	const sf::Vertex *ver = &m_vertices[0];
	target.draw(ver, m_particles.countAlive, sf::Points, states);
}