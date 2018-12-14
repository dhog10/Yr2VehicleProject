// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupAmmo.h"
#include "VehicleTemplatePawn.h"
#include "VehiclePlayerState.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

APickupAmmo::APickupAmmo() {
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BulletAsset(TEXT("/Game/Models/AR_Bullet_Shell"));
	if (BulletAsset.Succeeded())
	{
		pMeshComponent->SetStaticMesh(BulletAsset.Object);
		pMeshComponent->SetRelativeLocation(FVector(-20.f, -20.f, 0));
		pMeshComponent->SetRelativeRotation(FRotator(0, 0, 90));
		pMeshComponent->SetWorldScale3D(FVector(0.8f * 100.f));

		this->ApplyMaterial(currentMaterialPath);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SphereMaterialAsset(TEXT("/Game/Vehicle/Sedan/Materials/Yellow"));
	if (SphereMaterialAsset.Succeeded()) {
		pMeshComponent->SetMaterial(0, SphereMaterialAsset.Object);
	}
}

void APickupAmmo::ApplyPickup(AVehicleTemplatePawn* pPickupActor) {
	AVehiclePlayerState* pState = (AVehiclePlayerState*)pPickupActor->PlayerState;

	pState->Ammo += 5;
}

void APickupAmmo::InitializePickup() {
	
}