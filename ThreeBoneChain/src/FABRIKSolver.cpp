#include "FABRIKSolver.h"

FABRIKSolver::FABRIKSolver(float numStemps, float threshold)
	:m_NumStemps(numStemps), m_Threshold(threshold)
{
}

bool FABRIKSolver::Solve(const m3::Transform& target)
{
	unsigned int size = Size();
	if (size == 0) return false;

	unsigned int last = size - 1;

	_IKChainToWorld();
	m3::Vec3 goal = target.position;
	m3::Vec3 base = m_WorldChain[0];

	for (unsigned int i = 0; i < m_NumStemps; ++i)
	{
		m3::Vec3 effector = m_WorldChain[last];
		if (LenSq(goal - effector) < m_Threshold)
		{
			_WorldToIkChain();
			return true;
		}
		_IterateBackward(goal);
		_IterateForward(base);
	}

	_WorldToIkChain();
	m3::Vec3 effector = GetGlobalTranform(last).position;
	if (LenSq(goal - effector) < m_Threshold) return true;
	return false;
}

m3::Transform FABRIKSolver::GetGlobalTranform(unsigned int index)
{
	unsigned int size = (unsigned int)m_IKChain.size();
	m3::Transform world = m_IKChain[index];
	for (int i = index - 1; i >= 0; --i)
	{
		world = Combine(m_IKChain[i], world);
	}
	
	return world;
}


void FABRIKSolver::Resize(unsigned int newSize)
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

void FABRIKSolver::_WorldToIkChain()
{
	for (unsigned int i = 0; i < m_IKChain.size() - 1; ++i)
	{
		m3::Transform world = GetGlobalTranform(i);
		m3::Transform next = GetGlobalTranform(i + 1);
		m3::Vec3 position = world.position;
		m3::Quat rotation = world.rotation;
		
		m3::Vec3 toNext = Inverse(rotation) * (next.position - position);
		m3::Vec3 toDesired = Inverse(rotation) * (m_WorldChain[i + 1] - position);
		m3::Quat delta = FromTo(toNext, toDesired);
		m_IKChain[i].rotation = delta * m_IKChain[i].rotation;
	}
}

void FABRIKSolver::_IterateBackward(const m3::Vec3& goal)
{
	int size = (int)m_IKChain.size();
	if (size > 0) m_WorldChain[size - 1] = goal;
	for (int i = size - 2; i >= 0; --i)
	{
		m3::Vec3 dir = Normalized(m_WorldChain[i] - m_WorldChain[i + 1]);
		m_WorldChain[i] = m_WorldChain[i + 1] + dir * m_Lengths[i + 1];
	}
}

void FABRIKSolver::_IterateForward(const m3::Vec3& base)
{
	int size = (int)m_IKChain.size();
	if (size > 0) m_WorldChain[0] = base;

	for (int i = 1; i < size; ++i)
	{
		m3::Vec3 dir = Normalized(m_WorldChain[i] - m_WorldChain[i - 1]);
		m_WorldChain[i] = m_WorldChain[i-1] + dir * m_Lengths[i];
	}
}

