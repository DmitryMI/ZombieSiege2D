// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JobState.h"
#include "JobBase.generated.h"

class AUnitBase;
class AZombieSiegePlayerController;


DECLARE_LOG_CATEGORY_EXTERN(LogSurvivorJobs, Log, All);


UCLASS()
class ZOMBIESIEGE_API AJobBase : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	bool bIsExecutionAllowed = true;

	UPROPERTY(EditAnywhere)
	int jobPriority = 0;

	AZombieSiegePlayerController* owningPlayerController;

protected:

	UPROPERTY(VisibleAnywhere)
	EJobState jobState = EJobState::InvalidState;

	UPROPERTY(VisibleAnywhere)
	TArray<AUnitBase*> assignedExecutors;

	virtual void UnassignExecutorInternal(int executorAtIndex);

	bool IsExecutorAssignedToThisJob(AUnitBase* executor);

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void TickStateMachine();

	virtual bool CanExecute();

	virtual bool IsJobInitialized() { return true; }

	virtual bool IsFinished() { unimplemented(); return true; }

	virtual bool IsFailed() { unimplemented(); return false; }

	virtual bool IsValidExecutor(AUnitBase* unit);

	virtual void FindExecutors();

	virtual void OnStateChanged(EJobState stateOld, EJobState stateNew);

	virtual void FinalizeJob();

	virtual float CalculateJobSpecificPriorityMetric(AUnitBase* unit);

	bool GetUnitAssignedJobPriority(AUnitBase* unit, int& priority);

	void RemoveInvalidExecutors();

	template<typename T>
	T* GetUnitAssignedJob(AUnitBase* unit)
	{
		AController* controller = unit->GetController();
		if (!controller)
		{
			return nullptr;
		}
		ASurvivorAiController* survivorController = Cast<ASurvivorAiController>(controller);
		if (!survivorController)
		{
			return nullptr;
		}

		AJobBase* job = survivorController->GetAssignedToJob();

		return Cast<T>(job);
	}

public:	
	AJobBase();

	UFUNCTION(BlueprintCallable)
	virtual void SetOwningPlayerController(AZombieSiegePlayerController* owningPc);

	UFUNCTION(BlueprintCallable)
	AZombieSiegePlayerController* GetOwningPlayerController();

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
	const TArray<AUnitBase*>& GetAssignedExecutors();

	UFUNCTION(BlueprintCallable)
	virtual bool AssignExecutor(AUnitBase* unit);

	UFUNCTION(BlueprintCallable)
	virtual void UnassignExecutor(AUnitBase* unit, bool bAnnounceFreeWorker = true);

	UFUNCTION(BlueprintCallable)
	virtual void AllowExecution();

	UFUNCTION(BlueprintCallable)
	virtual void DisallowExecution();

	bool IsExecutionAllowed();

	virtual EJobState GetJobState();

};
