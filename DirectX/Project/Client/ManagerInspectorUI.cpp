#include "pch.h"
#include "ManagerInspectorUI.h"

#include <Engine/CEventMgr.h>
#include <Engine/CLevelMgr.h>

#include "TreeUI.h"

ManagerInspectorUI::ManagerInspectorUI()
	: UI("##ManagerInspectorUI")
{
	SetName("ManagerInspector");

	m_Tree = new TreeUI;
	m_Tree->SetName("ContentTree");
	m_Tree->SetActive(true);
	m_Tree->ShowRoot(false);

	m_Tree->AddDynamic_Select(this, (UI_DELEGATE_1)&ManagerInspectorUI::ChangeLevel);
	m_Tree->SetDragDropID("Levels");
	AddChildUI(m_Tree);
}

ManagerInspectorUI::~ManagerInspectorUI()
{
}

void ManagerInspectorUI::ChangeLevel(DWORD_PTR _ptr)
{
	TreeNode* pSelectedNode = (TreeNode*)_ptr;
	CLevel* pSelectLevel = (CLevel*)pSelectedNode->GetData();

	if (nullptr == pSelectLevel)
		return;

	ChangeCurLevel(pSelectLevel);
}

void ManagerInspectorUI::ResetLevels()
{
	m_Tree->Clear();
	m_Tree->AddItem("Root", 0);

	auto levels = CLevelMgr::GetInst()->GetLevels();
	TreeNode* pCategory = m_Tree->AddItem("Levels", 0);
	pCategory->SetCategoryNode(true);

	for (auto iter : levels)
	{
		string str(iter.first.begin(), iter.first.end());
		m_Tree->AddItem(str, (DWORD_PTR)iter.second, pCategory);
	}
}

void ManagerInspectorUI::init() { UI::init(); }
void ManagerInspectorUI::tick()
{
	UI::tick();

	if(CEventMgr::GetInst()->IsLevelChanged())
	{
		ResetLevels();
	}
}

int ManagerInspectorUI::render_update()
{
	return TRUE;
}