#pragma once
class ShadowMap
{
public:

	ShadowMap(int x, int y);
	~ShadowMap(void);
	void AddShadowMapLight(VECTOR lightPos);
	void AddShadowMapDrawArea(VECTOR minPos, VECTOR maxPos);
	void DrawSetup(void);
	void DrawEnd(void);
	void SetShadow(void);
	void EndShadow(void);
	void Release(void);

private:

	int shadowId_;
};

