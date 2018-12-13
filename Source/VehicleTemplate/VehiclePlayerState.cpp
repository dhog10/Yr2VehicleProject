// Fill out your copyright notice in the Description page of Project Settings.

#include "VehiclePlayerState.h"
// Debug
#include <Runtime/Engine/Classes/Engine/Engine.h>

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)

AVehiclePlayerState::AVehiclePlayerState(const FObjectInitializer& init)
	: Super(init)
{
	Health = 100.f;
	Score = 0;
	Ammo = 60;
}


