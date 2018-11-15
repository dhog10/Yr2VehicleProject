// Fill out your copyright notice in the Description page of Project Settings.

#include "BTSteeringService.h"
#include <EngineUtils.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "VehicleTemplatePawn.h"
#include "AIWheeledVehicleController.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Float.h>

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

	AActor* pAIActor = OwnerComp.GetOwner();

	FVector aiPosition = pAIActor->GetActorLocation();
	FVector PlayerPosition = PlayerPawn->GetActorLocation();

	FVector relativePositionNormalized = (PlayerPosition - aiPosition).GetSafeNormal(1.f);

	float SteeringValue = FVector::DotProduct(pAIActor->GetActorRightVector(), relativePositionNormalized);
	float ThrottleValue = FVector::DotProduct(pAIActor->GetActorRightVector(), relativePositionNormalized);

	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();

	int steerID = pBlackboard->GetKeyID(TEXT("SteeringValue"));
	pBlackboard->SetValue<UBlackboardKeyType_Float>(steerID, SteeringValue);

	int throttleID = pBlackboard->GetKeyID(TEXT("ThrottleValue"));
	pBlackboard->SetValue<UBlackboardKeyType_Float>(throttleID, ThrottleValue);
}
