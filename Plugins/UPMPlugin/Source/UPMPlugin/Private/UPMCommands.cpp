#include "UPMCommands.h"

#define LOCTEXT_NAMESPACE "FUPMCommands"

void FUPMCommands::RegisterCommands()
{
    UI_COMMAND(OpenUPMWindow, "Open UPM Window", "Open the UPM Plugin window.", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
