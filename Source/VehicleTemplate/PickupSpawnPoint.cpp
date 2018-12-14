// Fill out your copyright notice in the Description page of Project Settings.

// Used to spawn new pickup actors

#include "PickupSpawnPoint.h"
#include "Runtime/Engine/Classes/Engine/World.h"


// Sets default values
APickupSpawnPoint::APickupSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APickupSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Spawns a random pickup at this actors location
APickup* APickupSpawnPoint::SpawnActor() {
	UWorld* pWorld = GetWorld();

	FVector position = GetActorLocation() + FVector(0, 50.f, 0);
	FRotator angle = FRotator(0, 0, 0);

	FActorSpawnParameters params;
	params.Owner = this;

	int len = SpawnClasses.Num();
	if (len <= 0) { return NULL; }

	int index = (int)FMath::RandRange(1, len) - 1;
	if (index > len) { index = len; }

	TSubclassOf<APickup> spawnClass = SpawnClasses[index];

	APickup* pSpawned = pWorld->SpawnActor<APickup>(spawnClass, position, angle, params);

	return pSpawned;
}

