// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStateManager.generated.h"

UCLASS()
class VEHICLETEMPLATE_API AGameStateManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameStateManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float lastSpawnedPickup;
public:	
	void PickNewCheckpoint();
	void AddPlayerScore(int score);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		TArray<AActor*> checkpoints;
	UPROPERTY(EditAnywhere)
		TArray<AActor*> vehicleSpawnpoints;
	UPROPERTY(EditAnywhere)
		TArray<AActor*> vehicles;
	UPROPERTY(EditAnywhere)
		TArray<AActor*> pickupSpawns;
	UPROPERTY(EditAnywhere)
		int targetNumPickups;
	UPROPERTY(EditAnywhere)
		float pickupSpawnDelay;
	UPROPERTY(EditAnywhere)
		int targetNumVehicles;
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AAIWheeledVehicle> vehicleClass;

	TArray<AActor*> pickups;

	class AAIWheeledVehicle* CreateVehicle(FVector location, FRotator rotation);
	class APickup* CreatePickup(FVector location, FRotator rotation);
private:
	int ActiveCheckpointIndex;
	int LastVehicleSpawnIndex;
};
