/*!****************************************************************************************
\file       EmitterData.h
\author     Bhatwal, Ruchi
\date       1/30/17
\copyright  All content © 2017-2018 DigiPen (USA) Corporation, all rights reserved.
\par        Project: Field Punk
\brief
This is the interface for the emitterData struct
******************************************************************************************/
#pragma once

#include <vector>
#include "../../../Math/Vector4.h"
#include "../Renderer/Renderer.h"

/*!*************************************************************************************
\par struct: emitterData
\brief   This is all the data required to create the particle emitters. Most of this
  data is parsed.

\par baseClass: true
***************************************************************************************/
struct emitterData
{
  /*!************************************************************************************
  \brief default constructor for the emitter data
  **************************************************************************************/
  emitterData(std::string pEmitterName = "default", vector4 constantAcceleration = vector4(), float pInitialScale = 1.0f, float pFinalScale = 1.0f,
    const vector4& pInitialColor = vector4(0, 0, 0, 1), const vector4 pFinalColor = vector4(0, 0, 0, 1),
    float pRotVel = 0.0f, float pInitialVelocity = 1.0f, unsigned pNumberOfParticles = 100,
    float pTotalLifetime = 0.0f, float pParticleLifetime = 1.5f, int pParticlesPerSecond = 20,
    vector4 pOffset = vector4(0, 0), float pInitialAngle = 0, float pRandomAng = 0.23f, const vector4& pRandomPos = vector4(),
    renderer pRenderer = renderer(), float prandomParticleLifetime = 0.0f, float pWaveOntime = 0.0f, float pWaveOffTime = 0.0f,
    bool pStartOnTrigger = false, bool isInteractable = false, float randomScale = 0, float particleRestitution = 1.0, bool selfInteracting = false) :
    m_emitterName(pEmitterName), m_initialScale(pInitialScale), m_finalScale(pFinalScale), m_initialColor(pInitialColor),
    m_finalColor(pFinalColor), m_rotationalVelocity(pRotVel), m_initialVelocity(pInitialVelocity),
    m_totalLifeTime(pTotalLifetime), m_numberofParticles(pNumberOfParticles), m_totalParticleLifetime(pParticleLifetime),
    m_constantAcceleration(constantAcceleration), m_particlesPerSecond(pParticlesPerSecond), m_offset(pOffset), m_initialAngle(pInitialAngle),
    m_randomAngleRange(pRandomAng), m_randomPositionRange(pRandomPos), m_particleRenderer(pRenderer), m_randomParticleLifetimeRange(prandomParticleLifetime),
    m_waveOnTime(pWaveOntime), m_waveOffTime(pWaveOffTime), m_startOnTrigger(pStartOnTrigger), m_isInteractable(isInteractable), m_randomScaleFactor(randomScale), 
    m_particleRestistution(particleRestitution), m_interactsWithSelf(selfInteracting), m_isRemoved(false)
  {
  }

  std::string m_emitterName; //!< name of emitter (used for identifying multiple emitters)
  
  vector4 m_constantAcceleration; //!< acceleration of each particle in the emitter

  float m_initialScale; //!< initial scale of spawned particle
  float m_finalScale;   //!< final scale of spawned particle
  float m_randomScaleFactor; //!< random factor for the start and end scale

  float m_randomParticleLifetimeRange; //!< small random range for particles to die
  float m_randomAngleRange;      //!< range for the random direction the particle should move in
  float m_randomAngleRangeZ;      //!< randge for random angle about the x-axis
  vector4 m_randomPositionRange; //!< random range for initial spawn position
  vector4 m_offset;              // positional offset from the actual transform

  vector4 m_initialColor; //!< initial spawn color of the particle
  vector4 m_finalColor;   //!< final color of particles before dying

  renderer m_particleRenderer; //!< graphics renderer

  float m_rotationalVelocity; //!< how fast the particle should rotate in place while movig
  float m_initialAngle;       //!< initial angle (direction of movement) of the particle
  float m_initialAngleZ;       //!< initial angle (direction of movement) of the particle
  float m_initialVelocity;    //!< initial speed of the particle (magnetude)

  unsigned m_numberofParticles;  //!< total number of particles that can be used (kind of effects the "density")
  float m_totalLifeTime;         //!< total lifetime of the emitter (0 if infinite)
  float m_totalParticleLifetime; //!< total life time of the particle itself
  int m_particlesPerSecond;      //!< number of particles spawned per second

  float m_waveOnTime;  //!< time for a wave of particles to run
  float m_waveOffTime; //!< time between wave of particles

  bool m_startOnTrigger; //!< determines if the emitter should start after an event occurs in the game and not just on load
  bool m_isInteractable; //!< determines if the particles from this emitter should be able to interact with interactable colliders
  bool m_interactsWithSelf; //!< particles interact with each other
  float m_particleRestistution; //!< "bouncyness" of the particles

  bool m_isRemoved; //!< (mainly editor stuff) bool used by emitter bundle to remove for itself.
};
