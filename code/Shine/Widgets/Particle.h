/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Ref.h"
#include "Core/Containers/AlignedVector.h"
#include "Shine/Visual.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::render
{
	class ITexture;
}

namespace traktor::shine
{

/*!
 * \brief Particle widget for 2D particle effects.
 *
 * Particle creates and animates 2D particles for visual effects like
 * sparks, smoke, magic, explosions, etc.
 *
 * Usage:
 *   Ref<Particle> emitter = new Particle(L"Sparkles");
 *   emitter->setParticleTexture(sparkTexture);
 *   emitter->setEmissionRate(50);  // 50 particles/sec
 *   emitter->setLifetime(2.0f);    // 2 second lifetime
 *   emitter->setVelocity(Vector2(0, -100), Vector2(50, 50)); // Up with variance
 *   emitter->play();
 */
class T_DLLCLASS Particle : public Visual
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Individual particle data
	 */
	struct ParticleData
	{
		Vector2 position;
		Vector2 velocity;
		float rotation = 0.0f;
		float rotationSpeed = 0.0f;
		float lifetime = 0.0f;
		float maxLifetime = 1.0f;
		float size = 1.0f;
		Color4f color = Color4f(1, 1, 1, 1);
	};

	/*!
	 * \brief Constructor
	 * \param name Optional name for debugging
	 */
	explicit Particle(const std::wstring& name = L"Particle");

	/*!
	 * \brief Destructor
	 */
	virtual ~Particle();

	// === Emission Control ===

	/*!
	 * \brief Start emitting particles
	 */
	void play();

	/*!
	 * \brief Stop emitting particles
	 */
	void stop();

	/*!
	 * \brief Pause emission
	 */
	void pause();

	/*!
	 * \brief Resume emission
	 */
	void resume();

	/*!
	 * \brief Clear all particles
	 */
	void clear();

	/*!
	 * \brief Get whether emitter is playing
	 */
	bool isPlaying() const { return m_isPlaying && !m_isPaused; }

	/*!
	 * \brief Get whether emitter is paused
	 */
	bool isPaused() const { return m_isPaused; }

	// === Emission Configuration ===

	/*!
	 * \brief Get emission rate (particles per second)
	 */
	float getEmissionRate() const { return m_emissionRate; }

	/*!
	 * \brief Set emission rate
	 */
	void setEmissionRate(float rate) { m_emissionRate = rate; }

	/*!
	 * \brief Get particle lifetime
	 */
	float getLifetime() const { return m_lifetime; }

	/*!
	 * \brief Set particle lifetime
	 */
	void setLifetime(float lifetime) { m_lifetime = lifetime; }

	/*!
	 * \brief Get lifetime variance
	 */
	float getLifetimeVariance() const { return m_lifetimeVariance; }

	/*!
	 * \brief Set lifetime variance
	 */
	void setLifetimeVariance(float variance) { m_lifetimeVariance = variance; }

	/*!
	 * \brief Get max particles
	 */
	int getMaxParticles() const { return m_maxParticles; }

	/*!
	 * \brief Set max particles
	 */
	void setMaxParticles(int max) { m_maxParticles = max; }

	// === Particle Properties ===

	/*!
	 * \brief Get particle texture
	 */
	render::ITexture* getParticleTexture() const { return m_particleTexture; }

	/*!
	 * \brief Set particle texture
	 */
	void setParticleTexture(render::ITexture* texture) { m_particleTexture = texture; }

	/*!
	 * \brief Get start velocity
	 */
	Vector2 getStartVelocity() const { return m_startVelocity; }

	/*!
	 * \brief Set start velocity
	 */
	void setStartVelocity(const Vector2& velocity) { m_startVelocity = velocity; }

	/*!
	 * \brief Get velocity variance
	 */
	Vector2 getVelocityVariance() const { return m_velocityVariance; }

	/*!
	 * \brief Set velocity variance
	 */
	void setVelocityVariance(const Vector2& variance) { m_velocityVariance = variance; }

	/*!
	 * \brief Get gravity
	 */
	Vector2 getGravity() const { return m_gravity; }

	/*!
	 * \brief Set gravity
	 */
	void setGravity(const Vector2& gravity) { m_gravity = gravity; }

	/*!
	 * \brief Get start size
	 */
	float getStartSize() const { return m_startSize; }

	/*!
	 * \brief Set start size
	 */
	void setStartSize(float size) { m_startSize = size; }

	/*!
	 * \brief Get start color
	 */
	Color4f getStartColor() const { return m_startColor; }

	/*!
	 * \brief Set start color
	 */
	void setStartColor(const Color4f& color) { m_startColor = color; }

	/*!
	 * \brief Get end color
	 */
	Color4f getEndColor() const { return m_endColor; }

	/*!
	 * \brief Set end color
	 */
	void setEndColor(const Color4f& color) { m_endColor = color; }

	// === Burst Emission ===

	/*!
	 * \brief Emit burst of particles
	 */
	void burst(int count);

	// === Update & Render ===

	virtual void update(float deltaTime) override;
	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	AlignedVector<ParticleData> m_particles;
	Ref<render::ITexture> m_particleTexture;

	// Emission properties
	float m_emissionRate = 10.0f;
	float m_lifetime = 1.0f;
	float m_lifetimeVariance = 0.0f;
	int m_maxParticles = 100;

	// Particle properties
	Vector2 m_startVelocity = Vector2(0.0f, -100.0f);
	Vector2 m_velocityVariance = Vector2(50.0f, 50.0f);
	Vector2 m_gravity = Vector2(0.0f, 0.0f);
	float m_startSize = 10.0f;
	Color4f m_startColor = Color4f(1, 1, 1, 1);
	Color4f m_endColor = Color4f(1, 1, 1, 0);

	// Playback state
	bool m_isPlaying = false;
	bool m_isPaused = false;
	float m_emissionTime = 0.0f;

	// Helper methods
	void emitParticle();
	void updateParticles(float deltaTime);
	float randomRange(float min, float max) const;
};

}
