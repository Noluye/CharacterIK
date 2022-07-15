#include "RaylibUtils.h"

void DrawAxis(const m3::Vec3& pos, const m3::Quat rot, const float scale)
{
	m3::Vec3 axis0 = pos + rot * (m3::Vec3(1.0f, 0.0f, 0.0f) * scale);
	m3::Vec3 axis1 = pos + rot * (m3::Vec3(0.0f, 1.0f, 0.0f) * scale);
	m3::Vec3 axis2 = pos + rot * (m3::Vec3(0.0f, 0.0f, 1.0f) * scale);

	DrawLine3D(ToVector3(pos), ToVector3(axis0), RED);
	DrawLine3D(ToVector3(pos), ToVector3(axis1), GREEN);
	DrawLine3D(ToVector3(pos), ToVector3(axis2), BLUE);
}
