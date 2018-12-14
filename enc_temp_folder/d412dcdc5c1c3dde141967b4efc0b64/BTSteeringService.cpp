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
#include "AIWheeledVehicleController.h"

// Debug
#include <Runtime/Engine/Public/DrawDebugHelpers.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)

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

	AAIWheeledVehicleController* pAIController = (AAIWheeledVehicleController*)OwnerComp.GetAIOwner();

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
	float steerDistanceMultiplier = FMath::Max(0.2f, AISpeed / 900.f);
	float traceForwardDistance = 1200.f * 1.5f * steerDistanceMultiplier * 0.7f;
	float traceRightDistance = FMath::Max(260.f, 220.f * 1.5f * steerDistanceMultiplier * FMath::Clamp(600.f / AISpeed, 0.5f, 3.f));
	float traceDirectRightDistance = 250.f;
	float steerAvoidanceThreshold = 0.7f;
	float ForwardTraceOffset = 240.f;
	float upTraceOffset = 70.f;
	int NumSidewaysChecks = 5;
	
	FVector traceEndPosition = aiPosition;

	FHitResult RV_Hit(ForceInit);

	AActor* pActorHitRight = NULL;
	float NormalDotRight = 0.f;
	bool HitRight = false;
	
	// Trace Direct Right
	bool HitDirectRight = false;
	AActor* pActorHitDirectRight = NULL;

	for (int i = 0; i <= 1; i++) {
		traceEndPosition = aiPosition;
		traceEndPosition += pAIActor->GetActorRightVector() * traceDirectRightDistance;
		traceEndPosition += pAIActor->GetActorUpVector() * upTraceOffset;
		traceEndPosition += pAIActor->GetActorForwardVector() * 150.f * (float)i;

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

		pActorHitDirectRight = RV_Hit.GetActor();
		if (pActorHitDirectRight && pActorHitDirectRight != PlayerPawn) {
			HitDirectRight = true;
			break;
		}
	}

	// Trace Direct Left
	bool HitDirectLeft = false;
	AActor* pActorHitDirectLeft = NULL;

	for (int i = 0; i <= 1; i++) {
		traceEndPosition = aiPosition;
		traceEndPosition -= pAIActor->GetActorRightVector() * traceDirectRightDistance;
		traceEndPosition += pAIActor->GetActorUpVector() * upTraceOffset;
		traceEndPosition += pAIActor->GetActorForwardVector() * 50.f * (float)i;

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

		pActorHitDirectLeft = RV_Hit.GetActor();
		if (pActorHitDirectLeft && pActorHitDirectLeft != PlayerPawn) {
			HitDirectLeft = true;
			break;
		}
	}	

	float SteerHitRight = -1.f;
	float SteerClampRight = 0.f;
	float SteerHitLeft = -1.f;
	float SteerClampLeft = 0.f;

	for (float i = 0; i < NumSidewaysChecks; i++) {
		traceEndPosition = aiPosition + pAIActor->GetActorForwardVector() * ForwardTraceOffset;
		traceEndPosition += pAIActor->GetActorForwardVector() * traceForwardDistance * (1.f / (i + 1.f));
		traceEndPosition += pAIActor->GetActorUpVector() * upTraceOffset;
		traceEndPosition += pAIActor->GetActorRightVector() * traceRightDistance * (1.5f / (i + 1.f));

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

		AActor* pActorHitRight = RV_Hit.GetActor();
		if (pActorHitRight && pActorHitRight != PlayerPawn) {

			HitRight = true;

			relativePositionNormalized = (RV_Hit.ImpactPoint - aiPosition).GetSafeNormal(1.f);
			NormalDotRight = FMath::Min(0.f, FVector::DotProduct(pAIActor->GetActorForwardVector(), RV_Hit.Normal) * steerDistanceMultiplier);

			NormalDotRight = -NormalDotRight;

			if (i > 0.f && NormalDotRight > 0.f) {
				float diff = 1.f - NormalDotRight;

				NormalDotRight += diff / (i + 1.f);
			}

			SteerHitRight = i;
			SteerClampRight = -steerAvoidanceThreshold * NormalDotRight;
		}
	}

	float NormalDotLeft = 0.f;
	bool HitLeft = false;

	AActor* pActorHitLeft = NULL;

	// Project end vector to position infront and to the left of the vehicle
	for (float i = 0; i < NumSidewaysChecks; i++) {
		traceEndPosition = aiPosition + pAIActor->GetActorForwardVector() * ForwardTraceOffset;
		traceEndPosition += pAIActor->GetActorForwardVector() * traceForwardDistance * (1.f / (i + 1.f));
		traceEndPosition += pAIActor->GetActorUpVector() * upTraceOffset;
		traceEndPosition -= pAIActor->GetActorRightVector() * traceRightDistance * (1.5f / (i + 1.f));

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

		AActor* pActorHitLeft = RV_Hit.GetActor();
		if (pActorHitLeft && pActorHitLeft != PlayerPawn) {
			HitLeft = true;

			relativePositionNormalized = (RV_Hit.ImpactPoint - aiPosition).GetSafeNormal(1.f);
			NormalDotLeft = FMath::Min(0.f, FVector::DotProduct(pAIActor->GetActorForwardVector(), RV_Hit.Normal) * steerDistanceMultiplier);

			NormalDotLeft = -NormalDotLeft;

			if (i > 0.f && NormalDotLeft > 0.f) {
				float diff = 1.f - NormalDotLeft;

				NormalDotLeft -= diff / (i + 1.f);
			}

			SteerHitLeft = i;
			SteerClampLeft = steerAvoidanceThreshold * NormalDotLeft;
		}
	}

	if (SteerHitRight > -1.f && SteerHitRight > SteerHitLeft) {
		SteeringValue = FMath::Min(SteerClampRight, SteeringValue);
	}
	else if (SteerHitLeft > -1.f) {
		SteeringValue = FMath::Max(SteerClampLeft, SteeringValue);
	}

	if (HitDirectLeft) {
		SteeringValue = FMath::Max(0.2f, SteeringValue);
	}

	if (HitDirectRight) {
		SteeringValue = FMath::Min(-0.2f, SteeringValue);
	}


	// Trace directly ahead for collisions
	float forwardRightOffset = 50.f;

	AActor* pActorHitFront = NULL;
	bool HitForward = false;

	for (int i = -1; i <= 1; i++) {
		traceEndPosition = aiPosition;
		traceEndPosition += pAIActor->GetActorForwardVector() * FMath::Min(AISpeed + 50.f, 350.f);
		traceEndPosition += pAIActor->GetActorUpVector() * upTraceOffset;
		traceEndPosition += pAIActor->GetActorRightVector() * forwardRightOffset * (float)i;

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
			FColor(255, 0, 0),
			false,
			0.03
		);

		AActor* pActorHitFront = RV_Hit.GetActor();
		if (pActorHitFront && pActorHitFront != PlayerPawn) {
			HitForward = true;
			break;
		}
	}

	// Trace directly ahead for speed
	traceEndPosition = aiPosition;
	traceEndPosition += pAIActor->GetActorForwardVector() * AISpeed + 50.f;
	traceEndPosition += pAIActor->GetActorUpVector() * 10.f;

	world->LineTraceSingleByChannel(
		RV_Hit,        //result
		aiPosition,    //start
		traceEndPosition, //end
		ECC_Pawn, //collision channel
		RV_TraceParams
	);

	pActorHitFront = RV_Hit.GetActor();
	bool HitForwardSpeed = false;

	if (pActorHitFront && pActorHitFront != PlayerPawn) {
		pAIController->hitForwardCount++;
	}
	else {
		pAIController->hitForwardCount--;
	}

	if (pAIController->hitForwardCount >= 15) {
		HitForward = true;
	}

	bool ShouldReverse = forwardDot < -0.3f || HitForward || (AISpeed < 20.f && (SteerHitRight == NumSidewaysChecks -1 || SteerHitLeft == NumSidewaysChecks - 1));// || (HitRight && HitLeft && NormalDotRight < -0.9f && NormalDotLeft < -0.9f);
	bool ShouldReverseTime = world->GetTimeSeconds() - pAIController->reverseStart < 1.f;

	if (ShouldReverse || ShouldReverseTime){
		if (SteerHitRight > SteerHitLeft) {
			SteeringValue = -1.f;
		}
		else {
			SteeringValue = 1.f;
		}

		if (ShouldReverse && !ShouldReverseTime) {
			pAIController->reverseStart = world->GetTimeSeconds();
		}		
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



	if (ShouldReverse || ShouldReverseTime) {
		ThrottleValue = -1.f;
	}

	if (HitForwardSpeed) {
		ThrottleValue = FMath::Min(0.f, ThrottleValue);
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
