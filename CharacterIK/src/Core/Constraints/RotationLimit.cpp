#include "cikpch.h"
#include "RotationLimit.h"

namespace cik
{
	RotationLimit::RotationLimit(m3::Vec3 v)
		:axis(v)
	{
	}

	m3::Quat RotationLimit::Apply(m3::Quat localRotation)
	{
		m3::Quat rotation = Inverse(m_DefaultLocalRotation) * localRotation;
		m3::Quat limitedRotation = LimitRotation(rotation);
		return m_DefaultLocalRotation * limitedRotation;
	}
}