// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VehiclePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLETEMPLATE_API AVehiclePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
		AVehiclePlayerState(const FObjectInitializer&);

		float Health;
		int Score;
		int Ammo;
};
