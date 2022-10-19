// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorAiController.h"
#include "UnitBase.h"
#include "Building.h"
#include "GatherOrder.h"
#include "BuildOrder.h"
#include "RepairOrder.h"


void ASurvivorAiController::BeginPlay()
{
    Super::BeginPlay();
}

void ASurvivorAiController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}

AJobBase* ASurvivorAiController::GetAssignedToJob()
{
    return assignedToJob;
}

void ASurvivorAiController::SetAssignedToJob(AJobBase* job)
{
    assignedToJob = job;

    if (job == nullptr)
    {
        CancelAllOrders();
    }
}

void ASurvivorAiController::IssueGatherOrder(AUnitBase* gatherableUnit)
{
    UGatherOrder* order = CreateOrder<UGatherOrder>(gatherOrderClass);
    order->SetGatherTarget(Cast<ADoodad>(gatherableUnit));
    IssueOrder(order);
}

void ASurvivorAiController::IssueBuildOrder(TSubclassOf<ABuilding> buildingClass, const FVector& location)
{
    UBuildOrder* order = CreateOrder<UBuildOrder>(buildOrderClass);
    order->SetParameters(buildingClass, location);
    IssueOrder(order);
}

void ASurvivorAiController::IssueRepairOrder(ABuilding* building)
{
    URepairOrder* order = CreateOrder<URepairOrder>(repairOrderClass);
    order->SetTargetBuilding(building);
    IssueOrder(order);
}
