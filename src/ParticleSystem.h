#pragma once

#include "ParticleData.h"
#include "ParticleEmitter.h"
#include "ParticleUpdater.h"

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:
	explicit ParticleSystem(size_t maxCount);
	virtual ~ParticleSystem() {}

	ParticleSystem(const ParticleSystem &) = delete;
	ParticleSystem &operator=(const ParticleSystem &) = delete;

	virtual void update(float dt);
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	virtual void reset();

	virtual size_t numAllParticles() const { return m_particles.count; }
	virtual size_t numAliveParticles() const { return m_particles.countAlive; }

	void addEmitter(std::shared_ptr<ParticleEmitter> em) { m_emitters.push_back(em); }
	void addUpdater(std::shared_ptr<ParticleUpdater> up) { m_updaters.push_back(up); }

	ParticleData *finalData() { return &m_particles; }

protected:
	ParticleData m_particles;
	sf::VertexArray m_vertices;

	size_t m_count;

	std::vector<std::shared_ptr<ParticleEmitter>> m_emitters;
	std::vector<std::shared_ptr<ParticleUpdater>> m_updaters;
};