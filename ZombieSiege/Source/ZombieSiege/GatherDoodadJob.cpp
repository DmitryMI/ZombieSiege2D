// Fill out your copyright notice in the Description page of Project Settings.


#include "GatherDoodadJob.h"
#include "UnitBase.h"
#include "Doodad.h"
#include "ZombieSiegeUtils.h"
#include "SurvivorAiController.h"
#include "ZombieSiegeUtils.h"

void AGatherDoodadJob::FindExecutors()
{
	check(GetOwningPlayerController());

	const TArray<AUnitBase*>& controlledUnits = GetOwningPlayerController()->GetControlledUnits();

	TArray<AUnitBase*> suitableExecutors;

	for (AUnitBase* unit : controlledUnits)
	{
		if (!unit)
		{
			continue;
		}

		bool bAssign = false;

		float myMetric = CalculateJobSpecificPriorityMetric(unit);

		if (IsValidExecutor(unit))
		{
			if (!IsExecutorAssignedToThisJob(unit))
			{
				int unitJobPriority;
				bool hasJobPriority = GetUnitAssignedJobPriority(unit, unitJobPriority);
				if (!hasJobPriority)
				{
					bAssign = ShouldAssignUnemployedUnit(unit);
				}
				else
				{
					bool myMajorPrioGreater = GetJobPriority() > unitJobPriority;
					bAssign = myMajorPrioGreater;
				}
			}
		}

		if (bAssign)
		{
			auto comparator = [this](AUnitBase* a, AUnitBase* b)
			{
				return UZombieSiegeUtils::CompareDistances2D(a, b, targetDoodad);
			};

			UZombieSiegeUtils::InsertSortedAscending(suitableExecutors, unit, comparator);
		}		
	}

	if (suitableExecutors.Num() > 0)
	{
		AUnitBase* bestExecutor = suitableExecutors[0];
		AssignExecutor(bestExecutor);
	}
}

bool AGatherDoodadJob::IsJobInitialized()
{
	if (!GetOwningPlayerController())
	{
		return false;
	}

	if (!targetDoodad)
	{
		return false;
	}

	return true;
}

bool AGatherDoodadJob::IsFinished()
{
	if (!targetDoodad)
	{
		return false;
	}

	return !targetDoodad->IsAlive();
}

bool AGatherDoodadJob::IsFailed()
{
	return targetDoodad == nullptr;
}

void AGatherDoodadJob::OnStateChanged(EJobState stateOld, EJobState stateNew)
{
	Super::OnStateChanged(stateOld, stateNew);

	if (stateNew == EJobState::Executing)
	{
		IssueOrders();
	}
	else if (stateNew == EJobState::Finished || stateNew == EJobState::Failed)
	{
		Destroy();
	}
}

bool AGatherDoodadJob::IsValidExecutor(AUnitBase* executor)
{
	if (!Super::IsValidExecutor(executor))
	{
		return false;
	}

	check(targetDoodad);

	FVector locationUnused;
	bool hasPath = UZombieSiegeUtils::GetBestLocationNearUnitToArriveWorld(GetWorld(), executor, targetDoodad, 50.0f, locationUnused);
	
	if (!hasPath)
	{
		FString executorName;
		FString doodadName;
		targetDoodad->GetName(doodadName);
		executor->GetName(executorName);
		UE_LOG(LogSurvivorJobs, Log, TEXT("Executor %s invalidated: no path to %s"), *executorName, *doodadName);
	}

	return hasPath;
}

float AGatherDoodadJob::CalculateJobSpecificPriorityMetric(AUnitBase* unit)
{
	check(unit);
	
	if (!targetDoodad)
	{
		return -1000;
	}

	FVector unitLocation = unit->GetActorLocation();
	FVector targetLocation = targetDoodad->GetActorLocation();

	// TODO Use path length, not distance!
	float distanceSquare = (targetLocation - unitLocation).SizeSquared2D();

	float metric = 1000.0f - distanceSquare;

	return metric;
}

bool AGatherDoodadJob::ShouldAssignUnemployedUnit(AUnitBase* unit)
{
	return true;
}


void AGatherDoodadJob::BeginPlay()
{
	Super::BeginPlay();
	jobState = EJobState::Created;
}

bool AGatherDoodadJob::CanExecute()
{
	return Super::CanExecute();
}

AGatherDoodadJob::AGatherDoodadJob()
{
	targetDoodad = nullptr;
	PrimaryActorTick.TickInterval = 0.5f;
}

void AGatherDoodadJob::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);
}

void AGatherDoodadJob::SetTargetDoodad(ADoodad* doodad)
{
	check(doodad);

	if (targetDoodad && jobState != EJobState::Created)
	{
		// Changing of a doodad is not implemented
		checkNoEntry();
	}

	targetDoodad = doodad;
}

ADoodad* AGatherDoodadJob::GetTargetDoodad()
{
	return targetDoodad;
}


void AGatherDoodadJob::IssueOrders()
{
	check(targetDoodad);

	for (AUnitBase* executor : assignedExecutors)
	{
		AController* controller = executor->GetController();
		check(controller);

		ASurvivorAiController* survivorController = Cast<ASurvivorAiController>(controller);
		check(survivorController);

		survivorController->IssueGatherOrder(targetDoodad);
	}
}
