// Fill out your copyright notice in the Description page of Project Settings.

#include "AIWheeledVehicleController.h"
#include "AIWheeledVehicle.h"
#include "WheeledVehicleMovementComponent.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include <BehaviorTree/BehaviorTreeComponent.h>
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"

AAIWheeledVehicleController::AAIWheeledVehicleController() {
	pVehicleMovementComponent = NULL;

	pBehaviourTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behaviour Tree"));
	pBlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
}

void AAIWheeledVehicleController::Possess(APawn* pPawn) {
	Super::Possess(pPawn);

	AAIWheeledVehicle* pVehicle = Cast<AAIWheeledVehicle>(pPawn);
	
	if (pVehicle) {
		pVehicleMovementComponent = pVehicle->GetVehicleMovementComponent();

		if (pVehicle->BehaviorTree->BlackboardAsset) {
			pBlackboardComp->InitializeBlackboard(*(pVehicle->BehaviorTree->BlackboardAsset));
		}

		pBehaviourTreeComp->StartTree(*pVehicle->BehaviorTree);
	}
}

void AAIWheeledVehicleController::Tick(float DeltaTime) {
	//pVehicleMovementComponent->SetThrottleInput(1.f);
	//pVehicleMovementComponent->SetSteeringInput(-1.f);
}