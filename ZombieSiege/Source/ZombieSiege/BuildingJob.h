// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JobBase.h"
#include "BuildingJob.generated.h"

class ABuilding;

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API ABuildingJob : public AJobBase
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABuilding> buildingClass;

	UPROPERTY(EditAnywhere)
	FVector targetLocation;

	UPROPERTY(VisibleAnywhere)
	ABuilding* building;

protected:
	virtual void FindExecutors() override;

	virtual void BeginPlay() override;

	virtual bool CanExecute() override;

	virtual bool IsJobInitialized() override;

	virtual bool IsFinished() override;

	virtual bool IsFailed() override;

	virtual void OnStateChanged(EJobState stateOld, EJobState stateNew) override;

	virtual bool IsValidExecutor(AUnitBase* executor) override;

	virtual float CalculateJobSpecificPriorityMetric(AUnitBase* unit) override;

	virtual void IssueOrders();

public:

	UFUNCTION(BlueprintCallable)
	void SetBuildingClass(const TSubclassOf<ABuilding>& clazz);

	UFUNCTION(BlueprintCallable)
	TSubclassOf<ABuilding> GetBuildingClass();

	UFUNCTION(BlueprintCallable)
	FVector GetBuildingLocation();

	UFUNCTION(BlueprintCallable)
	void SetTargetLocation(const FVector& location);
};
