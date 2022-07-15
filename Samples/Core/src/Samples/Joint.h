#pragma once
#include <Math3D.h>
#include "RaylibUtils.h"

struct Joint
{
	m3::Transform transform;
	m3::Transform localTransform;

	Joint()
		:transform(), localTransform()
	{}
	Joint(const m3::Transform& t)
		:localTransform(t)
	{}

	void Draw()
	{
		DrawSphereWires(ToVector3(transform.position), 0.1, 10, 10, GOLD);
		DrawAxis(transform.position, transform.rotation);
	}
};
