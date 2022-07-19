#pragma once
#include "RotationLimit.h"

namespace cik
{
	class RotationLimitHinge : public RotationLimit
	{
	public:
		RotationLimitHinge(float angleMin=-45, float angleMax=90);
	public:
		float m_AngleMin;
		float m_AngleMax;
	protected:
		virtual m3::Quat LimitRotation(m3::Quat rotation) override;

	private:
		float lastAngle = 0.0f;
	};
}