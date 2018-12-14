// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VehicleTemplatePawn.h"
#include "VehicleTemplateWheelFront.h"
#include "VehicleTemplateWheelRear.h"
#include "VehicleTemplateHud.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/TextRenderComponent.h"
#include "Materials/Material.h"
#include "GameFramework/Controller.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "AIWheeledVehicle.h"
#include "VehiclePlayerState.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/GameStateBase.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"

// Debug
#include <Runtime/Engine/Public/DrawDebugHelpers.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

// Needed for VR Headset
#if HMD_MODULE_INCLUDED
#include "IXRTrackingSystem.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#endif // HMD_MODULE_INCLUDED

const FName AVehicleTemplatePawn::LookUpBinding("LookUp");
const FName AVehicleTemplatePawn::LookRightBinding("LookRight");

#define LOCTEXT_NAMESPACE "VehiclePawn"

AVehicleTemplatePawn::AVehicleTemplatePawn()
{
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);
	GetMesh()->OnComponentHit.AddDynamic(this, &AVehicleTemplatePawn::OnHit);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	OnActorBeginOverlap.AddDynamic(this, &AVehicleTemplatePawn::OnOverlapBegin);


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

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 200.f);
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 250.f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = true;
	Camera->FieldOfView = 90.f;

	// Create In-Car camera component 
	InternalCameraOrigin = FVector(0.0f, -40.0f, 120.0f);
	CameraDistance = 6000.f;
	CameraPitch = 0.f;
	CameraYaw = 0.f;

	InternalCameraBase = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraBase"));
	InternalCameraBase->SetRelativeLocation(InternalCameraOrigin);
	InternalCameraBase->SetupAttachment(GetMesh());

	InternalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCamera"));
	InternalCamera->bUsePawnControlRotation = true;
	InternalCamera->FieldOfView = 90.f;
	InternalCamera->SetupAttachment(InternalCameraBase);

	//Setup TextRenderMaterial
	static ConstructorHelpers::FObjectFinder<UMaterial> TextMaterial(TEXT("Material'/Engine/EngineMaterials/AntiAliasedTextMaterialTranslucent.AntiAliasedTextMaterialTranslucent'"));
	
	UMaterialInterface* Material = TextMaterial.Object;

	// Create text render component for in car speed display
	InCarSpeed = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarSpeed"));
	InCarSpeed->SetTextMaterial(Material);
	InCarSpeed->SetRelativeLocation(FVector(70.0f, -75.0f, 99.0f));
	InCarSpeed->SetRelativeRotation(FRotator(18.0f, 180.0f, 0.0f));
	InCarSpeed->SetupAttachment(GetMesh());
	InCarSpeed->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));

	// Create text render component for in car gear display
	InCarGear = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
	InCarGear->SetTextMaterial(Material);
	InCarGear->SetRelativeLocation(FVector(66.0f, -9.0f, 95.0f));	
	InCarGear->SetRelativeRotation(FRotator(25.0f, 180.0f,0.0f));
	InCarGear->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));
	InCarGear->SetupAttachment(GetMesh());
	
	// Colors for the incar gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	// Colors for the in-car gear display. One for normal one for reverse
	GearDisplayReverseColor = FColor(255, 0, 0, 255);
	GearDisplayColor = FColor(255, 255, 255, 255);

	bInReverseGear = false;
}

void AVehicleTemplatePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AVehicleTemplatePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVehicleTemplatePawn::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AVehicleTemplatePawn::OnMouseUp);
	PlayerInputComponent->BindAxis("LookRight", this, &AVehicleTemplatePawn::OnMouseRight);

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AVehicleTemplatePawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AVehicleTemplatePawn::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AVehicleTemplatePawn::OnToggleCamera);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AVehicleTemplatePawn::Shoot);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AVehicleTemplatePawn::OnResetVR); 
}

void AVehicleTemplatePawn::MoveForward(float Val)
{
	GetVehicleMovementComponent()->SetThrottleInput(Val);
}

void AVehicleTemplatePawn::MoveRight(float Val)
{
	GetVehicleMovementComponent()->SetSteeringInput(Val);
}

void AVehicleTemplatePawn::OnHandbrakePressed()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void AVehicleTemplatePawn::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void AVehicleTemplatePawn::OnMouseRight(float val) {
	CameraYaw += val;
}

void AVehicleTemplatePawn::OnMouseUp(float val) {
	CameraPitch += val;
	CameraPitch = FMath::Clamp(CameraPitch, -90.f, 90.f);
}

void AVehicleTemplatePawn::OnToggleCamera()
{
	EnableIncarView(!bInCarCameraActive);
}

void AVehicleTemplatePawn::EnableIncarView(const bool bState, const bool bForce)
{
	if ((bState != bInCarCameraActive) || ( bForce == true ))
	{
		bInCarCameraActive = bState;
		
		if (bState == true)
		{
			OnResetVR();
			Camera->Deactivate();
			InternalCamera->Activate();
		}
		else
		{
			InternalCamera->Deactivate();
			Camera->Activate();
		}
		
		InCarSpeed->SetVisibility(bInCarCameraActive);
		InCarGear->SetVisibility(bInCarCameraActive);
	}
}


void AVehicleTemplatePawn::Tick(float Delta)
{
	Super::Tick(Delta);

	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;
	
	// Update the strings used in the hud (incar and onscreen)
	UpdateHUDStrings();

	// Set the string in the incar hud
	SetupInCarHUD();

	bool bHMDActive = false;
#if HMD_MODULE_INCLUDED
	if ((GEngine->XRSystem.IsValid() == true) && ((GEngine->XRSystem->IsHeadTrackingAllowed() == true) || (GEngine->IsStereoscopic3D() == true)))
	{
		bHMDActive = true;
	}
#endif // HMD_MODULE_INCLUDED
	if (bHMDActive == false)
	{
		if ( (InputComponent) && (bInCarCameraActive == true ))
		{
			FRotator HeadRotation = InternalCamera->RelativeRotation;
			HeadRotation.Pitch += InputComponent->GetAxisValue(LookUpBinding);
			HeadRotation.Yaw += InputComponent->GetAxisValue(LookRightBinding);
			InternalCamera->RelativeRotation = HeadRotation;
		}
	}

	// Camera rotation
	FRotator rot = FRotator(-15.f, CameraYaw, 0.f);
	FVector rotForward = FRotationMatrix(rot).GetScaledAxis(EAxis::X);
	
	if (false) {
		FVector camPos = rotForward * -6000;

		FVector Pos = GetActorLocation();
		// SpringArm->SetRelativeLocation(camPos);
		SpringArm->SetWorldLocation(Pos + camPos + FVector(0, 0, 250.f));

		FRotator cameraRotation = UKismetMathLibrary::FindLookAtRotation(FVector(0.f, 0.f, 0.f), -camPos);
		SpringArm->SetRelativeRotation(cameraRotation);
	}
	else {
		FVector camPos = rotForward * -1000;

		FVector Pos = GetActorLocation();
		// SpringArm->SetRelativeLocation(camPos);
		SpringArm->SetWorldLocation(Pos + camPos + FVector(0, 0, 250.f));

		FRotator cameraRotation = UKismetMathLibrary::FindLookAtRotation(FVector(0.f, 0.f, 0.f), -camPos);
		SpringArm->SetWorldRotation(cameraRotation);
	}
}

void AVehicleTemplatePawn::BeginPlay()
{
	Super::BeginPlay();

	bool bEnableInCar = false;
#if HMD_MODULE_INCLUDED
	bEnableInCar = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
#endif // HMD_MODULE_INCLUDED
	EnableIncarView(bEnableInCar,true);
}

void AVehicleTemplatePawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	int a = 0;
}

void AVehicleTemplatePawn::OnResetVR()
{
#if HMD_MODULE_INCLUDED
	if (GEngine->XRSystem.IsValid())
	{
		GEngine->XRSystem->ResetOrientationAndPosition();
		InternalCamera->SetRelativeLocation(InternalCameraOrigin);
		GetController()->SetControlRotation(FRotator());
	}
#endif // HMD_MODULE_INCLUDED
}

void AVehicleTemplatePawn::UpdateHUDStrings()
{
	float KPH = FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) * 0.036f;
	int32 KPH_int = FMath::FloorToInt(KPH);

	// Using FText because this is display text that should be localizable
	SpeedDisplayString = FText::Format(LOCTEXT("SpeedFormat", "{0} km/h"), FText::AsNumber(KPH_int));
	
	if (bInReverseGear == true)
	{
		GearDisplayString = FText(LOCTEXT("ReverseGear", "R"));
	}
	else
	{
		int32 Gear = GetVehicleMovement()->GetCurrentGear();
		GearDisplayString = (Gear == 0) ? LOCTEXT("N", "N") : FText::AsNumber(Gear);
	}	
}

void AVehicleTemplatePawn::SetupInCarHUD()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if ((PlayerController != nullptr) && (InCarSpeed != nullptr) && (InCarGear != nullptr) )
	{
		// Setup the text render component strings
		InCarSpeed->SetText(SpeedDisplayString);
		InCarGear->SetText(GearDisplayString);
		
		if (bInReverseGear == false)
		{
			InCarGear->SetTextRenderColor(GearDisplayColor);
		}
		else
		{
			InCarGear->SetTextRenderColor(GearDisplayReverseColor);
		}
	}
}

// Detect an overlap with another AI Actor
// Here I adjust the players health appropriately by the impact intensity

void AVehicleTemplatePawn::OnOverlapBegin(AActor* MyOverlappedActor, AActor* OtherActor)
{
	if (MyOverlappedActor == OtherActor) { return; }

	FVector rPos = MyOverlappedActor->GetActorLocation() - OtherActor->GetActorLocation();
	rPos.Normalize();

	FVector MyVel = MyOverlappedActor->GetVelocity();
	FVector OtherVel = OtherActor->GetVelocity();
	FVector rVel = OtherVel - MyVel;
	float vel = rVel.Size();

	MyVel.Normalize();

	float CollisionStrength = FMath::Abs(FVector::DotProduct(-MyVel, rPos)) * vel;

	if (OtherActor->GetClass() == AAIWheeledVehicle::StaticClass()) {
		UWorld* pWorld = GetWorld();
		AGameStateBase* pState = pWorld->GetGameState();

		AVehiclePlayerState* pPlayer = (AVehiclePlayerState*)pState->PlayerArray[0];

		// Adjust the health based on the relative velocity of the collision
		pPlayer->Health = FMath::Max(0.f, pPlayer->Health - (vel * 0.006f));

		// If the health is 0 then we quit the game currently
		if (pPlayer->Health <= 0.f) {
			FGenericPlatformMisc::RequestExit(false);
		}
	}
}

// Method to fire a projectile from the front of the player vehicle
void AVehicleTemplatePawn::Shoot()
{
	if (!ProjectileClass) { return; }

	UWorld* pWorld = GetWorld();

	AVehiclePlayerState* pState = (AVehiclePlayerState*)pWorld->GetGameState()->PlayerArray[0];

	// If the player has ammo, instantiate a new projectile
	if (pState->Ammo > 0) {
		pState->Ammo--;

		FActorSpawnParameters params;
		params.Owner = this;

		FRotator rotation = FRotator(0,0,0);
		FVector location = GetActorLocation() + (GetActorForwardVector() * 250.f) + (GetActorUpVector() * 80.f);

		AProjectile* pProjectile = pWorld->SpawnActor<AProjectile>(ProjectileClass, location, rotation, params);
		
		// Apply an impulse to the projectile actor to propel it forward from the players vehicle
		if (pProjectile) {
			USphereComponent* pSphere = Cast<USphereComponent>(pProjectile->GetRootComponent());
			if (pSphere) {
				pSphere->AddImpulse(GetActorForwardVector() * pSphere->GetBodyInstance()->GetBodyMass() * 5000.f);
			}
		}
	}
}


#undef LOCTEXT_NAMESPACE
