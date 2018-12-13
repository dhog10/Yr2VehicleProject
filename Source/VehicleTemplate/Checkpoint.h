// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

UCLASS()
class VEHICLETEMPLATE_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ActivateCheckpoint();
	void DeactivateCheckpoint();
	void SetMaterialColor(FLinearColor color);
	void SetMaterialAlpha(float alpha);

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
		FLinearColor ActiveColor;

	UPROPERTY(EditAnywhere)
		FLinearColor InactiveColor;

	UFUNCTION()
		void OnOverlapBegin(AActor* MyOverlappedActor, AActor* OtherActor);

	class AGameStateManager* pManager;
	bool bCheckpointActive;
	bool bDeactivating;

private:
	float DeactivatingAlpha;
	class UMaterialInstanceDynamic* MILight;
};
