// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitAiController.h"
#include "SurvivorAiController.generated.h"

class AJobBase;
class AUnitBase;
class ABuilding;

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API ASurvivorAiController : public AUnitAiController
{
	GENERATED_BODY()


private:
	UPROPERTY(VisibleAnywhere)
	AJobBase* assignedToJob;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* gathererBehaviorTree;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* buildBehaviorTree;

	UPROPERTY(EditDefaultsOnly)
	UBehaviorTree* repairBehaviorTree;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float deltaTime) override;

	UFUNCTION(BlueprintCallable)
	AJobBase* GetAssignedToJob();

	UFUNCTION(BlueprintCallable)
	virtual void SetAssignedToJob(AJobBase* job);

	UFUNCTION(BlueprintCallable)
	virtual void IssueGatherOrder(AUnitBase* gatherableUnit);

	UFUNCTION(BlueprintCallable)
	virtual void IssueBuildOrder(TSubclassOf<ABuilding> buildingClass, const FVector& location);

	UFUNCTION(BlueprintCallable)
	virtual void IssueRepairOrder(ABuilding* building);

	
};
