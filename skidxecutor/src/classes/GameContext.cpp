#include "GameContext.hpp"
#include "../sdk/sdk.hpp"

void CGameContext::Setup()
{
	m_rescanThread = std::thread(&CGameContext::_RescanLoop, this);
}

void CGameContext::_Setup()
{
	auto fake_datamodel = memory->read<unsigned long long>(memory->get_module_address() + offsets::FakeDataModel::Pointer);
	auto real_datamodel = memory->read<unsigned long long>(fake_datamodel + offsets::FakeDataModel::RealDataModel);

	this->DataModel = reinterpret_cast<RBX::DataModel*>(real_datamodel);

	this->VisualEngine = reinterpret_cast<RBX::VisualEngine*>(memory->read<unsigned long long>(memory->get_module_address() + offsets::VisualEngine::Pointer));

	this->Players = (RBX::Players*)DataModel->FindFirstChildOfClass("Players");
	this->Workspace = DataModel->Workspace();
	this->Mouse = (RBX::MouseService*)DataModel->FindFirstChildOfClass("MouseService");
	this->isInGame = (DataModel->Name() == "Ugc");
	this->CurrentCamera = Workspace->FindFirstChildOfClass("Camera")->As<RBX::Camera>();
}

void CGameContext::Shutdown()
{
	m_rescanThread.join();
}

void CGameContext::_RescanLoop()
{
	while (true)
	{
		this->_Setup();
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}
