#include "cikpch.h"
#include "FABRIKSolver.h"

namespace cik
{

	FABRIKSolver::FABRIKSolver(float numStemps, float threshold)
		:m_NumStemps(numStemps), m_Threshold(threshold)
	{
	}

	bool FABRIKSolver::Solve(const m3::Transform& target, std::vector<RotationLimit*> limits)
	{
		if (Size() <= 0) return false;
		_IKChainToWorld();
		m3::Vec3 goal = target.position;
		m3::Vec3 base = m_WorldChain[0];

		int last = Size() - 1;
		for (int i = 0; i < m_NumStemps; ++i)
		{
			m3::Vec3 effector = m_WorldChain[last];
			if (LenSq(goal - effector) < m_Threshold)
			{
				_WorldToIKChain();
				return true;
			}
			_IterateBackward(goal);
			_IterateForward(base);
			for (int j = 0; j < Size(); ++j)
			{
				if (j < limits.size() && limits[j])
				{
					_WorldToIKChain();
					m_IKChain[j].rotation = limits[j]->Apply(m_IKChain[j].rotation);
					_IKChainToWorld();
				}
			}
			
		}

		_WorldToIKChain();
		m3::Vec3 effector = GetGlobalTranform(last).position;
		if (LenSq(goal - effector) < m_Threshold) return true;
		return false;
	}

	m3::Transform FABRIKSolver::GetGlobalTranform(int index)
	{
		m3::Transform world = m_IKChain[index];
		for (int i = index - 1; i >= 0; --i) world = m_IKChain[i] * world;  // i means parent
		return world;
	}


	void FABRIKSolver::Resize(int newSize)
	{
		m_IKChain.resize(newSize);
		m_WorldChain.resize(newSize);
		m_Lengths.resize(newSize);
	}

	// Copy the IK chain into the world transform vector and record the segment lengths.
	// The segment length means the distance of a joint from its parent(the root joint has 0 length).
	void FABRIKSolver::_IKChainToWorld()
	{
		for (unsigned int i = 0; i < m_IKChain.size(); ++i)
		{
			m3::Transform world = GetGlobalTranform(i);
			m_WorldChain[i] = world.position;

			if (i >= 1) m_Lengths[i] = Len(world.position - m_WorldChain[i - 1]);
		}

		if (m_IKChain.size() > 0) m_Lengths[0] = 0.0f;
	}

	void FABRIKSolver::_WorldToIKChain()
	{
		for (int i = 0; i < Size() - 1; ++i)
		{
			m3::Transform currWorld = GetGlobalTranform(i);
			m3::Transform childWorld = GetGlobalTranform(i + 1);

			m3::Vec3 toChild = Inverse(currWorld.rotation) * (childWorld.position - currWorld.position);
			m3::Vec3 toDesired = Inverse(currWorld.rotation) * (m_WorldChain[i + 1] - currWorld.position);
			m3::Quat delta = FromTo(toChild, toDesired);
			m_IKChain[i].rotation = m_IKChain[i].rotation * delta;
		}
	}

	void FABRIKSolver::_IterateBackward(const m3::Vec3& goal)
	{
		m_WorldChain[Size() - 1] = goal;
		for (int i = Size() - 2; i >= 0; --i)
		{
			m3::Vec3 dir = Normalized(m_WorldChain[i] - m_WorldChain[i + 1]);
			m_WorldChain[i] = m_WorldChain[i + 1] + dir * m_Lengths[i + 1];
		}
	}

	void FABRIKSolver::_IterateForward(const m3::Vec3& base)
	{
		m_WorldChain[0] = base;
		for (int i = 1; i < Size(); ++i)
		{
			m3::Vec3 dir = Normalized(m_WorldChain[i] - m_WorldChain[i - 1]);
			m_WorldChain[i] = m_WorldChain[i - 1] + dir * m_Lengths[i];
		}
	}
}