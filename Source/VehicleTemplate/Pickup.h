// Fill out your copyright notice in the Description page of Project Settings.

// This is the base Pickup class, it contains basic functionality for pickups
// Specific pickup types inherit this class

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class VEHICLETEMPLATE_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float scale = 5.f;

	virtual void ApplyPickup(class AVehicleTemplatePawn* pPickupActor);
	virtual void InitializePickup();

	FVector originalPosition;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(AActor* MyOverlappedActor, AActor* OtherActor);
	
	void ApplyModel(TCHAR * modelPath);
	void ApplyMaterial(TCHAR * materialPath);
	TCHAR * currentMaterialPath;

	class USphereComponent* pSphereComponent;
	class UStaticMeshComponent* pMeshComponent;
};
