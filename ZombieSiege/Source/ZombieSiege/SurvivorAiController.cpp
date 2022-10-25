// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorAiController.h"
#include "UnitBase.h"
#include "Building.h"
#include "GatherOrder.h"
#include "BuildOrder.h"
#include "RepairOrder.h"
#include "JobBase.h"


void ASurvivorAiController::BeginPlay()
{
    Super::BeginPlay();
}

void ASurvivorAiController::PostSetManualModeEnabled(bool bIsEnabled)
{
    if (!bIsEnabled)
    {
        return;
    }

    if (!assignedToJob)
    {
        return;
    }

    AUnitBase* survivor = Cast<AUnitBase>(GetPawn());
    if (!survivor)
    {
        return;
    }

    assignedToJob->UnassignExecutor(survivor);
    
    // The job must set assignedToJob to nullptr during UnassignExecutor() call
    check(assignedToJob == nullptr);
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

void ASurvivorAiController::IssueGatherOrder(ADoodad* gatherableUnit, bool bQueue)
{
    UGatherOrder* order = CreateOrder<UGatherOrder>(gatherOrderClass);
    order->SetGatherTarget(Cast<ADoodad>(gatherableUnit));

    IssueOrder(order, bQueue);
}

void ASurvivorAiController::IssueBuildOrder(TSubclassOf<ABuilding> buildingClass, const FVector& location, bool bQueue)
{
    UBuildOrder* order = CreateOrder<UBuildOrder>(buildOrderClass);
    order->SetParameters(buildingClass, location);
    IssueOrder(order, bQueue);
}

void ASurvivorAiController::IssueRepairOrder(ABuilding* building, bool bQueue)
{
    URepairOrder* order = CreateOrder<URepairOrder>(repairOrderClass);
    order->SetTargetBuilding(building);
    IssueOrder(order, bQueue);
}

bool ASurvivorAiController::HandleTargetActorCommandAction(AActor* targetActor, bool bQueue)
{
    ADoodad* doodad = Cast<ADoodad>(targetActor);

    if (doodad)
    {
        IssueGatherOrder(doodad, bQueue);
        return true;
    }

    return Super::HandleTargetActorCommandAction(targetActor, bQueue);
}
