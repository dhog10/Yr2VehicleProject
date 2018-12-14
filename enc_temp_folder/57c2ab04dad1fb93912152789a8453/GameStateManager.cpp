// Fill out your copyright notice in the Description page of Project Settings.

#include "GameStateManager.h"
#include "Checkpoint.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "VehiclePlayerState.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "AIWheeledVehicle.h"
#include "Pickup.h"
#include "PickupSpawnPoint.h"

// Sets default values
AGameStateManager::AGameStateManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	checkpoints = TArray<AActor*>();
	ActiveCheckpointIndex = 0;
	LastVehicleSpawnIndex = 0;
	targetNumVehicles = 3;
	targetNumPickups = 5;
	lastSpawnedPickup = 0.f;
	pickupSpawnDelay = 10.f;
}

// Called when the game starts or when spawned
void AGameStateManager::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0; i < checkpoints.Num(); i++) {
		ACheckpoint* pCheckpoint = (ACheckpoint*)checkpoints[i];

		pCheckpoint->pManager = this;
	}
}

void AGameStateManager::PickNewCheckpoint()
{
	int oldCheckpoint = ActiveCheckpointIndex;
	int len = checkpoints.Num();

	if (len > 0) {
		for (int i = 0; i < 10; i++) {
			ActiveCheckpointIndex = (int)FMath::RandRange(1, len) - 1;
			if (oldCheckpoint != ActiveCheckpointIndex) { break; }
		}
	}
}

void AGameStateManager::AddPlayerScore(int score)
{
	UWorld* pWorld = GetWorld();
	AVehiclePlayerState* pState = (AVehiclePlayerState*)pWorld->GetGameState()->PlayerArray[0];
	pState->Score += score;
}

// Called every frame
void AGameStateManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	lastSpawnedPickup += DeltaTime;

	if (ActiveCheckpointIndex < checkpoints.Num()) {
		AActor* pCheckpointActor = checkpoints[ActiveCheckpointIndex];

		if (pCheckpointActor) {
			ACheckpoint* pCheckpoint = (ACheckpoint*)pCheckpointActor;
			if (!pCheckpoint->bCheckpointActive) {
				pCheckpoint->ActivateCheckpoint();
			}
		}
	}

	// Remove destroyed / pending destruction actors from list
	for (int i = 0; i < vehicles.Num(); i++) {
		AActor* pActor = vehicles[i];

		if (!pActor || !pActor->IsValidLowLevel() || pActor->IsPendingKill()) {
			vehicles.RemoveAt(i);
		}
	}

	int len = vehicles.Num();
	int spawnLen = vehicleSpawnpoints.Num();

	if (len < targetNumVehicles && spawnLen > 0) {
		if (LastVehicleSpawnIndex >= spawnLen) {
			LastVehicleSpawnIndex = 0;
		}

		AActor* pSpawn = vehicleSpawnpoints[LastVehicleSpawnIndex++];

		if (pSpawn) {
			AAIWheeledVehicle* pVehicle = CreateVehicle(pSpawn->GetActorLocation() + FVector(0, 500.0, 0), pSpawn->GetActorRotation());

			if (pVehicle) {
				vehicles.Add(pVehicle);
			}
		}
	}

	// Remove destroyed / pending destruction actors from pickup list
	for (int i = 0; i < pickups.Num(); i++) {
		AActor* pActor = pickups[i];

		if (!pActor || !pActor->IsValidLowLevel() || pActor->IsPendingKill()) {
			pickups.RemoveAt(i);
		}
	}

	if (lastSpawnedPickup >= pickupSpawnDelay) {
		len = pickups.Num();
		spawnLen = pickupSpawns.Num();

		if (len < targetNumVehicles && spawnLen > 0) {
			int PickupIndex = (int)FMath::RandRange(1, spawnLen) - 1;

			if (PickupIndex >= spawnLen) {
				PickupIndex = 0;
			}

			APickupSpawnPoint* pSpawn = (APickupSpawnPoint*)pickupSpawns[PickupIndex];
			APickup* pPickup = pSpawn->SpawnActor();

			if (pPickup) {
				pickups.Add(pPickup);
			}
		}
		lastSpawnedPickup = 0.f;
	}
}

AAIWheeledVehicle* AGameStateManager::CreateVehicle(FVector location, FRotator rotation) {
	UWorld* pWorld = GetWorld();

	FActorSpawnParameters params;
	params.Owner = this;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AAIWheeledVehicle* SpawnedActor = pWorld->SpawnActor<AAIWheeledVehicle>(AAIWheeledVehicle::StaticClass(), location, rotation, params);

	return SpawnedActor;
}