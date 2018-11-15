// Fill out your copyright notice in the Description page of Project Settings.

#include "BTSteeringService.h"
#include <EngineUtils.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "VehicleTemplatePawn.h"
#include "AIWheeledVehicleController.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Float.h>
#include <Runtime/AIModule/Classes/AIController.h>

void UBTSteeringService::OnGameplayTaskActivated(UGameplayTask & task)
{
	//nothing here
	//This method must be overridden from the inherited interface
	//Otherwise, the linker will fail.
}

void UBTSteeringService::OnGameplayTaskDeactivated(UGameplayTask & task)
{
	//nothing here
	//This method must be overridden from the inherited interface
	//Otherwise, the linker will fail.
}

void UBTSteeringService::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	UWorld* world = OwnerComp.GetWorld();
	
	TActorIterator<AVehicleTemplatePawn> PlayerPawnIter(world);
	//Not particularly robust, but at least here we are dealing with only one player
	PlayerPawn = *PlayerPawnIter;

	AAIController* pAIController = OwnerComp.GetAIOwner();

	AActor* pAIActor = pAIController->GetPawn();

	FVector aiPosition = pAIActor->GetActorLocation();
	FVector PlayerPosition = PlayerPawn->GetActorLocation();

	// Set target location to where the player should be in roughly 1 seconds to intercept
	PlayerPosition += (PlayerPawn->GetVelocity() * 1.0f);

	FVector relativePositionNormalized = (PlayerPosition - aiPosition).GetSafeNormal(1.f);

	float SteeringValue = FVector::DotProduct(pAIActor->GetActorRightVector(), relativePositionNormalized);
	float ThrottleValue = FVector::DotProduct(pAIActor->GetActorForwardVector(), relativePositionNormalized);

	ThrottleValue = (float)FMath::Clamp(ThrottleValue, 0.2f, 1.f);

	// Reduce AI speed if player has stopped
	float AISpeed = pAIActor->GetVelocity().Size();
	float PlayerSpeed = PlayerPawn->GetVelocity().Size();
	if (AISpeed > PlayerSpeed * 1.5f) { ThrottleValue = FMath::Max(ThrottleValue * 0.3f, 0.6f); }

	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();

	int steerID = pBlackboard->GetKeyID(TEXT("SteeringValue"));
	pBlackboard->SetValue<UBlackboardKeyType_Float>(steerID, SteeringValue);

	int throttleID = pBlackboard->GetKeyID(TEXT("ThrottleValue"));
	pBlackboard->SetValue<UBlackboardKeyType_Float>(throttleID, ThrottleValue);
}
