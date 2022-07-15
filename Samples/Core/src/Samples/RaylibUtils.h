#pragma once
#include "SamplesCommon.h"

inline static Vector3 ToVector3(m3::Vec3 v) { return Vector3(v.x, v.y, v.z); }

void DrawAxis(const m3::Vec3& pos, const m3::Quat rot, const float scale = 0.5f);
