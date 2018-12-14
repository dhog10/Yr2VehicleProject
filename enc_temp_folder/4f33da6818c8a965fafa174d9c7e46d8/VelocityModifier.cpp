// Fill out your copyright notice in the Description page of Project Settings.

#include "VelocityModifier.h"
#include "DrawDebugHelpers.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "WheeledVehicleMovementComponent4W.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)

// Sets default values
AVelocityModifier::AVelocityModifier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OnActorBeginOverlap.AddDynamic(this, &AVelocityModifier::OnOverlapBegin);
	
	VelocityIncrease = 1000.f;
}

// Called when the game starts or when spawned
void AVelocityModifier::BeginPlay()
{
	Super::BeginPlay();
	
	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Purple, true, -1, 0, 5);
}

// Called every frame
void AVelocityModifier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVelocityModifier::OnOverlapBegin(AActor* MyOverlappedActor, AActor* OtherActor)
{
	print(TEXT("Collide"));

	USkeletalMeshComponent* SM = Cast<USkeletalMeshComponent>(OtherActor->GetRootComponent());

	//If the static mesh is valid apply the given impulse
	if (SM)
	{
		print(TEXT("Impulse"));

		FVector Forward = OtherActor->GetActorForwardVector();
		SM->AddImpulse(Forward * SM->GetBodyInstance()->GetBodyMass() * VelocityIncrease);

	}
	else print(TEXT("Not a mesh"));
}

