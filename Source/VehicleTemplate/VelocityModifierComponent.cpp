// Fill out your copyright notice in the Description page of Project Settings.

#include "VelocityModifierComponent.h"


// Sets default values for this component's properties
UVelocityModifierComponent::UVelocityModifierComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UVelocityModifierComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UVelocityModifierComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UVelocityModifierComponent::OnOverlapBegin(UPrimitiveComponent * pComponent, AActor * pOtherActor, UPrimitiveComponent * pOtherComponent, int32 otherBodyIndex, bool bFromSweep, const FHitResult hitResult)
{

}

