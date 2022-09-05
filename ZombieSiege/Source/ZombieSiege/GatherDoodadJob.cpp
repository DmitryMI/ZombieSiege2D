// Fill out your copyright notice in the Description page of Project Settings.


#include "GatherDoodadJob.h"
#include "UnitBase.h"
#include "Doodad.h"
#include "SurvivorAiController.h"

void AGatherDoodadJob::FindExecutors()
{
	check(GetOwningPlayerController());

	const TArray<AUnitBase*>& controlledUnits = GetOwningPlayerController()->GetControlledUnits();

	for (AUnitBase* unit : controlledUnits)
	{
		check(unit);

		ASurvivorAiController* controller = Cast<ASurvivorAiController>(unit->GetController());

		if (!controller)
		{
			continue;
		}

		if (!unit->IsAlive())
		{
			continue;
		}

		AJobBase* unitJob = controller->GetAssignedToJob();
		if (unitJob == this)
		{
			continue;
		}

		if (unitJob && unitJob->GetJobPriority() >= GetJobPriority())
		{
			continue;
		}

		AssignExecutor(unit);
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

	if (targetDoodad && jobState == EJobState::Executing)
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
