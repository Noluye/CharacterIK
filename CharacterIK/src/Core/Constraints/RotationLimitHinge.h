#pragma once
#include "RotationLimit.h"

namespace cik
{
	class RotationLimitHinge : public RotationLimit
	{
	public:
		RotationLimitHinge();
	public:
		float m_AngleMin = -45;
		float m_AngleMax = 90;
	protected:
		virtual m3::Quat LimitRotation(m3::Quat rotation) override;

	private:
		float lastAngle = 0.0f;
	};
}