// Fill out your copyright notice in the Description page of Project Settings.

#include "BTSteeringService.h"
#include <EngineUtils.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "VehicleTemplatePawn.h"
#include "AIWheeledVehicleController.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Float.h>
#include <Runtime/AIModule/Classes/AIController.h>
#include <Runtime/Engine/Public/CollisionQueryParams.h>
#include <Runtime/Engine/Classes/Engine/EngineTypes.h>
// Debug
#include <Runtime/Engine/Public/DrawDebugHelpers.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

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
	///////////
	// Setup //
	///////////

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
	float AISpeed = pAIActor->GetVelocity().Size();

	//////////////
	// Steering //
	//////////////

	// Calculate basic steering value based on relative location to player
	float forwardDot = FVector::DotProduct(pAIActor->GetActorForwardVector(), relativePositionNormalized);
	float SteeringValue = FVector::DotProduct(pAIActor->GetActorRightVector(), relativePositionNormalized);

	// Recalculate steering value to avoid obstacles
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, pAIActor);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	// Project end vector to position infront and to the right of the vehicle
	float steerDistanceMultiplier = FMath::Max(1.f, AISpeed / 900.f);
	float traceForwardDistance = 1200.f * 1.5f * steerDistanceMultiplier;
	float traceRightDistance = 220.f * 1.5f * steerDistanceMultiplier;
	float steerAvoidanceThreshold = 0.9f;
	
	FVector traceEndPosition = aiPosition;
	traceEndPosition += pAIActor->GetActorForwardVector() * traceForwardDistance;
	traceEndPosition += pAIActor->GetActorRightVector() * traceRightDistance;

	FHitResult RV_Hit(ForceInit);
	world->LineTraceSingleByChannel(
		RV_Hit,        //result
		aiPosition,    //start
		traceEndPosition, //end
		ECC_Pawn, //collision channel
		RV_TraceParams
	);

	DrawDebugPoint(
		world,
		traceEndPosition,
		20,
		FColor(255, 0, 255),
		false,
		0.03
	);

	float NormalDotRight = 0.f;
	bool HitRight = false;

	AActor* pActorHitRight = RV_Hit.GetActor();
	if (pActorHitRight && pActorHitRight != PlayerPawn) {
		// Trace hit something, clamp steering, do not allow right turn
		relativePositionNormalized = (RV_Hit.ImpactPoint - aiPosition).GetSafeNormal(1.f);
		NormalDotRight = FMath::Min(0.f, FVector::DotProduct(pAIActor->GetActorForwardVector(), RV_Hit.Normal) * steerDistanceMultiplier);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("RIGHT ") + FString::SanitizeFloat(NormalDotRight));

		SteeringValue = FMath::Min(-steerAvoidanceThreshold * -NormalDotRight, SteeringValue);
		HitRight = true;
	}

	// Project end vector to position infront and to the left of the vehicle
	traceEndPosition -= pAIActor->GetActorRightVector() * (traceRightDistance * 2.f);

	world->LineTraceSingleByChannel(
		RV_Hit,        //result
		aiPosition,    //start
		traceEndPosition, //end
		ECC_Pawn, //collision channel
		RV_TraceParams
	);

	float NormalDotLeft = 0.f;
	bool HitLeft = false;

	AActor* pActorHitLeft = RV_Hit.GetActor();
	if (pActorHitLeft && pActorHitLeft != PlayerPawn) {
		// Trace hit something, clamp steering, do not allow left turn
		relativePositionNormalized = (RV_Hit.ImpactPoint - aiPosition).GetSafeNormal(1.f);
		NormalDotLeft = FMath::Min(0.f, FVector::DotProduct(pAIActor->GetActorForwardVector(), RV_Hit.Normal) * steerDistanceMultiplier);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("LEFT ") + FString::SanitizeFloat(NormalDotLeft));

		SteeringValue = FMath::Max(steerAvoidanceThreshold * -NormalDotLeft, SteeringValue);
		HitLeft = true;
	}

	// Trace directly ahead for collisions
	traceEndPosition = aiPosition;
	traceEndPosition += pAIActor->GetActorForwardVector() * 1000.f;
	traceEndPosition += pAIActor->GetActorUpVector() * 10.f;

	world->LineTraceSingleByChannel(
		RV_Hit,        //result
		aiPosition,    //start
		traceEndPosition, //end
		ECC_Pawn, //collision channel
		RV_TraceParams
	);

	AActor* pActorHitFront = RV_Hit.GetActor();
	bool HitForward = false;

	if (pActorHitFront && pActorHitFront != PlayerPawn) {
		HitForward = true;
	}

	bool ShouldReverse = forwardDot < -0.3f || HitForward || (HitRight && HitLeft && NormalDotRight < -0.9f && NormalDotLeft < -0.9f);

	if (ShouldReverse){
		SteeringValue = -1.f;
	}
	
	//////////////
	// Throttle //
	//////////////

	float ThrottleValue = FVector::DotProduct(pAIActor->GetActorForwardVector(), relativePositionNormalized);

	ThrottleValue = (float)FMath::Clamp(ThrottleValue, 0.2f, 1.f);

	float distanceToTarget = FVector::Distance(PlayerPosition, aiPosition);

	if (distanceToTarget < 5000.f) {
		// Reduce AI speed if player has stopped
		float PlayerSpeed = PlayerPawn->GetVelocity().Size();
		if (AISpeed > PlayerSpeed * 1.5f) { ThrottleValue = FMath::Max(ThrottleValue * 0.3f, 0.6f); }
	}



	if (ShouldReverse) {
		ThrottleValue = -1.f;
	}

	//////////////////
	// Apply Values //
	//////////////////

	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();

	int steerID = pBlackboard->GetKeyID(TEXT("SteeringValue"));
	pBlackboard->SetValue<UBlackboardKeyType_Float>(steerID, SteeringValue);

	int throttleID = pBlackboard->GetKeyID(TEXT("ThrottleValue"));
	pBlackboard->SetValue<UBlackboardKeyType_Float>(throttleID, ThrottleValue);
}
