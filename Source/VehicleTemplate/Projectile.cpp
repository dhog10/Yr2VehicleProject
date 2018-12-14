// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Materials/MaterialInterface.h"
#include "AIWheeledVehicle.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

// Sets default values
AProjectile::AProjectile()
{
	float scale = 0.5f;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Intialize a sphere collider
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	SphereComponent->InitSphereRadius(40.0f * scale);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	// Initialize a sphere mesh for visual representation
	UStaticMeshComponent* SphereVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	SphereVisual->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	if (SphereVisualAsset.Succeeded())
	{
		SphereVisual->SetStaticMesh(SphereVisualAsset.Object);
		SphereVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f * scale));
		SphereVisual->SetWorldScale3D(FVector(0.8f * scale));

		static ConstructorHelpers::FObjectFinder<UMaterialInterface> SphereMaterialAsset(TEXT("/Game/Vehicle/Sedan/Materials/M_Vehicle_Sedan"));
		if (SphereMaterialAsset.Succeeded()) {
			SphereVisual->SetMaterial(0, SphereMaterialAsset.Object);
		}
	}

	SphereComponent->SetSimulatePhysics(true);
	SphereComponent->SetEnableGravity(false);
	SphereComponent->SetNotifyRigidBodyCollision(true);
	SphereComponent->SetCollisionProfileName("Projectile");
	SphereComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	SphereComponent->GetBodyInstance()->bUseCCD = true;

	LastVelocity = FVector(0, 0, 0);

	this->InitialLifeSpan = 10.f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LastVelocity = GetVelocity();
}

// Applies a physics impulse to AI vehicles which this projectile hits
void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) {
	if (!OtherActor) { return; }

	if (OtherActor->GetClass() == AAIWheeledVehicle::StaticClass()) {
		AAIWheeledVehicle* pVehicle = (AAIWheeledVehicle*)OtherActor;

		USkeletalMeshComponent* SM = Cast<USkeletalMeshComponent>(pVehicle->GetRootComponent());

		//If the static mesh is valid apply the given impulse
		if (SM)
		{
			SM->AddImpulse(LastVelocity.GetSafeNormal() * SM->GetBodyInstance()->GetBodyMass() * 2000.f);
			Destroy();
		}
	}

	int a = 0;
}