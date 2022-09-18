// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZombieSiegePlayerController.generated.h"

class ABuilding;
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

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AUnitBase>> buildables;
	
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

	UFUNCTION(BlueprintNativeEvent)
	void ShowGameUiInternal(bool bShow);

public:
	UFUNCTION(BlueprintCallable)
	void AddToControlledUnits(AUnitBase* unit);

	UFUNCTION(BlueprintCallable)
	void RemoveFromControlledUnits(AUnitBase* unit);

	UFUNCTION(BlueprintCallable)
	const TArray<AUnitBase*>& GetControlledUnits();

	UFUNCTION(BlueprintCallable)
	void RemoveJob(AJobBase* job);

	UFUNCTION(BlueprintCallable)
	const TArray<AJobBase*> GetJobs();

	UFUNCTION(BlueprintCallable)
	const TArray<TSubclassOf<AUnitBase>>& GetBuildableUnits();

	UFUNCTION(BlueprintCallable, Exec)
	void ShowGameUi(bool bShow);
};
