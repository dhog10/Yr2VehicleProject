// Fill out your copyright notice in the Description page of Project Settings.

// This is the base Pickup class, it contains basic functionality for pickups
// Specific pickup types inherit this class

#include "Pickup.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "VehicleTemplatePawn.h"


// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = pSphereComponent;
	pSphereComponent->InitSphereRadius(40.0f * scale);
	pSphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	pMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	pMeshComponent->SetupAttachment(RootComponent);

	currentMaterialPath = TEXT("/Game/Vehicle/Sedan/Materials/M_Vehicle_Sedan");
	this->ApplyModel(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));

	pSphereComponent->SetCollisionProfileName("OverlapAll");
	pMeshComponent->SetCollisionProfileName("OverlapAll");

	OnActorBeginOverlap.AddDynamic(this, &APickup::OnHit);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
	originalPosition = GetActorLocation();
	this->InitializePickup();
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float time = UGameplayStatics::GetRealTimeSeconds(GetWorld());

	SetActorLocation(originalPosition + FVector(0, 0, 100.f + FMath::Cos(time) * 100.f));
}

void APickup::OnHit(AActor* MyOverlappedActor, AActor* OtherActor) {
	if (OtherActor->GetClass() != AVehicleTemplatePawn::StaticClass()) { return; }

	ApplyPickup((AVehicleTemplatePawn*)OtherActor);
	Destroy();
}

// Called when the pickup is collected, this is overriden by child pickups
void APickup::ApplyPickup(AVehicleTemplatePawn* pPickupActor) {
	
}

void APickup::InitializePickup() {
	
}

// This method applies the selected model to the static mesh component, given the models path
void APickup::ApplyModel(TCHAR * modelPath) {
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(modelPath);
	if (SphereVisualAsset.Succeeded())
	{
		pMeshComponent->SetStaticMesh(SphereVisualAsset.Object);
		pMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f * scale));
		pMeshComponent->SetWorldScale3D(FVector(0.8f * scale));

		this->ApplyMaterial(currentMaterialPath);
	}
}

void APickup::ApplyMaterial(TCHAR * materialPath) {
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SphereMaterialAsset(materialPath);
	if (SphereMaterialAsset.Succeeded()) {
		pMeshComponent->SetMaterial(0, SphereMaterialAsset.Object);
	}
}