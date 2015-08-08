#include "ParticleEmitter.h"

namespace particles
{
	void ParticleEmitter::emit(float dt, ParticleData *p)
	{
		const size_t maxNewParticles = static_cast<size_t>(dt * emitRate);
		const size_t startId = p->countAlive;
		const size_t endId = std::min(startId + maxNewParticles, p->count-1);

		for (auto &gen : m_generators)
			gen->generate(dt, p, startId, endId);

		for (size_t i = startId; i < endId; ++i)
		{
			p->wake(i);
		}
	}
}
