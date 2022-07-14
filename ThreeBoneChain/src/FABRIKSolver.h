#pragma once
#include <vector>
#include <Math3D.h>

class FABRIKSolver
{
public:
	FABRIKSolver(float numStemps = 15, float threshold = 1e-5f);

	
	inline m3::Transform GetLocalTransform(unsigned int index) { return m_IKChain[index]; }
	inline void SetLocalTransform(unsigned int index, const m3::Transform& t) { m_IKChain[index] = t; };
	m3::Transform GetGlobalTranform(unsigned int index);
	bool Solve(const m3::Transform& target);

	inline unsigned int GetNumStemps() { return m_NumStemps; };
	inline void SeteNumSteps(unsigned int numSteps) { m_NumStemps = numSteps; };
	inline float GetThreshold() { return m_Threshold; };
	inline void SetThreshold(float threshold) { m_Threshold = threshold; };
	inline unsigned int Size() { return m_IKChain.size(); };
	void Resize(unsigned int newSize);
private:
	void _IKChainToWorld();
	void _WorldToIkChain();
	void _IterateBackward(const m3::Vec3& goal);
	void _IterateForward(const m3::Vec3& base);
private:
	std::vector<m3::Transform> m_IKChain;
	unsigned int m_NumStemps;
	float m_Threshold;
	std::vector<m3::Vec3> m_WorldChain;
	std::vector<float> m_Lengths;
};