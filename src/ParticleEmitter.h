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

		void addGenerator(std::shared_ptr<ParticleGenerator> gen) { m_generators.push_back(gen); }

	public:
		float emitRate{ 0.0 };

	protected:
		std::vector<std::shared_ptr<ParticleGenerator>> m_generators;
	};
}
