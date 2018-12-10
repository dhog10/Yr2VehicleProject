// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "VehicleGamemode.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLETEMPLATE_API AVehicleGamemode : public AGameMode
{
	GENERATED_BODY()

		AVehicleGamemode(const FObjectInitializer& ObjectInitializer);
		virtual void StartPlay() override;
};
