#pragma once
#include <Math3D.h>

namespace cik
{
	class RotationLimit
	{
	public:
		RotationLimit(m3::Vec3 v = m3::Vec3::Forward());
		// Map the zero rotation point to the specified rotation.
		inline void SetDefaultLocalRotation(m3::Quat localRotation)
		{
			m_DefaultLocalRotation = localRotation;
		}
		m3::Quat Apply(m3::Quat localRotation);
		inline m3::Vec3 SecondaryAxis() { return m3::Vec3(axis.y, -axis.z, axis.x); }
	public:
		m3::Vec3 axis;
	protected:
		virtual m3::Quat LimitRotation(m3::Quat rotation) = 0;
		// Limits rotation to a single degree of freedom (along axis)
		static inline m3::Quat Limit1DOF(m3::Quat rotation, m3::Vec3 axis)
		{
			return FromTo(rotation * axis, axis) * rotation;
		}
	private:
		m3::Quat m_DefaultLocalRotation = {};
	};
}
