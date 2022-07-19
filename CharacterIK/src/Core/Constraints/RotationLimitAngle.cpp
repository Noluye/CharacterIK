#include "cikpch.h"
#include "RotationLimitAngle.h"

namespace cik
{

	RotationLimitAngle::RotationLimitAngle() :
		RotationLimit()
	{
	}

	m3::Quat RotationLimitAngle::LimitRotation(m3::Quat rotation)
	{
		// Subtracting off-limits swing
		m3::Quat swing = LimitSwing(rotation);
		// Apply twist limits
		return LimitTwist(swing, axis, SecondaryAxis(), m_TwistLimit);
	}

	m3::Quat RotationLimitAngle::LimitSwing(m3::Quat rotation)
	{
		if (axis == m3::Vec3(0.0) || rotation == m3::Quat() || m_SwingLimit >= 180) return rotation;
		
		m3::Vec3 swingAxis = rotation * axis;
		// Get the limited swing axis
		m3::Quat swingRotation = m3::FromTo(axis, swingAxis);
		m3::Quat limitedSwingRotation = RotateTowards(m3::Quat(), swingRotation, m_SwingLimit);

		// Rotation from current(illegal) swing rotation to the limited(legal) swing rotation
		m3::Quat toLimits = FromTo(swingAxis, limitedSwingRotation * axis);

		// Subtract the illegal rotation
		return toLimits * rotation;
	}

	m3::Quat RotationLimitAngle::LimitTwist(m3::Quat rotation, m3::Vec3 axis, m3::Vec3 orthoAxis, float twistLimit)
	{
		twistLimit = m3::Clamp(twistLimit, 0, 180);
		if (twistLimit >= 180) return rotation;

		m3::Vec3 normal = rotation * axis;
		m3::Vec3 orthoTangent = orthoAxis;
		m3::OrthoNormalize(normal, orthoTangent);

		m3::Vec3 rotatedOrthoTangent = rotation * orthoAxis;
		m3::OrthoNormalize(normal, rotatedOrthoTangent);

		m3::Quat fixedRotation = m3::FromTo(rotatedOrthoTangent, orthoTangent) * rotation;

		if (twistLimit <= 0) return fixedRotation;

		// Rotate from zero twist to free twist by the limited angle
		return m3::RotateTowards(fixedRotation, rotation, twistLimit);
	}
}