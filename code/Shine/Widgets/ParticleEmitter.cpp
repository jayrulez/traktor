/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Core/Math/Const.h"
#include "Shine/Widgets/ParticleEmitter.h"
#include "Shine/RenderContext.h"

namespace traktor::shine
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.shine.ParticleEmitter", ParticleEmitter, Visual)

ParticleEmitter::ParticleEmitter(const std::wstring& name)
:	Visual(name)
{
	std::random_device rd;
	m_random.seed(rd());
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::setRandomSeed(int seed)
{
	m_randomSeed = seed;
	m_random.seed(seed);
}

void ParticleEmitter::clearActiveParticles()
{
	m_particles.clear();
}

void ParticleEmitter::update(float deltaTime)
{
	Visual::update(deltaTime);

	// Update emitter age
	if (m_isEmitting)
	{
		m_emitterAge += deltaTime;

		// Check emitter lifetime
		if (!m_isEmitterLifetimeInfinite && m_emitterAge >= m_emitterLifetime)
		{
			m_isEmitting = false;
		}
		else
		{
			// Emit new particles
			m_nextEmitTime += deltaTime;
			float emitInterval = 1.0f / m_emissionRate;

			while (m_nextEmitTime >= emitInterval)
			{
				m_nextEmitTime -= emitInterval;

				// Check particle limit
				if (!m_isParticleCountLimited || m_particles.size() < m_maxParticles)
					emitParticle();
			}
		}
	}

	// Update existing particles
	updateParticles(deltaTime);
}

void ParticleEmitter::render(RenderContext* rc, float parentOpacity)
{
	if (!isVisible() || !m_sprite)
		return;

	float finalOpacity = getOpacity() * parentOpacity;
	Vector2 emitterWorldPos = getWorldPosition();

	// Render all particles
	for (const auto& particle : m_particles)
	{
		// Calculate lifetime progress (0-1)
		float progress = particle.lifetime / particle.maxLifetime;

		// Apply color/alpha (could use curves here in advanced version)
		Color4f color = particle.color;
		color.setAlpha(Scalar(color.getAlpha() * finalOpacity));

		// Calculate world position
		Vector2 worldPos = m_isPositionRelativeToEmitter
			? emitterWorldPos + particle.position
			: particle.position;

		// Calculate sprite sheet UVs if animated
		Vector2 uvMin(0, 0);
		Vector2 uvMax(1, 1);

		if (m_isSpriteSheetAnimated && m_spriteSheetCellsPerRow > 0)
		{
			int totalCells = m_spriteSheetCellEndIndex - m_spriteSheetCellIndex + 1;
			if (totalCells > 0)
			{
				int cellIndex = particle.spriteSheetIndex;
				int column = cellIndex % m_spriteSheetCellsPerRow;
				int row = cellIndex / m_spriteSheetCellsPerRow;

				float cellWidth = 1.0f / m_spriteSheetCellsPerRow;
				float cellHeight = cellWidth; // Assume square cells for simplicity

				uvMin = Vector2(column * cellWidth, row * cellHeight);
				uvMax = Vector2((column + 1) * cellWidth, (row + 1) * cellHeight);
			}
		}

		// Render particle
		rc->drawTexturedRect(
			m_sprite,
			worldPos - particle.size * m_particlePivot,
			particle.size,
			color,
			uvMin,
			uvMax,
			particle.rotation
		);
	}
}

void ParticleEmitter::emitParticle()
{
	ParticleData particle;

	Vector2 emitterPos = m_isPositionRelativeToEmitter ? Vector2(0, 0) : getWorldPosition();
	Vector2 emitterCenter = emitterPos + getSize() * 0.5f;

	// Initialize position based on emit shape
	particle.position = getRandomEmitPosition();

	// Initialize velocity
	if (m_movementCoordinateType == CoordinateType::Polar)
	{
		// Polar coordinates
		particle.velocity = Vector2(
			m_particleInitialVelocity.x + randomRange(-m_particleInitialVelocityVariation.x, m_particleInitialVelocityVariation.x),
			m_particleInitialVelocity.y + randomRange(-m_particleInitialVelocityVariation.y, m_particleInitialVelocityVariation.y)
		);
	}
	else
	{
		// Cartesian coordinates - calculate velocity from speed and direction
		particle.velocity = getRandomParticleVelocity(particle.position, emitterCenter);
		particle.initialSpeed = particle.velocity.length();
	}

	// Initialize acceleration
	particle.acceleration = m_particleAcceleration;

	// Initialize rotation
	if (m_isParticleInitialRotationFromInitialVelocity)
		particle.rotation = getRandomParticleRotation(particle.velocity);
	else
		particle.rotation = m_particleInitialRotation + randomRange(-m_particleInitialRotationVariation, m_particleInitialRotationVariation);

	particle.rotationSpeed = m_particleRotationSpeed + randomRange(-m_particleRotationSpeedVariation, m_particleRotationSpeedVariation);

	// Initialize lifetime
	if (m_isParticleLifetimeInfinite)
		particle.maxLifetime = 10000.0f; // Very large number
	else
		particle.maxLifetime = m_particleLifetime + randomRange(-m_particleLifetimeVariation, m_particleLifetimeVariation);

	particle.lifetime = 0.0f;

	// Initialize size
	particle.initialSize = Vector2(
		m_particleSize.x + randomRange(-m_particleSizeVariation.x, m_particleSizeVariation.x),
		m_particleSize.y + randomRange(-m_particleSizeVariation.y, m_particleSizeVariation.y)
	);
	particle.size = particle.initialSize;

	// Initialize color
	particle.color = getRandomParticleColor();

	// Initialize sprite sheet
	if (m_isSpriteSheetAnimated)
	{
		if (m_isSpriteSheetIndexRandom)
		{
			int range = m_spriteSheetCellEndIndex - m_spriteSheetCellIndex + 1;
			particle.spriteSheetIndex = m_spriteSheetCellIndex + (rand() % range);
		}
		else
		{
			particle.spriteSheetIndex = m_spriteSheetCellIndex;
		}
		particle.spriteSheetTime = 0.0f;
	}
	else
	{
		particle.spriteSheetIndex = m_spriteSheetCellIndex;
	}

	m_particles.push_back(particle);
}

void ParticleEmitter::updateParticles(float deltaTime)
{
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

		// Update velocity and position
		particle.velocity += particle.acceleration * deltaTime;
		particle.position += particle.velocity * deltaTime;

		// Update rotation
		if (m_isParticleRotationFromVelocity && particle.velocity.length() > 0.01f)
		{
			particle.rotation = std::atan2(particle.velocity.y, particle.velocity.x);
		}
		else
		{
			particle.rotation += particle.rotationSpeed * deltaTime;
		}

		// Update sprite sheet animation
		if (m_isSpriteSheetAnimated && m_spriteSheetFrameDelay > 0.0f)
		{
			particle.spriteSheetTime += deltaTime;
			if (particle.spriteSheetTime >= m_spriteSheetFrameDelay)
			{
				particle.spriteSheetTime -= m_spriteSheetFrameDelay;

				int totalCells = m_spriteSheetCellEndIndex - m_spriteSheetCellIndex + 1;
				particle.spriteSheetIndex++;

				if (particle.spriteSheetIndex > m_spriteSheetCellEndIndex)
				{
					if (m_isSpriteSheetAnimationLooped)
						particle.spriteSheetIndex = m_spriteSheetCellIndex;
					else
						particle.spriteSheetIndex = m_spriteSheetCellEndIndex;
				}
			}
		}

		++it;
	}
}

Vector2 ParticleEmitter::getRandomEmitPosition()
{
	Vector2 emitterSize = getSize();
	Vector2 emitterCenter = m_isPositionRelativeToEmitter ? emitterSize * 0.5f : getWorldPosition() + emitterSize * 0.5f;

	switch (m_emitShape)
	{
	case EmitShape::Point:
		return emitterCenter;

	case EmitShape::Circle:
		{
			float angle = randomRange(0, TWO_PI);
			float distance;

			if (m_isEmitOnEdge)
				distance = m_outsideDistance;
			else
				distance = randomRange(m_insideDistance, m_outsideDistance);

			return emitterCenter + Vector2(std::cos(angle) * distance, std::sin(angle) * distance);
		}

	case EmitShape::Rectangle:
		{
			Vector2 offset(
				randomRange(-emitterSize.x * 0.5f, emitterSize.x * 0.5f),
				randomRange(-emitterSize.y * 0.5f, emitterSize.y * 0.5f)
			);
			return emitterCenter + offset;
		}

	default:
		return emitterCenter;
	}
}

Vector2 ParticleEmitter::getRandomParticleVelocity(const Vector2& particlePos, const Vector2& emitterPos)
{
	float speed = m_particleSpeed + randomRange(-m_particleSpeedVariation, m_particleSpeedVariation);
	float angle;

	if (m_initialDirectionType == InitialDirectionType::RelativeToEmitAngle)
	{
		angle = (m_emitAngle + randomRange(-m_emitAngleVariation, m_emitAngleVariation)) * 0.0174532925f;
	}
	else // RelativeToEmitterCenter
	{
		Vector2 dir = (particlePos - emitterPos).normalized();
		angle = std::atan2(dir.y, dir.x) + randomRange(-m_emitAngleVariation, m_emitAngleVariation) * 0.0174532925f;
	}

	return Vector2(std::cos(angle) * speed, std::sin(angle) * speed);
}

float ParticleEmitter::getRandomParticleRotation(const Vector2& velocity)
{
	if (velocity.length() > 0.01f)
		return std::atan2(velocity.y, velocity.x);
	return 0.0f;
}

Color4f ParticleEmitter::getRandomParticleColor()
{
	Color4f color = m_particleColor;

	// Apply brightness variation
	if (m_particleColorBrightnessVariation > 0.0f)
	{
		float brightness = 1.0f + randomRange(-m_particleColorBrightnessVariation, m_particleColorBrightnessVariation);
		color.setRed(Scalar(color.getRed() * brightness));
		color.setGreen(Scalar(color.getGreen() * brightness));
		color.setBlue(Scalar(color.getBlue() * brightness));
	}

	// Apply tint variation
	if (m_particleColorTintVariation > 0.0f)
	{
		color.setRed(Scalar(color.getRed() + randomRange(-m_particleColorTintVariation, m_particleColorTintVariation)));
		color.setGreen(Scalar(color.getGreen() + randomRange(-m_particleColorTintVariation, m_particleColorTintVariation)));
		color.setBlue(Scalar(color.getBlue() + randomRange(-m_particleColorTintVariation, m_particleColorTintVariation)));
	}

	// Clamp to valid range
	float r = clamp(float(color.getRed()), 0.0f, 1.0f);
	float g = clamp(float(color.getGreen()), 0.0f, 1.0f);
	float b = clamp(float(color.getBlue()), 0.0f, 1.0f);
	color.setRed(Scalar(r));
	color.setGreen(Scalar(g));
	color.setBlue(Scalar(b));

	color.setAlpha(Scalar(m_particleAlpha));

	return color;
}

float ParticleEmitter::randomRange(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(m_random);
}

}
