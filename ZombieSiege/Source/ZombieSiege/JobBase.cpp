// Fill out your copyright notice in the Description page of Project Settings.


#include "JobBase.h"
#include "UnitBase.h"
#include "ZombieSiegeUtils.h"
#include "SurvivorAiController.h"

DEFINE_LOG_CATEGORY(LogSurvivorJobs);

bool AJobBase::IsValidExecutor(AUnitBase* unit)
{
	if (!unit)
	{
		return false;
	}

	if (!unit->IsAlive())
	{
		return false;
	}

	ASurvivorAiController* controller = Cast<ASurvivorAiController>(unit->GetController());
	if (!controller)
	{
		return false;
	}

	return true;
}

void AJobBase::FindExecutors()
{
	// Basic implementation, assigns the first available executor.

	check(GetOwningPlayerController());

	const TArray<AUnitBase*>& controlledUnits = GetOwningPlayerController()->GetControlledUnits();

	TArray<AUnitBase*> unemployedExecutors;

	// First, check only unemployed units
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
					bAssign = true;
				}
				else
				{
					unemployedExecutors.Add(unit);
				}
			}
		}

		if (bAssign)
		{
			AssignExecutor(unit);
			return;
		}
	}

	// Now check employed units. We want to steal workers from jobs with lower priority
	for (AUnitBase* unit : unemployedExecutors)
	{
		check(unit);

		int unitJobPriority;

		if (GetUnitAssignedJobPriority(unit, unitJobPriority))
		{
			bool myMajorPrioGreater = GetJobPriority() > unitJobPriority;
			if (myMajorPrioGreater)
			{
				AssignExecutor(unit);
				return;
			}
		}
	}
}

void AJobBase::OnStateChanged(EJobState stateOld, EJobState stateNew)
{
	FString name;
	GetName(name);

	switch (stateNew)
	{
	case EJobState::InvalidState:
		checkNoEntry()
		break;

	case EJobState::Created:
		UE_LOG(LogSurvivorJobs, Log, TEXT("Job %s: Created."), *name);
		break;

	case EJobState::WaitingForExecutors:
		if (stateOld == EJobState::Created)
		{
			UE_LOG(LogSurvivorJobs, Log, TEXT("Job %s: Created -> Waiting"), *name);
		}
		else if (stateOld == EJobState::Executing)
		{
			UE_LOG(LogSurvivorJobs, Warning, TEXT("Job %s: Executing -> Waiting"), *name);
		}
		else
		{
			checkNoEntry();
		}
		break;

	case EJobState::Executing:
		UE_LOG(LogSurvivorJobs, Log, TEXT("Job %s: Waiting -> Executing"), *name);
		break;

	case EJobState::Finished:
		UE_LOG(LogSurvivorJobs, Log, TEXT("Job %s: Executing -> Finished"), *name);
		FinalizeJob();
		break;

	case EJobState::Failed:
		UE_LOG(LogSurvivorJobs, Warning, TEXT("Job %s : Executing -> Failed"), *name);
		FinalizeJob();
		break;

	default:
		checkNoEntry();
		break;
	}
}

void AJobBase::FinalizeJob()
{
	for (int i = 0; i < assignedExecutors.Num(); i++)
	{
		UnassignExecutorInternal(i);
	}

	GetOwningPlayerController()->RemoveJob(this);
}

float AJobBase::CalculateJobSpecificPriorityMetric(AUnitBase* unit)
{
	return 0.0f;
}

bool AJobBase::GetUnitAssignedJobPriority(AUnitBase* unit, int& priority)
{
	ASurvivorAiController* controller = Cast<ASurvivorAiController>(unit->GetController());
	if (!controller)
	{
		return false;
	}

	AJobBase* unitJob = controller->GetAssignedToJob();
	if (unitJob == nullptr)
	{
		return false;
	}
	
	priority = unitJob->GetJobPriority();
	return true;
}

void AJobBase::RemoveInvalidExecutors()
{
	int index = assignedExecutors.Num() - 1;

	for (int i = 0; i < assignedExecutors.Num(); i++)
	{
		AUnitBase* executor = assignedExecutors[i];
		bool bIsAssignedToThis = IsExecutorAssignedToThisJob(executor);
		bool bIsValid = IsValidExecutor(executor);
		if (!bIsValid || !bIsAssignedToThis)
		{
			UnassignExecutorInternal(i);
			i--;
		}
	}
}

AJobBase::AJobBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AJobBase::SetOwningPlayerController(AZombieSiegePlayerController* owningPc)
{
	owningPlayerController = owningPc;
}

AZombieSiegePlayerController* AJobBase::GetOwningPlayerController()
{
	return owningPlayerController;
}

int AJobBase::GetJobPriority()
{
	return jobPriority;
}

void AJobBase::UnassignExecutorInternal(int executorAtIndex)
{
	checkSlow(executorAtIndex >= 0 && assignedExecutors.Num() >= executorAtIndex);

	AUnitBase* executor = assignedExecutors[executorAtIndex];

	assignedExecutors.RemoveAt(executorAtIndex);

	FString jobName;
	FString unitName;
	GetName(jobName);
	executor->GetName(unitName);
	UE_LOG(LogSurvivorJobs, Warning, TEXT("Job %s: unassigned unit %s"), *jobName, *unitName);

	if (executor == nullptr)
	{
		return;
	}

	AController* controller = executor->GetController();
	if (controller == nullptr)
	{
		return;
	}

	ASurvivorAiController* survivorController = Cast<ASurvivorAiController>(controller);
	if (survivorController == nullptr)
	{
		return;
	}

	AJobBase* executorsJob = survivorController->GetAssignedToJob();
	if (executorsJob == this)
	{
		survivorController->SetAssignedToJob(nullptr);
	}
}

bool AJobBase::IsExecutorAssignedToThisJob(AUnitBase* executor)
{
	if (executor == nullptr)
	{
		return false;
	}

	AController* controller = executor->GetController();
	if (controller == nullptr)
	{
		return false;
	}

	ASurvivorAiController* survivorController = Cast<ASurvivorAiController>(controller);
	if (survivorController == nullptr)
	{
		return false;
	}

	AJobBase* executorsJob = survivorController->GetAssignedToJob();
	
	return executorsJob == this;
}

void AJobBase::BeginPlay()
{
	Super::BeginPlay();
	
	jobState = EJobState::Created;
	OnStateChanged(EJobState::InvalidState, jobState);
}

void AJobBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FinalizeJob();

	Super::EndPlay(EndPlayReason);

	FString name;
	GetName(name);
	UE_LOG(LogSurvivorJobs, Warning, TEXT("Job %s is destroyed."), *name);
}

void AJobBase::TickStateMachine()
{
	EJobState prevState = jobState;

	switch (jobState)
	{
	case EJobState::InvalidState:
		checkNoEntry();
		break;

	case EJobState::Created:
		if (IsJobInitialized())
		{
			jobState = EJobState::WaitingForExecutors;
			OnStateChanged(prevState, jobState);
		}

	case EJobState::WaitingForExecutors:
		if (!IsJobInitialized())
		{
			jobState = EJobState::Failed;
			OnStateChanged(prevState, jobState);
		}
		else if (CanExecute())
		{
			jobState = EJobState::Executing;
			OnStateChanged(prevState, jobState);
		}
		break;

	case EJobState::Executing:
		if (!IsJobInitialized())
		{
			jobState = EJobState::Failed;
			OnStateChanged(prevState, jobState);
		}
		else if (!CanExecute())
		{
			jobState = EJobState::WaitingForExecutors;
			OnStateChanged(prevState, jobState);
		}
		else if (IsFailed())
		{
			jobState = EJobState::Failed;
			OnStateChanged(prevState, jobState);
		}
		else if (IsFinished())
		{
			jobState = EJobState::Finished;
			OnStateChanged(prevState, jobState);
		}
		break;
	}
}

bool AJobBase::CanExecute()
{
	RemoveInvalidExecutors();

	if (assignedExecutors.Num() == 0)
	{
		FindExecutors();
	}

	return assignedExecutors.Num() > 0;
}


void AJobBase::SetJobPriority(int priority)
{
	jobPriority = priority;
}

void AJobBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsExecutionAllowed)
	{
		TickStateMachine();
	}
}

const TArray<AUnitBase*>& AJobBase::GetAssignedExecutors()
{
	return assignedExecutors;
}

bool AJobBase::AssignExecutor(AUnitBase* unit)
{
	check(!assignedExecutors.Contains(unit));

	AController* controller = unit->GetController();
	check(controller);

	ASurvivorAiController* survivorController = Cast<ASurvivorAiController>(controller);
	check(survivorController);

	AJobBase* prevJob = survivorController->GetAssignedToJob();

	if (prevJob == this)
	{
		return true;
	}

	if (IsValidExecutor(unit))
	{
		survivorController->SetAssignedToJob(this);
		if (prevJob)
		{
			prevJob->UnassignExecutor(unit, false);
		}
		assignedExecutors.Add(unit);

		FString jobName;
		FString unitName;
		GetName(jobName);
		unit->GetName(unitName);
		UE_LOG(LogSurvivorJobs, Log, TEXT("Job %s: assigned unit %s"), *jobName, *unitName);

		return true;
	}
	else
	{
		return false;
	}
}

void AJobBase::UnassignExecutor(AUnitBase* unit, bool bAnnounceFreeWorker)
{
	int index = assignedExecutors.IndexOfByKey(unit);
	if (index == INDEX_NONE)
	{
		return;
	}

	UnassignExecutorInternal(index);
}

void AJobBase::AllowExecution()
{
	bIsExecutionAllowed = true;
}

void AJobBase::DisallowExecution()
{
	bIsExecutionAllowed = false;
}

bool AJobBase::IsExecutionAllowed()
{
	return bIsExecutionAllowed;
}

EJobState AJobBase::GetJobState()
{
	return jobState;
}

