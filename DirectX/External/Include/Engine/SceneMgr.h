#pragma once

enum class SCENE_TYPE
{
	S_MAIN,
	S_GAME,
	S_END,

	END,
};

class Scene;
class SceneMgr
	: public CSingleton<SceneMgr>
{
private:
	Scene*	m_vecScenes[(UINT)SCENE_TYPE::END];
	Scene*	m_pCurScene;

public:
	void create(SCENE_TYPE _Type, Scene* _Scene);
	void init();
	void release();

public:
	Scene* GetScene(SCENE_TYPE _Type) { return m_vecScenes[(UINT)_Type]; }
	void ChangeScene(SCENE_TYPE _Type);

};

