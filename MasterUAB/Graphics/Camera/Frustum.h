#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "Math\Matrix44.h"
#include "Math\Vector3.h"

class CRenderManager;

class CFrustum 
{
private:
	float m_Proj[16];
	float m_Modl[16];
	float m_Clip[16];
	float m_Frustum[6][4];

public:
	void Update(const Mat44f &ViewProj);
	bool SphereVisible(const Vect3f &Center, float Radius) const;
	bool BoxVisible(const Vect3f &Max, const Vect3f &Min) const;
#ifdef _DEBUG
	bool Render(CRenderManager *RenderManager);
#endif
};

#endif