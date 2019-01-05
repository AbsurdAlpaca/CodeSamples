/*!****************************************************************************************
\file       ParticleEmitter.cpp
\author     Bhatwal, Ruchi
\date       1/23/18
\copyright  All content © 2017-2018 DigiPen (USA) Corporation, all rights reserved.
\par        Project: Field Punk
\brief
This is the implementation for the particle emitter class
******************************************************************************************/

#include "ParticleEmitter.h"
#include "Particle.h"
#include <numeric>
#include "../../../Tools/ScalarTools.h"
#include "../Physics/Collider.h"
#include "../../Systems/SystemManager.h"
#include "../../Messaging/MessageDrawLine.h"
#include "../../Messaging/MessageDrawPoint.h"
#include "../Physics/ColliderPolygon.h"
#include "../Physics/ColliderCircle.h"
#include "../../Systems/Collision.h"
#include "../Physics/Physics.h"
#include "../Physics/RigidBody.h"


particleEmitter::particleEmitter(const emitterData& pEmitterData) : m_emitterData(pEmitterData), m_liveParticleCount(0), m_currentLifeTime(0.0f), m_isEmitterActive(true), m_currentWaveTime(0)
{
  m_timeBetweenParticles = 1.0f / (float)m_emitterData.m_particlesPerSecond;
  m_timesSinceLastParticleSpawned = 0.0f;

  if (pEmitterData.m_startOnTrigger)
    m_isEmitterActive = false;
}

void particleEmitter::UpdateParticleEmitter(float dt, transform& pTransform)
{
    // additional forces are added during a frame
  vector4 totalForce = m_additionalForce + m_emitterData.m_constantAcceleration;
  m_timesSinceLastParticleSpawned += dt;

  if (m_particles.size() < m_emitterData.m_numberofParticles)
  {
      // create particle if not enough (default set to inactive)
    CreateParticle(pTransform.pos());
  }

    // updating each particle
  for (auto& currentParticle : m_particles)
  {
    UpdateParticle(totalForce, currentParticle, dt, pTransform);
  }
  m_currentLifeTime += dt; // increment the lifetime of the entire particle emitter

  if (m_currentLifeTime > m_emitterData.m_totalLifeTime)
  {
      // time to kill a particle emitter
    m_currentLifeTime = 0;

      // if total lifetime is 0, it means that it is immortal
    if (m_emitterData.m_totalLifeTime)
      m_isEmitterActive = false;
  }

  UpdateParticleEmitterWaveTiming(dt);

    // clears the forces accumulated over the last frame.
  m_additionalForce.Clear();
}

void particleEmitter::UpdateParticleEmitterWaveTiming(float dt)
{
	// nothing to update if emitter is inactive
  if (!m_isEmitterActive)
    return;

  // updating the particle emitter activity according to the waves data
  m_currentWaveTime += dt;

    // if particle emitter is currently unpaused, update it with the wave on time
  if (!m_isEmitterPaused)
  {
    if (m_currentWaveTime >= m_emitterData.m_waveOnTime)
    {
        // wave duration is complete, pausing the emitter
      m_isEmitterPaused = true;
      m_currentWaveTime = 0;
    }
  }
  else // else update it with the wave off time
  {
    if (m_currentWaveTime >= m_emitterData.m_waveOffTime)
    {
        // pause in between waves are complete, unpausing the emitter
      m_isEmitterPaused = false;
      m_currentWaveTime = 0;
    }
  }
}

void particleEmitter::UpdateParticle(const vector4& accumulatedForce, particle& particle, float dt, transform& pTransform)
{
    // update activity
  UpdateParticleActivity(particle, dt, pTransform);

    // update particle data if its active
  if (particle.IsActive())
  {
      // update particle colors
    UpdateParticleColors(particle);

      // update particle scale
    UpdateParticleScale(particle);

      // update physics
    UpdateParticlePhysics(accumulatedForce, particle, dt);
  }
}

  // handles whether a particle should be active or inactive
void particleEmitter::UpdateParticleActivity(particle& particle, float dt, transform& pTransform)
{
    // increment lifetime only if it's active
  if (particle.IsActive())
    particle.GetCurrentLifetime() += dt;

  if ((m_timesSinceLastParticleSpawned > m_timeBetweenParticles) && m_isEmitterActive && !m_isEmitterPaused)
  {
    if (m_liveParticleCount < m_emitterData.m_numberofParticles)
    {
        // only respawn if there are too less particles active atm and if there has been
        // enough time between the last particle spawning
      if (!particle.IsActive())
      {
          // ressurecting the dead ass particle because there arent enough active
        ResetParticle(particle, pTransform);
        particle.SetActive(true);
        SwapWithFirstInactiveParticle(particle);
        ++m_liveParticleCount;
        m_timesSinceLastParticleSpawned -= m_timeBetweenParticles;
      }
    }
  }

    // setting a particle to inactive(aka particle murder) if it's lived a whole full life
  if (particle.GetCurrentLifetime() > particle.GetTotalLifetime())
  {
    particle.GetCurrentLifetime() = 0;
    particle.SetActive(false);
    SwapWithLastActiveParticle(particle);
    --m_liveParticleCount;
  }
}

void particleEmitter::UpdateParticleScale(particle& particle)
{
    // t is 0 when lifetime of particle is 0
    // t is 1 when life time of particle has maxed
  float particleCurrLifetime = particle.GetCurrentLifetime();
  float t = particleCurrLifetime / particle.GetTotalLifetime();

  vector4 newScale = m_particleDataForGPUs[particle.GetGPUData()].m_particleTransform.Scl();
  newScale.x = particle.GetRandomScale()[0] + (particle.GetRandomScale()[1] - particle.GetRandomScale()[0]) * t;
  newScale.y = particle.GetRandomScale()[0] + (particle.GetRandomScale()[1] - particle.GetRandomScale()[0]) * t;

  m_particleDataForGPUs[particle.GetGPUData()].m_particleTransform.Scl(newScale);
}

void particleEmitter::UpdateParticleColors(particle& particle)
{
  // t is 0 when lifetime of particle is 0
  // t is 1 when life time of particle has maxed
  float particleCurrLifetime = particle.GetCurrentLifetime();
  float t = particleCurrLifetime / particle.GetTotalLifetime();

  vector4 newColor = m_particleDataForGPUs[particle.GetGPUData()].m_particleColor;
  newColor.x = m_emitterData.m_initialColor.x + (m_emitterData.m_finalColor.x - m_emitterData.m_initialColor.x) * t; // red
  newColor.y = m_emitterData.m_initialColor.y + (m_emitterData.m_finalColor.y - m_emitterData.m_initialColor.y) * t; // green
  newColor.z = m_emitterData.m_initialColor.z + (m_emitterData.m_finalColor.z - m_emitterData.m_initialColor.z) * t; // blue
  newColor.w = m_emitterData.m_initialColor.w + (m_emitterData.m_finalColor.w - m_emitterData.m_initialColor.w) * t; // alpha

  m_particleDataForGPUs[particle.GetGPUData()].m_particleColor = newColor;
}

void particleEmitter::ResetParticle(particle& particle, const transform& pTransform)
{
    // setting a random Position
  float xRandomPosOffset = 0.0f;
  float yRandomPosOffset = 0.0f;
  float zRandomPosOffset = 0.0f;

  if (m_emitterData.m_randomPositionRange.x)
    xRandomPosOffset = ScalarTools::RandomRange(-m_emitterData.m_randomPositionRange.x, m_emitterData.m_randomPositionRange.x);
  if (m_emitterData.m_randomPositionRange.y)
    yRandomPosOffset = ScalarTools::RandomRange(-m_emitterData.m_randomPositionRange.y, m_emitterData.m_randomPositionRange.y);
  if (m_emitterData.m_randomPositionRange.z)
	  zRandomPosOffset = ScalarTools::RandomRange(-m_emitterData.m_randomPositionRange.z, m_emitterData.m_randomPositionRange.z);

    // setting position
  vector4 position = vector4(xRandomPosOffset, yRandomPosOffset, zRandomPosOffset) + pTransform.pos() + m_emitterData.m_offset;
  m_particleDataForGPUs[particle.GetGPUData()].m_particleTransform.pos(position);

    // setting rotation
  m_particleDataForGPUs[particle.GetGPUData()].m_particleTransform.Rot(pTransform.Rot());

    // setting initial color
  m_particleDataForGPUs[particle.GetGPUData()].m_particleColor = m_emitterData.m_initialColor;

    // setting random angle
  float RandomAngle = 0.0f;

  if (m_emitterData.m_randomAngleRange)
    RandomAngle = ScalarTools::RandomRange(-m_emitterData.m_randomAngleRange, m_emitterData.m_randomAngleRange);

    // setting random initial and final scale
  particle.GetRandomScale()[0] = m_emitterData.m_initialScale + ScalarTools::RandomRange(-m_emitterData.m_randomScaleFactor, m_emitterData.m_randomScaleFactor);
  particle.GetRandomScale()[1] = m_emitterData.m_finalScale + ScalarTools::RandomRange(-m_emitterData.m_randomScaleFactor, m_emitterData.m_randomScaleFactor);
  m_particleDataForGPUs[particle.GetGPUData()].m_particleTransform.Scl(particle.GetRandomScale()[0]);

    // setting random lifetime
  float randomLifetime = 0.0f;
  if (m_emitterData.m_randomParticleLifetimeRange)
    randomLifetime = ScalarTools::RandomRange(0.0f, m_emitterData.m_randomParticleLifetimeRange);
  particle.GetTotalLifetime() = m_emitterData.m_totalParticleLifetime + randomLifetime;

  vector4 initialVelocity;
  initialVelocity.x = cosf(m_emitterData.m_initialAngle + RandomAngle) * m_emitterData.m_initialVelocity;
  initialVelocity.y = sinf(m_emitterData.m_initialAngle + RandomAngle) * m_emitterData.m_initialVelocity;
  particle.SetVelocity(initialVelocity);

    // setting the initial angular velocity
  particle.SetAngularVelocity(m_emitterData.m_rotationalVelocity);
}

  // using simple euler intergration to calculate new positions and velocities
void particleEmitter::UpdateParticlePhysics(const vector4& accumulatedForce, particle& particle, float dt)
{
    // incrementing velocity
  vector4 newVelocity = particle.GetVelocity();
  newVelocity += accumulatedForce * (dt / 2.0f) + particle.GetForce();
  float newAngularVelocity = particle.GetAngularVelocity();

    // setting the new incremented velocity
  particle.SetVelocity(newVelocity);
  particle.SetAngularVelocity(newAngularVelocity);

    // incrementing position with respect to the new incremented velocity
  vector4 newPosition = m_particleDataForGPUs[particle.GetGPUData()].m_particleTransform.pos();
  newPosition += particle.GetVelocity() * dt;

  float newRotation = m_particleDataForGPUs[particle.GetGPUData()].m_particleTransform.Rot();;
  newRotation += particle.GetAngularVelocity() * dt;

    // setting old transform
  particle.GetOldPosition() = m_particleDataForGPUs[particle.GetGPUData()].m_particleTransform.pos();

    // setting the new position and rotation values
  m_particleDataForGPUs[particle.GetGPUData()].m_particleTransform.pos(newPosition);
  m_particleDataForGPUs[particle.GetGPUData()].m_particleTransform.Rot(newRotation);

  if (!particle.GetAngularVelocity())
    m_particleDataForGPUs[particle.GetGPUData()].m_particleTransform.Rot(atan2f(newVelocity.y, newVelocity.x));

  particle.GetForce().Clear();
}

void particleEmitter::CreateParticle(const transform& pTransform)
{
  shaderHandler::gPUData data; // new data for particle
  m_particleDataForGPUs.push_back(data); // adding the data to the vector of gpu particle data
  particle newParticle(static_cast<unsigned>(m_particleDataForGPUs.size() - 1)); // constructing the particle
  m_particles.push_back(newParticle); // adding new particle to the vector of particles

    // reset particle sets all the position and velocity data with respect to the random values and stuff
  ResetParticle(m_particles[m_particles.size() - 1], pTransform);
}

void particleEmitter::AddForceToSystem(const vector4& pForce)
{
  m_additionalForce += pForce; // works like force-impulse system
}

std::vector<shaderHandler::gPUData>& particleEmitter::GetGPUData()
{
  return m_particleDataForGPUs;
}

std::vector<particle>& particleEmitter::GetParticles()
{
  return m_particles;
}

emitterData& particleEmitter::GetEmitterData()
{
  return m_emitterData;
}

void particleEmitter::ResetTimeBetweenParticles()
{
  m_timeBetweenParticles = 1.0f / (float)m_emitterData.m_particlesPerSecond;
}

void particleEmitter::SwapWithLastActiveParticle(particle& pParticle)
{
  particle& lastActive = m_particles[m_liveParticleCount - 1];

    // swap gpu data itself
  std::swap(m_particleDataForGPUs[pParticle.GetGPUData()], m_particleDataForGPUs[lastActive.GetGPUData()]);

    // swap gpu data index in the particle
  std::swap(pParticle.GetGPUData(), lastActive.GetGPUData());

    // swap particles
  std::swap(pParticle, m_particles[m_liveParticleCount - 1]);
}

void particleEmitter::SwapWithFirstInactiveParticle(particle& pParticle)
{
  particle& firstInactive = m_particles[m_liveParticleCount];

    // swap GPU data itself
  std::swap(m_particleDataForGPUs[pParticle.GetGPUData()], m_particleDataForGPUs[firstInactive.GetGPUData()]);

    // swap gpu data index in the particle
  std::swap(pParticle.GetGPUData(), firstInactive.GetGPUData());

    // swap particles
  std::swap(pParticle, m_particles[m_liveParticleCount]);
}

renderer& particleEmitter::GetEmitterRenderer()
{
  return m_emitterData.m_particleRenderer;
}

unsigned particleEmitter::GetLiveParticleCount() const
{
  return m_liveParticleCount;
}

void particleEmitter::RestartEmitter()
{
  m_currentLifeTime = 0.0f;
  m_currentWaveTime = 0.0f;

  m_timesSinceLastParticleSpawned = 0.0f;

  m_isEmitterPaused = false;
  m_isEmitterActive = true;
}

void particleEmitter::StopEmitter()
{
  m_isEmitterActive = false;
}

void particleEmitter::CheckParticleCollisions(transform& colliderTransform, collider& interactableCollider)
{
  if (interactableCollider.GetType() == collider::ct_polygon)
    ParticlePolygonCollisions(colliderTransform, dynamic_cast<colliderPolygon&>(interactableCollider));
}

void particleEmitter::ParticlePolygonCollisions(transform& colliderTransform, colliderPolygon& interactableCollider)
{
  polygon& interactablePoly = dynamic_cast<polygon&>(interactableCollider.GetColliderShape());

  const std::vector<vector4>& vertices = interactablePoly.GetVertexList(); //list
  const std::vector<vector4>& normals = interactablePoly.GetNormalList();

  for (auto& currentParticle : m_particles)
  {
    if (!currentParticle.IsActive())
      break;

    vector4 currentPosition = m_particleDataForGPUs[currentParticle.GetGPUData()].m_particleTransform.pos();
    int intersectingIndex = INT_MAX;

    bool isInsideCollider = true;
    for (int i = 0; i < vertices.size(); ++i)
    {
        // getting the face in world space
      vector4 currentFace[2];
      currentFace[0] = colliderTransform.GetLinearTransformation() * vertices[i] + colliderTransform.pos();
      currentFace[1] = colliderTransform.GetLinearTransformation() * vertices[(i + 1) % vertices.size()] + colliderTransform.pos();

        // transformed normal of the face
      vector4 normalFace = colliderTransform.GetLinearTransformation() * normals[i];

      float d = normalFace * currentFace[0]; // calculating "d" for the half place
      vector4 halfPlane = vector4(normalFace.x, normalFace.y, -d);
      vector4 homoParticlePos = vector4(currentPosition.x, currentPosition.y, 1);

      float dot = homoParticlePos.dotProduct3x3(halfPlane);

      if (dot > 0)
      {
          // outside the half plane
        isInsideCollider = false;
        break;
      }

        // checking if particle intersected with current line
      vector4 directionVector = currentParticle.GetOldPosition() - currentPosition;
      directionVector.Normalize();
      directionVector *= 0.2f;

      vector4 IntersectingPoint = CollisionDetect::LineIntersection(currentFace[0], currentFace[1], currentPosition, currentPosition + directionVector);
      if (IntersectingPoint != currentPosition + directionVector)
      {
        intersectingIndex = i;
      }
    }

      // we know we have to resolve the particle collision
    if (isInsideCollider)
    {
        // particle is too far inside the collider, just don't worry about it
      if (intersectingIndex >= vertices.size())
        continue;

      vector4 relativeVelocity = interactableCollider.GetPhysicsComponent()->GetRigidBodyComponent()->GetVelocity() - currentParticle.GetVelocity();

      vector4 normal = colliderTransform.GetLinearTransformation() * normals[intersectingIndex];
      normal.Normalize();
      normal *= 1.2f;
      float particleRestitution = m_emitterData.m_particleRestistution + ScalarTools::RandomRange(-0.2f, 0.2f);
      
      float ImpulseScalar = normal * relativeVelocity;
      vector4 Impulse = normal * ImpulseScalar;

      vector4 newVelocity = Impulse + currentParticle.GetVelocity();

      float restitution = particleRestitution;

        // capping the values
      if (restitution < 0)
        restitution = 0;
      if (restitution > 1.0f)
        restitution = 1.0f;

        // setting the reflected velocity
      currentParticle.SetVelocity(newVelocity * restitution);
    }
  }
}