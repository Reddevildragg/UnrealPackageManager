#pragma once

#include "CoreMinimal.h"
#include "UPMWindow.h"
#include "Modules/ModuleManager.h"
#include "UPMPlugin/UPMPackageJson.h"

class FToolBarBuilder;
class FMenuBuilder;

class FUPMPluginModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenus();
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	TSharedPtr<class FUICommandList> PluginCommands;
};
