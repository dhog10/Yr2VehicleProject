// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleTemplateGameMode.h"
#include "VehicleTemplatePawn.h"
#include "VehicleTemplateHud.h"

AVehicleTemplateGameMode::AVehicleTemplateGameMode()
{
	DefaultPawnClass = AVehicleTemplatePawn::StaticClass();
	HUDClass = AVehicleTemplateHud::StaticClass();
}
