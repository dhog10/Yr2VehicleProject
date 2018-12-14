// Fill out your copyright notice in the Description page of Project Settings.

#include "VehicleSpawnPoint.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/World.h"

// Sets default values
AVehicleSpawnPoint::AVehicleSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVehicleSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVehicleSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVehicleSpawnPoint::SpawnActor() {
	UWorld* pWorld = GetWorld();

	FVector position = GetActorLocation() + FVector(0, 50.f, 0);
	FRotator angle = FRotator(0, 0, 0);

	FActorSpawnParameters params;
	params.Owner = this;

	pWorld->SpawnActor<AAIWheeledVehicle>(AAIWheeledVehicle::StaticClass(), position, angle, params);
}

