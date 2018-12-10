// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleTemplateHud.h"
#include "VehicleTemplatePawn.h"
#include "WheeledVehicle.h"
#include "RenderResource.h"
#include "Shader.h"
#include "Engine/Canvas.h"
#include "WheeledVehicleMovementComponent.h"
#include "Engine/Font.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerState.h"
#include "VehiclePlayerState.h"

#define LOCTEXT_NAMESPACE "VehicleHUD"

AVehicleTemplateHud::AVehicleTemplateHud()
{
	static ConstructorHelpers::FObjectFinder<UFont> Font(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
	HUDFont = Font.Object;
}

void AVehicleTemplateHud::DrawHUD()
{
	Super::DrawHUD();

	UWorld* pWorld = GetWorld();

	TArray<APlayerState*> players = pWorld->GetGameState()->PlayerArray;
	AVehiclePlayerState* pPlayer = (AVehiclePlayerState*)players[0];

	// Calculate ratio from 720p
	const float HUDXRatio = Canvas->SizeX / 1280.f;
	const float HUDYRatio = Canvas->SizeY / 720.f;

	bool bWantHUD = true;
#if HMD_MODULE_INCLUDED
	bWantHUD = !GEngine->IsStereoscopic3D();
#endif // HMD_MODULE_INCLUDED
	// We dont want the onscreen hud when using a HMD device	
	if (bWantHUD == true)
	{
		// Get our vehicle so we can check if we are in car. If we are we don't want onscreen HUD
		AVehicleTemplatePawn* Vehicle = Cast<AVehicleTemplatePawn>(GetOwningPawn());
		if ((Vehicle != nullptr) && (Vehicle->bInCarCameraActive == false))
		{
			FVector2D ScaleVec(HUDYRatio * 1.4f, HUDYRatio * 1.4f);

			// Speed
			FCanvasTextItem SpeedTextItem(FVector2D(HUDXRatio * 805.f, HUDYRatio * 455), Vehicle->SpeedDisplayString, HUDFont, FLinearColor::White);
			SpeedTextItem.Scale = ScaleVec;
			Canvas->DrawItem(SpeedTextItem);

			// Gear
			FCanvasTextItem GearTextItem(FVector2D(HUDXRatio * 805.f, HUDYRatio * 500.f), Vehicle->GearDisplayString, HUDFont, Vehicle->bInReverseGear == false ? Vehicle->GearDisplayColor : Vehicle->GearDisplayReverseColor);
			GearTextItem.Scale = ScaleVec;
			Canvas->DrawItem(GearTextItem);

			FString HealthText = (TEXT("Health: ") + FString::SanitizeFloat(pPlayer->Health));
			
			// Health
			FCanvasTextItem HealthTextItem(FVector2D(HUDXRatio * 100.f, HUDYRatio * 600.f), FText::FromString(HealthText), HUDFont, Vehicle->GearDisplayColor);
			HealthTextItem.Scale = ScaleVec;
			Canvas->DrawItem(HealthTextItem);

			FString ScoreText = TEXT("Score: ") + FString::SanitizeFloat((float)pPlayer->Score);

			// Health
			FCanvasTextItem ScoreTextItem(FVector2D(HUDXRatio * 100.f, HUDYRatio * 650.f), FText::FromString(ScoreText), HUDFont, Vehicle->GearDisplayColor);
			ScoreTextItem.Scale = ScaleVec;
			Canvas->DrawItem(ScoreTextItem);
		}
	}
}


#undef LOCTEXT_NAMESPACE
