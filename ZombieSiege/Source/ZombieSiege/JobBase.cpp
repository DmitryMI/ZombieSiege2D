// Fill out your copyright notice in the Description page of Project Settings.


#include "JobBase.h"

// Sets default values
AJobBase::AJobBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

int AJobBase::GetJobPriority()
{
	return jobPriority;
}

void AJobBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AJobBase::FindExecutors()
{
}

void AJobBase::SetJobPriority(int priority)
{
	jobPriority = priority;
}

void AJobBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int AJobBase::GetNumOfRequestedExecutors()
{
	return 0;
}

const TArray<AUnitBase*>& AJobBase::GetAssignedExecutors()
{
	return assignedExecutors;
}

void AJobBase::AssignExecutor(AUnitBase* unit)
{
	check(!assignedExecutors.Contains(unit));

	assignedExecutors.Add(unit);
}

void AJobBase::DeassignExecutor(AUnitBase* unit)
{
	assignedExecutors.Remove(unit);
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

