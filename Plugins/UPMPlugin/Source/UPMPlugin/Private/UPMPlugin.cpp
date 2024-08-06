#include "UPMPlugin.h"
#include "UPMCommands.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "UPMWindow.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UPMStyle.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

static const FName UPMTabName("UPM");

#define LOCTEXT_NAMESPACE "FUPMPluginModule"

void FUPMPluginModule::StartupModule()
{
    FUPMStyle::Initialize();
    FUPMStyle::ReloadTextures();

    FUPMCommands::Register();

    PluginCommands = MakeShareable(new FUICommandList);

    PluginCommands->MapAction(
        FUPMCommands::Get().OpenUPMWindow,
        FExecuteAction::CreateRaw(this, &FUPMPluginModule::PluginButtonClicked),
        FCanExecuteAction());

    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FUPMPluginModule::RegisterMenus));

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(UPMTabName, FOnSpawnTab::CreateRaw(this, &FUPMPluginModule::OnSpawnPluginTab))
        .SetDisplayName(LOCTEXT("FtempTabTitle", "temp"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FUPMPluginModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    FUPMStyle::Shutdown();

    FUPMCommands::Unregister();
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(UPMTabName);
}

TSharedRef<SDockTab> FUPMPluginModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SUPMWindow)
        ];
}

void FUPMPluginModule::PluginButtonClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(UPMTabName);
}

void FUPMPluginModule::RegisterMenus()
{
    // Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
    FToolMenuOwnerScoped OwnerScoped(this);
    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
        {
            FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
            Section.AddMenuEntryWithCommandList(FUPMCommands::Get().OpenUPMWindow, PluginCommands);
        }
    }

    {
        UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
        {
            FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
            {
                FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FUPMCommands::Get().OpenUPMWindow));
                Entry.SetCommandList(PluginCommands);
            }
        }
    }
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUPMPluginModule, UPMPlugin)
