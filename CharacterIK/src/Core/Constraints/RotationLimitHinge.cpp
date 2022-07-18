#include "cikpch.h"
#include "RotationLimitHinge.h"

namespace cik
{
	RotationLimitHinge::RotationLimitHinge():
		RotationLimit()
	{
	}
	m3::Quat RotationLimitHinge::LimitRotation(m3::Quat rotation)
	{
		if (m_AngleMin == 0 && m_AngleMax == 0) return AngleAxis(axis, 0);

		//// Get 1 degree of freedom rotation along axis
		//m3::Quat free1DOF = Limit1DOF(rotation, axis);
		//m3::Quat workingSpace = Inverse(AngleAxis(axis, m3::Deg2Rad(lastAngle)) * LookRotation(axis, SecondaryAxis()));
		//m3::Vec3 d = workingSpace * free1DOF * SecondaryAxis();
		//float deltaAngle = m3::Rad2Deg(m3::Atan2(d.x, d.z));
		//lastAngle = m3::Clamp(lastAngle + deltaAngle, m_AngleMin, m_AngleMax);
		//return AngleAxis(axis, m3::Deg2Rad(lastAngle));
	}

	m3::Quat RotationLimitHinge::LimitRotation(m3::Quat lclRotation, m3::Quat glbRotation, m3::Quat parentGlbRotation)
	{
		if (m_AngleMin == 0 && m_AngleMax == 0) return AngleAxis(axis, 0);

		m3::Vec3 currHinge = glbRotation * axis;
		m3::Vec3 desireHinge = parentGlbRotation * axis;

		return lclRotation * m3::FromTo(currHinge, desireHinge);
	}
}