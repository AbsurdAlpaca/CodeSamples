/*!****************************************************************************************
\file       Particle.h
\author     Bhatwal, Ruchi
\date       1/23/18
\copyright  All content � 2017-2018 DigiPen (USA) Corporation, all rights reserved.
\par        Project: Field Punk
\brief
This is the implementation for the particle class 
******************************************************************************************/
#pragma once
#include "ParticleEmitter.h"

/*!*************************************************************************************
\par class: particle

\brief  class for a single particle. Super lightweight
\par baseClass: true
***************************************************************************************/
class particle
{
public:
  /*!***********************************************************************************
  \brief  constructor for the particle

  \param pGPUData - index of the gpud data the particle relates to in the emitter data's
         vector of gpu data
  *************************************************************************************/
  particle(unsigned int pGPUData);
   
  /*!***********************************************************************************
  \brief  Gets the particle's current lifetime

  \return m_currentLifetime
  *************************************************************************************/
  float& GetCurrentLifetime();

  /*!***********************************************************************************
  \brief  Gets the particle's index of the gpu data

  \return m_particleGPUDataIndex
  *************************************************************************************/
  unsigned int& GetGPUData();

  /*!***********************************************************************************
  \brief  sets the new velocity after update

  \param pVelocity - Velocity to update to
  *************************************************************************************/
  void SetVelocity(const vector4& pVelocity);

  /*!***********************************************************************************
  \brief  Gets the particle's velocity

  \return m_velocity
  *************************************************************************************/
  vector4 GetVelocity() const;
  
  /*!***********************************************************************************
  \brief  sets the new angular velocity after update

  \param pAngVelocity - pAngVelocity to update to
  *************************************************************************************/
  void SetAngularVelocity(float pAngVelocity);
  
  /*!***********************************************************************************
  \brief  Gets the particle's angular velocity

  \return m_angularVelocity
  *************************************************************************************/
  float GetAngularVelocity() const;

  /*!***********************************************************************************
  \brief  returns if particle is currently active

  \return isActive (bool)
  *************************************************************************************/
  bool IsActive() const;
  
  /*!***********************************************************************************
  \brief  Sets if particle should be active or not

  \return isActive - activity of particle (bool)
  *************************************************************************************/
  void SetActive(bool pisActive);

  /*!***********************************************************************************
  \brief  returns the total lifetime of the particle

  \return m_totalLifetime - total lifetime of the particle
  *************************************************************************************/
  float& GetTotalLifetime();

  /*!***********************************************************************************
  \brief  returns the total lifetime of the particle

  \return m_totalLifetime - total lifetime of the particle
  *************************************************************************************/
  float *GetRandomScale();

  /*!***********************************************************************************
  \brief  returns the total lifetime of the particle

  \return m_totalLifetime - total lifetime of the particle
  *************************************************************************************/
  vector4& GetForce();

  vector4& GetOldPosition() { return m_oldPosition;  }

private:
  bool isActive; //!< bool determining if the particle is active or not

    //!< index corresponding to gpu data in emitter's vector of gpu data
  unsigned int m_particleGPUDataIndex; 
  
  float m_totalLifetime;   //!< total lifetime of the particle
  float m_currentLifetime; //!< particle's current lifetime
  float m_angularVelocity; //!< current angular velocity of particle
  float m_randomScaleFactor[2]; //!< random scale factor for initial and final scale
  vector4 m_velocity;      //!< current velocity of the particle
  vector4 m_oldPosition;   //!< old position of the particle
  vector4 m_force;         //!< force of individual particle
};
