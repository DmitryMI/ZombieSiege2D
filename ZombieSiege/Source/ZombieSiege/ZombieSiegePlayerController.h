// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZombieSiegePlayerController.generated.h"

class AUnitBase;
class AJobBase;
class ADoodad;

DECLARE_LOG_CATEGORY_EXTERN(LogZombieSiegePlayerController, Log, All);

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API AZombieSiegePlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	float cameraDefaultHeight = 1000;

	UPROPERTY(EditDefaultsOnly)
	float cameraDefaultSpeed = 1000;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	float cameraDefaultMaxSize = 4096;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	float cameraDefaultMinSize = 2048;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	float cameraDefaultZoomSpeed = 15000.0f;
	
	UPROPERTY(VisibleAnywhere)
	TArray<AUnitBase*> controlledUnits;

	UPROPERTY(VisibleAnywhere)
	TArray<AJobBase*> jobs;

	void OnCameraVertical(float value);
	void OnCameraHorizontal(float value);
	void OnCameraZoom(float value);

	void OnSelectActionPressed();
	void OnSelectActionReleased();
	void OnSelectActionDoubleClick();
	void OnCommandActionReleased();

protected:
	virtual void SetupInputComponent() override;

	void OnDoodadSelectDoubleClicked(ADoodad* doodad);

	AActor* GetActorUnderScreenPoint(float x, float y);

public:
	void AddToControlledUnits(AUnitBase* unit);
	void RemoveFromControlledUnits(AUnitBase* unit);

	const TArray<AUnitBase*>& GetControlledUnits();

	void RemoveJob(AJobBase* job);

	const TArray<AJobBase*> GetJobs();
};
