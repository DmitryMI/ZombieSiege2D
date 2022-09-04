// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorAiController.h"

AJobBase* ASurvivorAiController::GetAssignedToJob()
{
    return assignedToJob;
}

void ASurvivorAiController::SetAssignedToJob(AJobBase* job)
{
    assignedToJob = job;
}
