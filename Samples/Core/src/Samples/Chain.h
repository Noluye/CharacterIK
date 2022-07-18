#pragma once
#include <vector>
#include "Samples/Joint.h"

template<int N>
class NJointChain
{
public:
	NJointChain()
	{
		for (int i = 0; i < N; ++i)
		{
			m3::Transform t = { m3::Vec3(0, 1.0f, 0), m3::Quat() };
			m_Joints[i] = Joint(t);
			m_Parents[i] = i - 1;
		}
		ForwardKinematics();
	}

	NJointChain(std::vector<m3::Vec3> positions)
	{
		for (int i = 0; i < N; ++i)
		{
			m3::Transform t = { positions[i], m3::Quat() };
			m_Joints[i] = Joint(t);
			m_Parents[i] = i - 1;
		}
		ForwardKinematics();
	}

	inline Joint& operator[](int i) { return m_Joints[i]; }

	void ForwardKinematics()
	{
		for (int i = 0; i < N; ++i)
		{
			int p = m_Parents[i];
			Joint& currJoint = m_Joints[i];

			if (p == -1)
			{
				currJoint.transform = currJoint.localTransform;
				continue;
			}

			Joint& parentJoint = m_Joints[p];
			currJoint.transform = parentJoint.transform * currJoint.localTransform;
		}
	}

	inline void SetLocalRotation(int i, const m3::Quat& q)
	{
		m_Joints[i].localTransform.rotation = q;
		// update current and children' global rotation
		for (int j = i; j < m_JointNum; ++j) {
			int p = m_Parents[j];
			if (p == -1)
			{
				m_Joints[j].transform = m_Joints[j].localTransform;
			}
			else
			{
				m_Joints[j].transform = m_Joints[p].transform * m_Joints[j].localTransform;
			}
		}
	}

	inline m3::Quat GetLocalRotation(int i)
	{
		return m_Joints[i].localTransform.rotation;
	}

	inline m3::Quat GetGlobalRotation(int i)
	{
		return m_Joints[i].transform.rotation;
	}

	inline m3::Quat GetParentGlobalRotation(int i)
	{
		if (m_Parents[i] == -1) return m3::Quat();

		return m_Joints[m_Parents[i]].transform.rotation;
	}

	inline m3::Transform GetLocalTransform(int i)
	{
		return m_Joints[i].localTransform;
	}

	void Draw(int id = -1)
	{
		for (int i = 0; i < N; ++i)
		{
			int p = m_Parents[i];
			if (p != -1) DrawCylinderEx(ToVector3(m_Joints[i].transform.position), ToVector3(m_Joints[p].transform.position), 0.04, 0.04, 10, DARKBLUE);
			
			m_Joints[i].Draw(id == i ? RED : GOLD);
		}
	}
	
	int m_JointNum = N;
private:
	Joint m_Joints[N];
	int m_Parents[N];
};