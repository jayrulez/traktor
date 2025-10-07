/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <random>
#include "Shine/Widgets/Particle.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.Particle", Particle, Visual)

Particle::Particle(const std::wstring& name)
:	Visual(name)
{
}

Particle::~Particle()
{
}

void Particle::play()
{
	m_isPlaying = true;
	m_isPaused = false;
	m_emissionTime = 0.0f;
}

void Particle::stop()
{
	m_isPlaying = false;
	m_isPaused = false;
	m_emissionTime = 0.0f;
}

void Particle::pause()
{
	m_isPaused = true;
}

void Particle::resume()
{
	m_isPaused = false;
}

void Particle::clear()
{
	m_particles.clear();
}

void Particle::burst(int count)
{
	for (int i = 0; i < count && (int)m_particles.size() < m_maxParticles; ++i)
		emitParticle();
}

void Particle::update(float deltaTime)
{
	Visual::update(deltaTime);

	if (m_isPlaying && !m_isPaused)
	{
		// Emit new particles
		m_emissionTime += deltaTime;
		float emissionInterval = 1.0f / m_emissionRate;

		while (m_emissionTime >= emissionInterval && (int)m_particles.size() < m_maxParticles)
		{
			m_emissionTime -= emissionInterval;
			emitParticle();
		}
	}

	// Update existing particles
	updateParticles(deltaTime);
}

void Particle::render(RenderContext* rc, float parentOpacity)
{
	if (!isVisible() || !m_particleTexture)
		return;

	float finalOpacity = getOpacity() * parentOpacity;

	// Render all particles
	for (const auto& particle : m_particles)
	{
		// Calculate lifetime progress (0-1)
		float progress = particle.lifetime / particle.maxLifetime;

		// Interpolate color
		Color4f color = Color4f(
			m_startColor.getRed() + (m_endColor.getRed() - m_startColor.getRed()) * progress,
			m_startColor.getGreen() + (m_endColor.getGreen() - m_startColor.getGreen()) * progress,
			m_startColor.getBlue() + (m_endColor.getBlue() - m_startColor.getBlue()) * progress,
			m_startColor.getAlpha() + (m_endColor.getAlpha() - m_startColor.getAlpha()) * progress
		);

		// Apply opacity
		color.setAlpha(Scalar(color.getAlpha() * finalOpacity));

		// Render particle
		Vector2 worldPos = getWorldPosition() + particle.position;
		Vector2 size = Vector2(particle.size, particle.size);

		rc->drawTexturedRect(
			m_particleTexture,
			worldPos,
			size,
			color,
			Vector2(0.0f, 0.0f),
			Vector2(1.0f, 1.0f),
			particle.rotation
		);
	}
}

void Particle::emitParticle()
{
	ParticleData particle;

	// Initialize position at emitter center
	particle.position = getSize() * 0.5f;

	// Initialize velocity with variance
	particle.velocity = Vector2(
		m_startVelocity.x + randomRange(-m_velocityVariance.x, m_velocityVariance.x),
		m_startVelocity.y + randomRange(-m_velocityVariance.y, m_velocityVariance.y)
	);

	// Initialize lifetime
	particle.maxLifetime = m_lifetime + randomRange(-m_lifetimeVariance, m_lifetimeVariance);
	particle.lifetime = 0.0f;

	// Initialize size and rotation
	particle.size = m_startSize;
	particle.rotation = 0.0f;
	particle.rotationSpeed = 0.0f;

	// Initialize color
	particle.color = m_startColor;

	m_particles.push_back(particle);
}

void Particle::updateParticles(float deltaTime)
{
	// Update all particles
	for (auto it = m_particles.begin(); it != m_particles.end(); )
	{
		auto& particle = *it;

		// Update lifetime
		particle.lifetime += deltaTime;

		// Remove dead particles
		if (particle.lifetime >= particle.maxLifetime)
		{
			it = m_particles.erase(it);
			continue;
		}

		// Update position
		particle.position += particle.velocity * deltaTime;

		// Apply gravity
		particle.velocity += m_gravity * deltaTime;

		// Update rotation
		particle.rotation += particle.rotationSpeed * deltaTime;

		++it;
	}
}

float Particle::randomRange(float min, float max) const
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}

}
