#pragma once

#include "RotationLimit.h"

namespace cik
{
	class RotationLimitAngle : public RotationLimit
	{
	public:
		RotationLimitAngle();
	public:
		float m_SwingLimit = 45;
	protected:
		virtual m3::Quat LimitRotation(m3::Quat rotation) override;
	private:
		float lastAngle = 0.0f;
		const float m_TwistLimit = 180;  // Limit of twist rotation around the main axis.
	private:
		m3::Quat LimitSwing(m3::Quat rotation);
		/*
		 * Applies uniform twist limit to the rotation
		 * */
		static m3::Quat LimitTwist(m3::Quat rotation, m3::Vec3 axis, m3::Vec3 orthoAxis, float twistLimit);
	};
}