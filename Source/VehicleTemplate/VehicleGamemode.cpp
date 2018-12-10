// Fill out your copyright notice in the Description page of Project Settings.

#include "VehicleGamemode.h"
#include "VehicleTemplatePawn.h"
#include "VehicleTemplateHud.h"
#include "VehiclePlayerState.h"

AVehicleGamemode::AVehicleGamemode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultPawnClass = AVehicleTemplatePawn::StaticClass();
	HUDClass = AVehicleTemplateHud::StaticClass();
	PlayerStateClass = AVehiclePlayerState::StaticClass();
}

void AVehicleGamemode::StartPlay() {
	Super::StartPlay();

	
}

