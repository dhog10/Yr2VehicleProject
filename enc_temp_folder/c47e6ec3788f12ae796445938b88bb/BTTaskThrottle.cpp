// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTaskThrottle.h"
#include <Runtime/AIModule/Classes/BehaviorTree/BTTaskNode.h>
#include <Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h>
#include <Runtime/AIModule/Classes/BehaviorTree/BehaviorTreeTypes.h>
#include "AIWheeledVehicleController.h"
#include "WheeledVehicleMovementComponent4W.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

EBTNodeResult::Type UBTTaskThrottle::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	AAIWheeledVehicleController* pController = Cast<AAIWheeledVehicleController>(OwnerComp.GetAIOwner());

	if (pController) {
		UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();

		float ThrottleValue = pBlackboard->GetValueAsFloat("ThrottleValue");

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString("Get Throttle  ") + FString::SanitizeFloat(ThrottleValue));

		float Throttle = 1.f;

		pController->pVehicleMovementComponent->SetThrottleInput(ThrottleValue);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UBTTaskThrottle::OnGameplayTaskActivated(UGameplayTask & task)
{
	//This method must be overridden
	//Otherwise, the linker will fail.
}


