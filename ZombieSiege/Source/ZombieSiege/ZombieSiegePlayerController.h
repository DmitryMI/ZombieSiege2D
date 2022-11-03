// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UnitDiedEventArgs.h"
#include "ZombieSiegeHUD.h"
#include "GenericTeamAgentInterface.h"
#include "Curves/CurveFloat.h"
#include "ZombieSiegePlayerController.generated.h"

class ABuilding;
class AUnitBase;
class AJobBase;
class ADoodad;
class AGatherDoodadJob;
class ABuildingJob;
class ABuildingPlacementMarker;
class AZombieSiegePlayerState;

DECLARE_LOG_CATEGORY_EXTERN(LogZombieSiegePlayerController, Log, All);

UENUM(BlueprintType)
enum class EControllerState : uint8
{
	None,
	BuildingPlacement,
	SelectingUnits,
};

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API AZombieSiegePlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	float cameraInterpolationFactorPerSecond = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	float cameraDefaultHeight = 1500;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	float cameraDefaultMaxSize = 10000;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	float cameraDefaultMinSize = 80;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	UCurveFloat* cameraAngleCurve;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	UCurveFloat* cameraMovementSpeedCurve;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	float cameraDefaultZoomSpeed = 30000.0f;

	UPROPERTY(EditDefaultsOnly)
	float selectionMouseDeltaThreshold = 5.0f;	

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABuildingPlacementMarker> buildingPlacementMarkerClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGatherDoodadJob> gatherDoodadJobClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABuildingJob> buildingJobClass;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AUnitBase>> buildables;

	UPROPERTY(VisibleAnywhere)
	TArray<AUnitBase*> selectedUnits;

	UPROPERTY(VisibleAnywhere)
	TArray<AJobBase*> jobs;

	void OnCameraVertical(float value);
	void OnCameraHorizontal(float value);
	void OnCameraZoom(float value);

	void OnSelectActionPressed();
	void OnSelectActionReleased();
	void OnSelectActionDoubleClick();

	void OnCommandActionReleased();

	void OnQueueOrderActionPressed();
	void OnQueueOrderActionReleased();

	void OnAddToSelectionActionPressed();
	void OnAddToSelectionActionReleased();

	void OnMouseMove(const FVector2D& delta);

	ABuildingPlacementMarker* buildingPlacementMarker;

	EControllerState controllerState;

	FVector2D mouseLastTickPosition;

	bool bSelectActionPressed;
	FVector2D selectActionPressedPosition;

	bool bCommandActionPressed;
	FVector2D commandActionPressedPosition;

	bool bQueueOrderActionPressed = false;
	bool bAddToSelectionActionPressed = false;

	FVector cameraTargetLocation;
	float cameraTargetZoom;

	void UpdateCamera(float deltaSeconds);

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	void OnDoodadSelectDoubleClicked(ADoodad* doodad);

	AActor* GetActorUnderScreenPoint(float x, float y);

	UFUNCTION(BlueprintNativeEvent)
	void ShowGameUiInternal(bool bShow);

	bool DeprojectMouseOnTerrain(FVector& deprojectedLocation);

	void OnControlledUnitDiedHandler(AZombieSiegePlayerState* sender, const FUnitDiedEventArgs& args);

	void SelectUnitsInsideSelectionBox();

	virtual void OnPossess(APawn* pawn) override;

public:

	virtual void Tick(float deltaSeconds) override;

	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;

	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	void AddUnitToSelection(AUnitBase* unit);

	UFUNCTION(BlueprintCallable)
	void RemoveUnitFromSelection(AUnitBase* unit);

	void ClearSelection();

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

	UFUNCTION(BlueprintCallable)
	void BeginBuildingJobPlacement(TSubclassOf<ABuilding> buildingClass);

	UFUNCTION(BlueprintCallable, Exec)
	void ShowGameUi(bool bShow);

	UFUNCTION(BlueprintCallable, Exec)
	void DebugAlert(bool bEnabled);

	
	template<typename T, typename S>
	bool GetActorsInSelectionBox(const TArray<T*>& searchSet, TArray<S*>& outActors)
	{
		AZombieSiegeHUD* hud = GetHUD<AZombieSiegeHUD>();
		check(hud);

		if (controllerState != EControllerState::SelectingUnits)
		{
			return false;
		}

		for (T* actor : searchSet)
		{
			if (!actor)
			{
				continue;
			}
			FBox2D selectionBox = hud->GetSelectionBox();

			FVector location = actor->GetActorLocation();
			FVector2D projectedScreen;
			bool projectionOk = ProjectWorldLocationToScreen(location, projectedScreen);

			if (!projectionOk)
			{
				return false;
			}

			if (!selectionBox.IsInside(projectedScreen))
			{
				continue;
			}

			S* actorCast = Cast<S>(actor);
			if (!actorCast)
			{
				continue;
			}

			outActors.Add(actorCast);
		}

		return outActors.Num() > 0;
	}
};
