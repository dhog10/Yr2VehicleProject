// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIWheeledVehicle.h"
#include "CoreMinimal.h"
#include "VehicleSpawnPoint.generated.h"

UCLASS()
class VEHICLETEMPLATE_API AVehicleSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVehicleSpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnActor();

	UPROPERTY(EditAnywhere)
		TSubclassOf<AAIWheeledVehicle> SpawnClass;
	
};
