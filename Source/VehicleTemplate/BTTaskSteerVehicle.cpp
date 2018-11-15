// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTaskSteerVehicle.h"
#include <Runtime/AIModule/Classes/BehaviorTree/BTTaskNode.h>
#include <Runtime/AIModule/Classes/BehaviorTree/BehaviorTreeTypes.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "AIWheeledVehicleController.h"
#include "WheeledVehicleMovementComponent4W.h"


EBTNodeResult::Type UBTTaskSteerVehicle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIWheeledVehicleController* pController = Cast<AAIWheeledVehicleController>(OwnerComp.GetAIOwner());

	if (pController) {
		float Steering = FMath::RandRange(-1.f, 1.f);

		pController->pVehicleMovementComponent->SetSteeringInput(Steering);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UBTTaskSteerVehicle::OnGameplayTaskActivated(UGameplayTask & task)
{
	//NB: this method must be overridden even if it has an empty body
	//otherwise the linker will fail
}
