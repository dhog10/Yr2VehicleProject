// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIWheeledVehicleController.generated.h"

/**
 * 
 */
UCLASS()
class VEHICLETEMPLATE_API AAIWheeledVehicleController : public AAIController
{
	GENERATED_BODY()
	
public:
	class UWheeledVehicleMovementComponent* pVehicleMovementComponent;
	AAIWheeledVehicleController();
	
	float reverseStart = 0.f;
	int hitForwardCount = 0;

	virtual void Possess(APawn* pPawn) override;
	virtual void Tick(float DeltaTime) override;

	class UBehaviorTreeComponent* pBehaviourTreeComp;
	class UBlackboardComponent* pBlackboardComp;
};
