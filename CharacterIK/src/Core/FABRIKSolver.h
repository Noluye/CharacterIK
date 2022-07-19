#pragma once
#include <Math3D.h>
#include <vector>
#include "Constraints/RotationLimit.h"

namespace cik
{

	class FABRIKSolver
	{
	public:
		FABRIKSolver(float numStemps = 15, float threshold = 1e-6f);


		inline m3::Transform GetLocalTransform(int index) { return m_IKChain[index]; }
		inline void SetLocalTransform(int index, const m3::Transform& t) { m_IKChain[index] = t; };
		m3::Transform GetGlobalTranform(int index);
		bool Solve(const m3::Transform& target, std::vector<RotationLimit*> limits={0});

		inline int GetNumStemps() { return m_NumStemps; };
		inline void SeteNumSteps(int numSteps) { m_NumStemps = numSteps; };
		inline float GetThreshold() { return m_Threshold; };
		inline void SetThreshold(float threshold) { m_Threshold = threshold; };
		inline int Size() { return m_IKChain.size(); };
		void Resize(int newSize);
	private:
		void _IKChainToWorld();
		void _WorldToIKChain();
		void _IterateBackward(const m3::Vec3& goal);
		void _IterateForward(const m3::Vec3& base);
	private:
		std::vector<m3::Transform> m_IKChain;
		int m_NumStemps;
		float m_Threshold;
		std::vector<m3::Vec3> m_WorldChain;
		std::vector<float> m_Lengths;
	};
}