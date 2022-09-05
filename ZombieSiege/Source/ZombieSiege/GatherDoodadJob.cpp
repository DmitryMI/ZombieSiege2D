// Fill out your copyright notice in the Description page of Project Settings.


#include "GatherDoodadJob.h"
#include "UnitBase.h"
#include "Doodad.h"
#include "ZombieSiegeUtils.h"
#include "SurvivorAiController.h"

void AGatherDoodadJob::FindExecutors()
{
	check(GetOwningPlayerController());

	const TArray<AUnitBase*>& controlledUnits = GetOwningPlayerController()->GetControlledUnits();

	for (AUnitBase* unit : controlledUnits)
	{
		check(unit);		

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
			AssignExecutor(unit);
		}

		if (assignedExecutors.Num() > 0)
		{
			break;
		}
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
	bool hasPath = UZombieSiegeUtils::GetBestLocationNearUnitToArriveWorld(GetWorld(), executor, targetDoodad, 250.0f, locationUnused);
	
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

	float metric = 1.0f / distanceSquare;

	return metric;
}

bool AGatherDoodadJob::ShouldAssignUnemployedUnit(AUnitBase* unit)
{
	AZombieSiegePlayerController* pc = GetOwningPlayerController();
	TArray<AJobBase*> jobs = pc->GetJobs();

	float myMetric = CalculateJobSpecificPriorityMetric(unit);

	int myPriority = GetJobPriority();

	for (AJobBase* job : jobs)
	{
		if (job == this)
		{
			continue;
		}

		if (job->GetJobState() != EJobState::WaitingForExecutors)
		{
			continue;
		}

		int theirPriority = job->GetJobPriority();

		AGatherDoodadJob* jobGather = Cast<AGatherDoodadJob>(job);
		if (!jobGather)
		{
			continue;
		}

		float theirMetric = jobGather->CalculateJobSpecificPriorityMetric(unit);

		bool isMyPriorityLower = myPriority < theirPriority || (myPriority == theirPriority && myMetric < theirMetric);
		if (isMyPriorityLower)
		{
			return false;
		}
	}

	return true;
}


void AGatherDoodadJob::BeginPlay()
{
	Super::BeginPlay();
	jobState = EJobState::Created;
}

bool AGatherDoodadJob::CanExecute()
{
	RemoveInvalidExecutors();

	if (assignedExecutors.Num() == 0)
	{
		FindExecutors();
	}

	return assignedExecutors.Num() > 0;
}

AGatherDoodadJob::AGatherDoodadJob()
{
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
	for (AUnitBase* executor : assignedExecutors)
	{
		AController* controller = executor->GetController();
		check(controller);

		ASurvivorAiController* survivorController = Cast<ASurvivorAiController>(controller);
		check(survivorController);

		survivorController->IssueGatherOrder(targetDoodad);
	}
}
