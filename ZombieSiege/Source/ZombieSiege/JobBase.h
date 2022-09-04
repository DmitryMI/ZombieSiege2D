// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JobState.h"
#include "JobBase.generated.h"

class AUnitBase;

UCLASS()
class ZOMBIESIEGE_API AJobBase : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	bool bIsExecutionAllowed;

	UPROPERTY(VisibleAnywhere)
	int jobPriority;

protected:

	UPROPERTY(VisibleAnywhere)
	EJobState jobState = EJobState::Invalid;

	UPROPERTY(VisibleAnywhere)
	TArray<AUnitBase*> assignedExecutors;

	virtual void BeginPlay() override;

	virtual void FindExecutors();

public:	
	AJobBase();

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Gets priority of this job. Jobs with higher priority can steal executors from jobs of lower priority
	/// </summary>
	/// <returns>Priority</returns>
	int GetJobPriority();

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Sets priority of this job. Jobs with higher priority can steal executors from jobs of lower priority
	/// </summary>
	/// <param name="priority"></param>
	void SetJobPriority(int priority);

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual int GetNumOfRequestedExecutors();

	UFUNCTION(BlueprintCallable)
	const TArray<AUnitBase*>& GetAssignedExecutors();

	UFUNCTION(BlueprintCallable)
	virtual void AssignExecutor(AUnitBase* unit);

	UFUNCTION(BlueprintCallable)
	virtual void DeassignExecutor(AUnitBase* unit);

	UFUNCTION(BlueprintCallable)
	virtual void AllowExecution();

	UFUNCTION(BlueprintCallable)
	virtual void DisallowExecution();

	bool IsExecutionAllowed();

	virtual EJobState GetJobState();

	DECLARE_EVENT_OneParam(AJobBase, FOnJobFinishedEvent, AJobBase*);

	FOnJobFinishedEvent& OnJobFinished() 
	{
		return onJobFinishedEvent;
	}

	DECLARE_EVENT_ThreeParams(AJobBase, FOnJobStateChangedEvent, AJobBase*, EJobState stateOld, EJobState stateNew);

	FOnJobStateChangedEvent& OnJobStateChanged()
	{
		return onJobStateChangedEvent;
	}

private:
	FOnJobFinishedEvent onJobFinishedEvent;
	FOnJobStateChangedEvent onJobStateChangedEvent;
};
