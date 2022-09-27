// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingJob.h"
#include "Building.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivorAiController.h"
#include "Survivor.h"
#include "ZombieSiegeUtils.h"

void ABuildingJob::SetBuildingClass(const TSubclassOf<ABuilding>& clazz)
{
	buildingClass = clazz;

	marker->SetBuildingClass(clazz);
}

TSubclassOf<ABuilding> ABuildingJob::GetBuildingClass()
{
	return buildingClass;
}

FVector ABuildingJob::GetBuildingLocation()
{
	return targetLocation;
}

void ABuildingJob::SetTargetLocation(const FVector& location)
{
	targetLocation = location;
	marker->SetActorLocation(location);
}

ABuildingJob* ABuildingJob::FromExistingMarker(UWorld* world, ABuildingPlacementMarker* markerArg, TSubclassOf<ABuildingJob> buildingJobClass)
{
	ABuildingJob* job = world->SpawnActorDeferred<ABuildingJob>(
		buildingJobClass,
		markerArg->GetActorTransform()
		);

	check(job->marker == nullptr);

	job->marker = markerArg;

	job->targetLocation = markerArg->GetActorLocation();
	job->buildingClass = markerArg->GetBuildingClass();

	UGameplayStatics::FinishSpawningActor(job, markerArg->GetActorTransform());

	return job;
}

void ABuildingJob::FindExecutors()
{
	Super::FindExecutors();
}

void ABuildingJob::BeginPlay()
{
	Super::BeginPlay();
	jobState = EJobState::Created;

	if (!marker)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.bNoFail = true;
		marker = GetWorld()->SpawnActor<ABuildingPlacementMarker>(markerClass, GetActorTransform(), spawnParams);
		check(marker);
		marker->SetBuildingClass(buildingClass);
	}
}

bool ABuildingJob::CanExecute()
{
	return Super::CanExecute();
}

bool ABuildingJob::IsJobInitialized()
{
	if (!GetOwningPlayerController())
	{
		return false;
	}

	if (buildingClass.Get() == nullptr)
	{
		return false;
	}

	return true;
}

bool ABuildingJob::IsFinished()
{
	if (!building)
	{
		return false;
	}

	if (!building->IsAlive())
	{
		return false;
	}

	return building->IsFullyBuilt();
}

bool ABuildingJob::IsFailed()
{
	return building && !building->IsAlive();
}

void ABuildingJob::OnStateChanged(EJobState stateOld, EJobState stateNew)
{
	Super::OnStateChanged(stateOld, stateNew);

	if (stateNew == EJobState::Executing)
	{
		for (AUnitBase* executor : assignedExecutors)
		{
			ASurvivor* survivorExecutor = Cast<ASurvivor>(executor);
			if (!survivorExecutor)
			{
				continue;
			}

			ASurvivorAiController* controller = Cast<ASurvivorAiController>(executor->GetController());
			check(controller);
			controller->IssueBuildOrder(buildingClass, targetLocation);

			survivorExecutor->OnUnitStartedBuilding().AddUObject(this, &ABuildingJob::OnUnitStartedBuildingHandler);
		
			break;
		}
	}
	else if (stateNew == EJobState::Finished || stateNew == EJobState::Failed)
	{
		Destroy();
	}
}

bool ABuildingJob::IsValidExecutor(AUnitBase* executor)
{
	if (!Super::IsValidExecutor(executor))
	{
		return false;
	}

	bool hasPath;

	if (building)
	{
		FVector locationUnused;
		hasPath = UZombieSiegeUtils::GetBestLocationNearUnitToArriveWorld(GetWorld(), executor, building, 250.0f, locationUnused);
	}
	else
	{
		hasPath = UZombieSiegeUtils::IsLocationReachable(GetWorld(), executor, targetLocation, 250.0f);
	}

	if (!hasPath)
	{
		FString executorName;
		executor->GetName(executorName);
		UE_LOG(LogSurvivorJobs, Log, TEXT("Executor %s invalidated: no path to %s"), *executorName, *targetLocation.ToString());
	}

	return hasPath;
}

float ABuildingJob::CalculateJobSpecificPriorityMetric(AUnitBase* unit)
{
	check(unit);	

	FVector unitLocation = unit->GetActorLocation();

	// TODO Use path length, not distance!
	float distanceSquare = (targetLocation - unitLocation).SizeSquared2D();

	float metric = 1000.0f - distanceSquare;

	return metric;
}

void ABuildingJob::OnUnitStartedBuildingHandler(const FUnitStartedBuildingEventArgs& args)
{
	AUnitBase* unit = args.builderUnit;
	ABuilding* buildingArg = args.building;

	check(unit);
	check(buildingArg);

	checkSlow(assignedExecutors.Contains(unit));
	check(!building);

	building = buildingArg;

	for (AUnitBase* executor : assignedExecutors)
	{
		ASurvivorAiController* controller = Cast<ASurvivorAiController>(executor->GetController());
		check(controller);

		controller->IssueRepairOrder(building);
	}
}
