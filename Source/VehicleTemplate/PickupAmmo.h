// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "PickupAmmo.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLETEMPLATE_API APickupAmmo : public APickup
{
	GENERATED_BODY()
	
public:
	APickupAmmo();
protected:
	virtual void ApplyPickup(class AVehicleTemplatePawn*) override;
	virtual void InitializePickup() override;
	
};
