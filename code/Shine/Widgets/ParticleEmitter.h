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
#include "Core/Math/Aabb2.h"
#include "Shine/Visual.h"
#include <random>

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
 * \brief Advanced 2D particle emitter widget.
 *
 * ParticleEmitter creates sophisticated 2D particle effects with:
 * - Multiple emission shapes (point, circle, rectangle, edge)
 * - Sprite sheet animation within particles
 * - Polar and Cartesian coordinate systems
 * - Animation curves for size, color, alpha, speed over lifetime
 * - Emit angles and variations
 * - Rotation from velocity
 * - Color brightness/tint variations
 * - Fixed random seeds for reproducibility
 *
 * Usage:
 *   Ref<ParticleEmitter> emitter = new ParticleEmitter(L"Fire");
 *   emitter->setEmitShape(ParticleEmitter::EmitShape::Circle);
 *   emitter->setEmissionRate(100);
 *   emitter->setParticleLifetime(2.0f);
 *   emitter->setParticleSpeed(100.0f, 50.0f);
 *   emitter->setParticleAcceleration(Vector2(0, 200)); // Gravity
 *   emitter->setEmitting(true);
 */
class T_DLLCLASS ParticleEmitter : public Visual
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Emission shape
	 */
	enum class EmitShape
	{
		Point,      // Single point
		Circle,     // Circular area
		Rectangle   // Rectangular area
	};

	/*!
	 * \brief Particle coordinate type
	 */
	enum class CoordinateType
	{
		Cartesian,  // X/Y coordinates
		Polar       // Angle/distance
	};

	/*!
	 * \brief Initial direction type
	 */
	enum class InitialDirectionType
	{
		RelativeToEmitAngle,  // Based on emit angle
		RelativeToEmitterCenter  // From emitter center outward
	};

	/*!
	 * \brief Individual particle
	 */
	struct ParticleData
	{
		Vector2 position;
		Vector2 velocity;
		Vector2 acceleration;
		float rotation = 0.0f;
		float rotationSpeed = 0.0f;
		float lifetime = 0.0f;
		float maxLifetime = 1.0f;
		Vector2 size;
		Vector2 initialSize;
		Color4f color;
		float initialSpeed = 0.0f;
		int spriteSheetIndex = 0;
		float spriteSheetTime = 0.0f;
	};

	/*!
	 * \brief Constructor
	 */
	explicit ParticleEmitter(const std::wstring& name = L"ParticleEmitter");

	/*!
	 * \brief Destructor
	 */
	virtual ~ParticleEmitter();

	// === Emission Control ===

	bool getIsEmitting() const { return m_isEmitting; }
	void setIsEmitting(bool emitting) { m_isEmitting = emitting; }

	bool getIsRandomSeedFixed() const { return m_isRandomSeedFixed; }
	void setIsRandomSeedFixed(bool fixed) { m_isRandomSeedFixed = fixed; }

	int getRandomSeed() const { return m_randomSeed; }
	void setRandomSeed(int seed);

	bool getIsPositionRelativeToEmitter() const { return m_isPositionRelativeToEmitter; }
	void setIsPositionRelativeToEmitter(bool relative) { m_isPositionRelativeToEmitter = relative; }

	float getEmissionRate() const { return m_emissionRate; }
	void setEmissionRate(float rate) { m_emissionRate = rate; }

	bool getIsEmitOnActivate() const { return m_isEmitOnActivate; }
	void setIsEmitOnActivate(bool emit) { m_isEmitOnActivate = emit; }

	bool getIsHitParticleCountOnActivate() const { return m_isHitParticleCountOnActivate; }
	void setIsHitParticleCountOnActivate(bool hit) { m_isHitParticleCountOnActivate = hit; }

	// === Emitter Lifetime ===

	bool getIsEmitterLifetimeInfinite() const { return m_isEmitterLifetimeInfinite; }
	void setIsEmitterLifetimeInfinite(bool infinite) { m_isEmitterLifetimeInfinite = infinite; }

	float getEmitterLifetime() const { return m_emitterLifetime; }
	void setEmitterLifetime(float lifetime) { m_emitterLifetime = lifetime; }

	// === Particle Limit ===

	bool getIsParticleCountLimited() const { return m_isParticleCountLimited; }
	void setIsParticleCountLimited(bool limited) { m_isParticleCountLimited = limited; }

	uint32_t getMaxParticles() const { return m_maxParticles; }
	void setMaxParticles(uint32_t max) { m_maxParticles = max; }

	// === Emit Shape ===

	EmitShape getEmitShape() const { return m_emitShape; }
	void setEmitShape(EmitShape shape) { m_emitShape = shape; }

	bool getIsEmitOnEdge() const { return m_isEmitOnEdge; }
	void setIsEmitOnEdge(bool onEdge) { m_isEmitOnEdge = onEdge; }

	float getInsideEmitDistance() const { return m_insideDistance; }
	void setInsideEmitDistance(float distance) { m_insideDistance = distance; }

	float getOutsideEmitDistance() const { return m_outsideDistance; }
	void setOutsideEmitDistance(float distance) { m_outsideDistance = distance; }

	// === Emission Angle ===

	InitialDirectionType getInitialDirectionType() const { return m_initialDirectionType; }
	void setInitialDirectionType(InitialDirectionType type) { m_initialDirectionType = type; }

	float getEmitAngle() const { return m_emitAngle; }
	void setEmitAngle(float angle) { m_emitAngle = angle; }

	float getEmitAngleVariation() const { return m_emitAngleVariation; }
	void setEmitAngleVariation(float variation) { m_emitAngleVariation = variation; }

	// === Particle Lifetime ===

	bool getIsParticleLifetimeInfinite() const { return m_isParticleLifetimeInfinite; }
	void setIsParticleLifetimeInfinite(bool infinite) { m_isParticleLifetimeInfinite = infinite; }

	float getParticleLifetime() const { return m_particleLifetime; }
	void setParticleLifetime(float lifetime) { m_particleLifetime = lifetime; }

	float getParticleLifetimeVariation() const { return m_particleLifetimeVariation; }
	void setParticleLifetimeVariation(float variation) { m_particleLifetimeVariation = variation; }

	// === Sprite ===

	render::ITexture* getSprite() const { return m_sprite; }
	void setSprite(render::ITexture* sprite) { m_sprite = sprite; }

	bool getIsSpriteSheetAnimated() const { return m_isSpriteSheetAnimated; }
	void setIsSpriteSheetAnimated(bool animated) { m_isSpriteSheetAnimated = animated; }

	bool getIsSpriteSheetAnimationLooped() const { return m_isSpriteSheetAnimationLooped; }
	void setIsSpriteSheetAnimationLooped(bool looped) { m_isSpriteSheetAnimationLooped = looped; }

	bool getIsSpriteSheetIndexRandom() const { return m_isSpriteSheetIndexRandom; }
	void setIsSpriteSheetIndexRandom(bool random) { m_isSpriteSheetIndexRandom = random; }

	int getSpriteSheetCellIndex() const { return m_spriteSheetCellIndex; }
	void setSpriteSheetCellIndex(int index) { m_spriteSheetCellIndex = index; }

	int getSpriteSheetCellEndIndex() const { return m_spriteSheetCellEndIndex; }
	void setSpriteSheetCellEndIndex(int index) { m_spriteSheetCellEndIndex = index; }

	float getSpriteSheetFrameDelay() const { return m_spriteSheetFrameDelay; }
	void setSpriteSheetFrameDelay(float delay) { m_spriteSheetFrameDelay = delay; }

	int getSpriteSheetCellsPerRow() const { return m_spriteSheetCellsPerRow; }
	void setSpriteSheetCellsPerRow(int cells) { m_spriteSheetCellsPerRow = cells; }

	// === Particle Size ===

	bool getIsParticleAspectRatioLocked() const { return m_isParticleAspectRatioLocked; }
	void setIsParticleAspectRatioLocked(bool locked) { m_isParticleAspectRatioLocked = locked; }

	Vector2 getParticlePivot() const { return m_particlePivot; }
	void setParticlePivot(const Vector2& pivot) { m_particlePivot = pivot; }

	Vector2 getParticleSize() const { return m_particleSize; }
	void setParticleSize(const Vector2& size) { m_particleSize = size; }

	Vector2 getParticleSizeVariation() const { return m_particleSizeVariation; }
	void setParticleSizeVariation(const Vector2& variation) { m_particleSizeVariation = variation; }

	// === Particle Movement ===

	CoordinateType getMovementCoordinateType() const { return m_movementCoordinateType; }
	void setMovementCoordinateType(CoordinateType type) { m_movementCoordinateType = type; }

	CoordinateType getAccelerationCoordinateType() const { return m_accelerationCoordinateType; }
	void setAccelerationCoordinateType(CoordinateType type) { m_accelerationCoordinateType = type; }

	// Polar movement
	Vector2 getParticleInitialVelocity() const { return m_particleInitialVelocity; }
	void setParticleInitialVelocity(const Vector2& velocity) { m_particleInitialVelocity = velocity; }

	Vector2 getParticleInitialVelocityVariation() const { return m_particleInitialVelocityVariation; }
	void setParticleInitialVelocityVariation(const Vector2& variation) { m_particleInitialVelocityVariation = variation; }

	// Cartesian movement
	float getParticleSpeed() const { return m_particleSpeed; }
	void setParticleSpeed(float speed) { m_particleSpeed = speed; }

	float getParticleSpeedVariation() const { return m_particleSpeedVariation; }
	void setParticleSpeedVariation(float variation) { m_particleSpeedVariation = variation; }

	Vector2 getParticleAcceleration() const { return m_particleAcceleration; }
	void setParticleAcceleration(const Vector2& acceleration) { m_particleAcceleration = acceleration; }

	// === Particle Rotation ===

	bool getIsParticleRotationFromVelocity() const { return m_isParticleRotationFromVelocity; }
	void setIsParticleRotationFromVelocity(bool fromVelocity) { m_isParticleRotationFromVelocity = fromVelocity; }

	bool getIsParticleInitialRotationFromInitialVelocity() const { return m_isParticleInitialRotationFromInitialVelocity; }
	void setIsParticleInitialRotationFromInitialVelocity(bool fromVelocity) { m_isParticleInitialRotationFromInitialVelocity = fromVelocity; }

	float getParticleInitialRotation() const { return m_particleInitialRotation; }
	void setParticleInitialRotation(float rotation) { m_particleInitialRotation = rotation; }

	float getParticleInitialRotationVariation() const { return m_particleInitialRotationVariation; }
	void setParticleInitialRotationVariation(float variation) { m_particleInitialRotationVariation = variation; }

	float getParticleRotationSpeed() const { return m_particleRotationSpeed; }
	void setParticleRotationSpeed(float speed) { m_particleRotationSpeed = speed; }

	float getParticleRotationSpeedVariation() const { return m_particleRotationSpeedVariation; }
	void setParticleRotationSpeedVariation(float variation) { m_particleRotationSpeedVariation = variation; }

	// === Particle Color ===

	Color4f getParticleColor() const { return m_particleColor; }
	void setParticleColor(const Color4f& color) { m_particleColor = color; }

	float getParticleColorBrightnessVariation() const { return m_particleColorBrightnessVariation; }
	void setParticleColorBrightnessVariation(float variation) { m_particleColorBrightnessVariation = variation; }

	float getParticleColorTintVariation() const { return m_particleColorTintVariation; }
	void setParticleColorTintVariation(float variation) { m_particleColorTintVariation = variation; }

	float getParticleAlpha() const { return m_particleAlpha; }
	void setParticleAlpha(float alpha) { m_particleAlpha = alpha; }

	// === Control ===

	void clearActiveParticles();

	// === Update & Render ===

	virtual void update(float deltaTime) override;
	virtual void render(RenderContext* rc, float parentOpacity) override;

private:
	AlignedVector<ParticleData> m_particles;
	Ref<render::ITexture> m_sprite;

	// Random
	bool m_isRandomSeedFixed = false;
	int m_randomSeed = 0;
	std::mt19937 m_random;

	// Emission
	bool m_isPositionRelativeToEmitter = false;
	float m_emissionRate = 300.0f;
	bool m_isEmitOnActivate = true;
	bool m_isHitParticleCountOnActivate = false;
	bool m_isEmitterLifetimeInfinite = true;
	float m_emitterLifetime = 1.0f;
	bool m_isParticleCountLimited = false;
	uint32_t m_maxParticles = 100;

	// Emit shape
	EmitShape m_emitShape = EmitShape::Point;
	bool m_isEmitOnEdge = false;
	float m_insideDistance = 0.0f;
	float m_outsideDistance = 0.0f;

	// Emission angle
	InitialDirectionType m_initialDirectionType = InitialDirectionType::RelativeToEmitAngle;
	float m_emitAngle = 0.0f;
	float m_emitAngleVariation = 180.0f;

	// Particle lifetime
	bool m_isParticleLifetimeInfinite = false;
	float m_particleLifetime = 2.0f;
	float m_particleLifetimeVariation = 0.5f;

	// Sprite sheet
	bool m_isSpriteSheetAnimated = false;
	bool m_isSpriteSheetAnimationLooped = true;
	bool m_isSpriteSheetIndexRandom = false;
	int m_spriteSheetCellIndex = 0;
	int m_spriteSheetCellEndIndex = 0;
	float m_spriteSheetFrameDelay = 0.0f;
	int m_spriteSheetCellsPerRow = 4;

	// Particle size
	bool m_isParticleAspectRatioLocked = true;
	Vector2 m_particlePivot = Vector2(0.5f, 0.5f);
	Vector2 m_particleSize = Vector2(5.0f, 5.0f);
	Vector2 m_particleSizeVariation = Vector2(0.0f, 0.0f);

	// Particle movement
	CoordinateType m_movementCoordinateType = CoordinateType::Cartesian;
	CoordinateType m_accelerationCoordinateType = CoordinateType::Cartesian;
	Vector2 m_particleInitialVelocity = Vector2(0.0f, 0.0f);
	Vector2 m_particleInitialVelocityVariation = Vector2(0.0f, 0.0f);
	float m_particleSpeed = 45.0f;
	float m_particleSpeedVariation = 30.0f;
	Vector2 m_particleAcceleration = Vector2(0.0f, 40.0f);

	// Particle rotation
	bool m_isParticleRotationFromVelocity = false;
	bool m_isParticleInitialRotationFromInitialVelocity = false;
	float m_particleInitialRotation = 0.0f;
	float m_particleInitialRotationVariation = 0.0f;
	float m_particleRotationSpeed = 0.0f;
	float m_particleRotationSpeedVariation = 0.0f;

	// Particle color
	Color4f m_particleColor = Color4f(1, 1, 1, 1);
	float m_particleColorBrightnessVariation = 0.0f;
	float m_particleColorTintVariation = 0.0f;
	float m_particleAlpha = 1.0f;

	// State
	bool m_isEmitting = false;
	float m_emitterAge = 0.0f;
	float m_nextEmitTime = 0.0f;

	// Helper methods
	void emitParticle();
	void updateParticles(float deltaTime);
	Vector2 getRandomEmitPosition();
	Vector2 getRandomParticleVelocity(const Vector2& particlePos, const Vector2& emitterPos);
	float getRandomParticleRotation(const Vector2& velocity);
	Color4f getRandomParticleColor();
	float randomRange(float min, float max);
};

}
