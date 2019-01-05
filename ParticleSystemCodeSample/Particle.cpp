/*!****************************************************************************************
\file       Particle.cpp
\author     Bhatwal, Ruchi
\date       1/23/18
\copyright  All content © 2017-2018 DigiPen (USA) Corporation, all rights reserved.
\par        Project: Field Punk
\brief
This is the implementation for the
******************************************************************************************/

#include "Particle.h"

particle::particle(unsigned int pGPUData) :
  m_particleGPUDataIndex(pGPUData), m_currentLifetime(0.0f), m_angularVelocity(0.0f), isActive(false), m_totalLifetime(0.0f)
{
}

float& particle::GetCurrentLifetime()
{
  return m_currentLifetime;
}

unsigned int& particle::GetGPUData()
{
  return m_particleGPUDataIndex;
}

void particle::SetVelocity(const vector4& pVelocity)
{
  m_velocity.x = pVelocity.x;
  m_velocity.y = pVelocity.y;
}

vector4 particle::GetVelocity() const
{
  return m_velocity;
}

void particle::SetAngularVelocity(float pAngVelocity)
{
  m_angularVelocity = pAngVelocity;
}

float particle::GetAngularVelocity() const
{
  return m_angularVelocity;
}

bool particle::IsActive() const
{
  return isActive;
}

void particle::SetActive(bool pisActive)
{
  isActive = pisActive;
}

float& particle::GetTotalLifetime()
{
  return m_totalLifetime;
}

float* particle::GetRandomScale()
{
  return m_randomScaleFactor;
}

vector4& particle::GetForce()
{
  return m_force;
}
