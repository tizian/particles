#pragma once

#include "ParticleGenerator.h"

namespace particles
{
	class ParticleEmitter
	{
	public:
		ParticleEmitter() { }
		virtual ~ParticleEmitter() { }

		// calls all the generators and at the end it activates (wakes) particle
		virtual void emit(float dt, ParticleData *p);

		void setSystem(ParticleSystem *s) { m_system = s; }

		void addGenerator(std::shared_ptr<ParticleGenerator> gen)
		{ 
			gen->setSystem(m_system);
			m_generators.push_back(gen);
		}

	public:
		float emitRate{ 0.0 };

	protected:
		ParticleSystem *m_system;
		std::vector<std::shared_ptr<ParticleGenerator>> m_generators;
	};
}
