#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FUPMCommands : public TCommands<FUPMCommands>
{
public:
	FUPMCommands()
		: TCommands<FUPMCommands>(
			TEXT("UPM"),
			NSLOCTEXT("Contexts", "UPM", "UPM Plugin"),
			NAME_None,
			FAppStyle::GetAppStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

	TSharedPtr< FUICommandInfo > OpenUPMWindow;
};
