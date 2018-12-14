// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupHealth.h"
#include "VehicleTemplatePawn.h"
#include "VehiclePlayerState.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

APickupHealth::APickupHealth() {
	// Apply the heart model, and pink material to the static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BulletAsset(TEXT("/Game/Models/Polygonal_heart"));
	if (BulletAsset.Succeeded())
	{
		pMeshComponent->SetStaticMesh(BulletAsset.Object);
		pMeshComponent->SetRelativeLocation(FVector(-20.f, -20.f, 0));
		pMeshComponent->SetRelativeRotation(FRotator(0, 0, 90));
		pMeshComponent->SetWorldScale3D(FVector(0.8f * 0.75f));

		this->ApplyMaterial(currentMaterialPath);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SphereMaterialAsset(TEXT("/Game/Vehicle/Sedan/Materials/Pink"));
	if (SphereMaterialAsset.Succeeded()) {
		pMeshComponent->SetMaterial(0, SphereMaterialAsset.Object);
	}
}

// Increase the players health when the pickup is collected
void APickupHealth::ApplyPickup(AVehicleTemplatePawn* pPickupActor) {
	AVehiclePlayerState* pState = (AVehiclePlayerState*)pPickupActor->PlayerState;

	pState->Health = FMath::Min(100.f, pState->Health + 5.f);
}