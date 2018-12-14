// Fill out your copyright notice in the Description page of Project Settings.

#include "Checkpoint.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "GameStateManager.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Core/Public/Math/Color.h"
#include "VehicleTemplatePawn.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComponent->SetupAttachment(RootComponent);

	OnActorBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlapBegin);

	bCheckpointActive = false;
	bDeactivating = false;
	DeactivatingAlpha = 1.f;
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	if (MeshComponent) {
		MILight = UMaterialInstanceDynamic::Create(MeshComponent->GetMaterial(0), this);
		MeshComponent->SetMaterial(0, MILight);
	}

	DeactivateCheckpoint();
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bDeactivating) {
		DeactivatingAlpha -= DeltaTime;
		if (DeactivatingAlpha < 0.f) {
			DeactivatingAlpha = 1.f;
			bDeactivating = false;
		}

		SetMaterialAlpha(DeactivatingAlpha);
	}

	if (!bCheckpointActive && !bDeactivating) {
		SetActorHiddenInGame(true);
	}
}

void ACheckpoint::ActivateCheckpoint()
{
	bCheckpointActive = true;
	SetActorHiddenInGame(false);

	SetMaterialColor(InactiveColor);
	SetMaterialAlpha(1.f);
}

void ACheckpoint::DeactivateCheckpoint()
{
	bCheckpointActive = false;
	bDeactivating = true;
	DeactivatingAlpha = 1.f;
}

void ACheckpoint::SetMaterialColor(FLinearColor color)
{
	if (MILight) {
		MILight->SetVectorParameterValue(TEXT("WireframeColor"), color);
	}
}

void ACheckpoint::SetMaterialAlpha(float alpha)
{
	if (MILight) {
		MILight->SetScalarParameterValue(TEXT("WireframeAlpha"), FMath::Clamp(alpha, 0.f, 1.f));
	}
}

void ACheckpoint::OnOverlapBegin(AActor * MyOverlappedActor, AActor * OtherActor)
{
	if (OtherActor->GetClass() != AVehicleTemplatePawn::StaticClass()) {
		return;
	}

	if (bCheckpointActive && pManager) {
		pManager->PickNewCheckpoint();
		pManager->AddPlayerScore(1);
		DeactivateCheckpoint();
	}

	SetMaterialColor(ActiveColor);
	SetMaterialAlpha(1.f);
}

