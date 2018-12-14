// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "PickupHealth.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLETEMPLATE_API APickupHealth : public APickup
{
	GENERATED_BODY()
	
public:
	APickupHealth();
	
	virtual void ApplyPickup(class AVehicleTemplatePawn*) override;
};
