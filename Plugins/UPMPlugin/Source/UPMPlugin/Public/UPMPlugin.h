#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "UPMPlugin/UPMPackage.h"

class FToolBarBuilder;
class FMenuBuilder;

class FUPMPluginModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	static TSharedPtr<UUPMPackage> LoadedPackage;

private:
	void RegisterMenus();
	void LoadOrCreatePackageJson();
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	TSharedPtr<class FUICommandList> PluginCommands;
};
