// Fill out your copyright notice in the Description page of Project Settings.

#include "AIWheeledVehicle.h"
#include "VehicleTemplateWheelFront.h"
#include "VehicleTemplateWheelRear.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/SkeletalMesh.h"
#include "AIWheeledVehicleController.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Engine/Engine.h"


AAIWheeledVehicle::AAIWheeledVehicle()
{
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SphereMaterialAsset(TEXT("/Game/Vehicle/Sedan/Materials/M_Vehicle_Sedan_Inst_TailLights"));
	if (SphereMaterialAsset.Succeeded()) {
		GetMesh()->SetMaterial(2, SphereMaterialAsset.Object);
	}

	// Simulation
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);

	Vehicle4W->WheelSetups[0].WheelClass = UVehicleTemplateWheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("Wheel_Front_Left");
	Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[1].WheelClass = UVehicleTemplateWheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("Wheel_Front_Right");
	Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	Vehicle4W->WheelSetups[2].WheelClass = UVehicleTemplateWheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("Wheel_Rear_Left");
	Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(0.f, -12.f, 0.f);

	Vehicle4W->WheelSetups[3].WheelClass = UVehicleTemplateWheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("Wheel_Rear_Right");
	Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(0.f, 12.f, 0.f);

	this->AIControllerClass = AAIWheeledVehicleController::StaticClass();

	FString Path = "BehaviorTree'/Game/BTAIVehicleChase.BTAIVehicleChase'";
	UBehaviorTree* BT = Cast<UBehaviorTree>(StaticLoadObject(UBehaviorTree::StaticClass(), nullptr, *Path));

	this->BehaviorTree = BT;
	this->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	StuckTime = 0;
	StuckPosition = FVector(0, 0, 0);
}

void AAIWheeledVehicle::Tick(float Delta)
{
	Super::Tick(Delta);

	// Destroy actor if stuck
	float Distance = FVector::Distance(StuckPosition, GetActorLocation());

	if (Distance > 150.f) {
		StuckPosition = GetActorLocation();
		StuckTime = 0.f;
	}
	else {
		StuckTime += Delta;
		if (StuckTime > 2.f) {
			Destroy();
		}
	}
}

void AAIWheeledVehicle::BeginPlay()
{
	Super::BeginPlay();
}

