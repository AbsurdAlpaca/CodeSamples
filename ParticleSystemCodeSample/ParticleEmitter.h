/*!****************************************************************************************
\file       ParticleEmitter.h
\author     Bhatwal, Ruchi
\date       1/23/18
\copyright  All content © 2017-2018 DigiPen (USA) Corporation, all rights reserved.
\par        Project: Field Punk
\brief
This is the interface for the particle emitter class
******************************************************************************************/
#pragma once
#include <vector>

#include "../Transform.h"
#include "EmitterData.h"
#include "ParticleEmitterBundle.h"


// forward declarations
class colliderCircle;
class colliderPolygon;
class circle;
class polygon;
class collider;
class particle;

/*!*************************************************************************************
\par class: particleEmitter

\brief  class for a single particle emitter. Manages the movement of particles in them
\par baseClass: true
***************************************************************************************/
class particleEmitter
{
public:
  /*!***********************************************************************************
  \brief  constructor for the particle emitter

  \param pEmitterData - information given by client to create the particle emiiter
  *************************************************************************************/
  particleEmitter(const emitterData& pEmitterData);
  
  /*!***********************************************************************************
  \brief  Updates the current lifetime of a particle and sets it to active/inactive 
    accordingly
  \param particle - particle to update
  \param dt - time passed since last frame
  \param pTransform - If the updated particle needs to be reset, this is the transform it
         needs to be reset to
  *************************************************************************************/
  void UpdateParticleActivity(particle& particle, float dt, transform& pTransform);

  /*!***********************************************************************************
  \brief  Updates the the color of the particle according to initial & final values.
          Uses linear interpolation
  
  \param particle - particle to update
  *************************************************************************************/
  void UpdateParticleColors(particle& particle);
 
  /*!***********************************************************************************
  \brief  If the particle has been set to inactive, this resets all its values to a "new" one

  \param particle - particle to reset
  \param pTransform - Position to reset the particle to
  *************************************************************************************/
  void ResetParticle(particle& particle, const transform& pTransform);

  /*!***********************************************************************************
  \brief  Updates the physics of the particle being updated

  \param particle - particle to update
  \param dt - time passed since last frame
  \param accumulatedForce - forces accumulated over the past frame that needs to be 
         taken into account.
  *************************************************************************************/
  void UpdateParticlePhysics(const vector4& accumulatedForce, particle& particle, float dt);

  /*!***********************************************************************************
  \brief  Updates the whole particle emitter
  
  \param pTransform - If the updated particle needs to be reset, this is the transform it
         needs to be reset to
  *************************************************************************************/
  void UpdateParticleEmitter(float dt, transform& pTransform);

  /*!***********************************************************************************
  \brief  Updates the particle emitter's activity according to the wave times

  \param dt - Time elapsed since last frame
  *************************************************************************************/
  void UpdateParticleEmitterWaveTiming(float dt);

  /*!***********************************************************************************
  \brief  Updates the the scale of the particle according to initial & final values.
          Uses linear interpolation

  \param particle - particle to update
  *************************************************************************************/
  void UpdateParticleScale(particle& particle);

  /*!***********************************************************************************
  \brief  Updates a single particle (calls all other particle stuff)

  \param particle - particle to update
  \param dt - time passed since last frame
  \param accumulatedForce - forces accumulated over the past frame that needs to be
         taken into account.
  \param pTransform - If the updated particle needs to be reset, this is the transform it
  needs to be reset to
  *************************************************************************************/
  void UpdateParticle(const vector4& accumulatedForce, particle& particle, float dt, transform& pTransform);

  /*!***********************************************************************************
  \brief  Creates a brand new particle if enough havent been created yet

  \param pPosition - position to create particle at
  *************************************************************************************/
  void CreateParticle(const transform& pTransform);

  /*!***********************************************************************************
  \brief  Adds force to the system of particles that should be taken into account

  \param pForce - force to add to the system
  *************************************************************************************/
  void AddForceToSystem(const vector4& pForce);

  /*!***********************************************************************************
  \brief  Returns reference to the vector of gpu data for each particle

  \return vector m_particleDataForGPUs
  *************************************************************************************/
  std::vector<shaderHandler::gPUData>& GetGPUData();

  /*!***********************************************************************************
  \brief  Returns reference to the vector of particles for each particle

  \return vector m_particles
  *************************************************************************************/
  std::vector<particle>& GetParticles();

  /*!***********************************************************************************
  \brief  Returns reference of the data the emitter is based on

  \return reference to m_emitterData
  *************************************************************************************/
  emitterData& GetEmitterData();

  /*!***********************************************************************************
  \brief  If the particles per second changes, the time between particles spawned is
          changed by this function accordingly.

  *************************************************************************************/
  void ResetTimeBetweenParticles();

  /*!***********************************************************************************
  \brief  used to sort the current active and inactive particles in the vector

  \param pParticle - particle to swap
  *************************************************************************************/
  void SwapWithLastActiveParticle(particle& pParticle);
  
  /*!***********************************************************************************
  \brief  used to sort the current active and inactive particles in the vector

  \param pParticle - particle to swap
  *************************************************************************************/
  void SwapWithFirstInactiveParticle(particle& pParticle);

  /*!***********************************************************************************
  \brief  returns the renderer used for graphics stuff

  \return reference to the renderer (in emitter data)
  *************************************************************************************/
  renderer& GetEmitterRenderer();
  
  /*!***********************************************************************************
  \brief  Gets the number of particles currently active

  \param returns number of particles currently active in the system
  *************************************************************************************/
  unsigned GetLiveParticleCount() const;

  /*!***********************************************************************************
  \brief  Resets the lifetime of the particle emitter
  *************************************************************************************/
  void RestartEmitter();

  /*!***********************************************************************************
  \brief sets the emitter to inactive (won't be updated until told to restart)
  *************************************************************************************/
  void StopEmitter();

  /*!***********************************************************************************
  \brief  If particle emitter interacts with other colliders this function handles that

  \param colliderTransform - transform of collider to check with interactions
  \param interactableCollider - collider to check interactions with
  *************************************************************************************/
  void CheckParticleCollisions(transform& colliderTransform, collider& interactableCollider);
  
  
private:
  /*!***********************************************************************************
  \brief  handles particle collisions with convex polygon colliders

  \param colliderTransform - transform of collider to check with interactions
  \param interactableCollider - collider to check interactions with
  *************************************************************************************/
  void ParticlePolygonCollisions(transform& colliderTransform, colliderPolygon& interactableCollider);

  vector4 m_additionalForce; //!< additional forces added to the system (cleared every frame)
  float m_currentLifeTime;   //!< emitter's current lifetime
  float m_isEmitterActive;   //!< if emitter is currently active or not

	// gpu data holds the particle's final transform and color that need's to be rendered by the shader
  std::vector<shaderHandler::gPUData> m_particleDataForGPUs; //!< vector of all the gpu data for each particle
  std::vector<particle> m_particles; //!< vector of particles in the emitter

  unsigned m_liveParticleCount;  //!< number of particles currently active
  emitterData m_emitterData;     //!< holds all the data for this particle emitter given by client

  float m_timeBetweenParticles; //!< time between each particle should be spawed
  float m_timesSinceLastParticleSpawned; //!< time sice the last particle spawned

  float m_currentWaveTime; //!< used for making a wave / burst of particles
  bool m_isEmitterPaused;  //!< boolean to help with determining if an emitter is in a wave of spawning particles or not.

  particleEmitterBundle *m_parent; //!< parent holding all the particle emitters.
};
