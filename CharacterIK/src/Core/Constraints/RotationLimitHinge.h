#pragma once
#include "RotationLimit.h"

namespace cik
{
	class RotationLimitHinge : public RotationLimit
	{
	public:
		RotationLimitHinge();
		m3::Quat LimitRotation(m3::Quat lclRotation, m3::Quat glbRotation, m3::Quat parentGlbRotation);
	public:
		float m_AngleMin = -45;
		float m_AngleMax = 90;
	protected:
		virtual m3::Quat LimitRotation(m3::Quat rotation) override;

	private:
		float lastAngle = 0.0f;
	};
}