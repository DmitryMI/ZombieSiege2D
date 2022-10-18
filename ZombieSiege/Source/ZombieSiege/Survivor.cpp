// Fill out your copyright notice in the Description page of Project Settings.


#include "Survivor.h"
#include "Building.h"
#include "Doodad.h"
#include "ZombieSiegeUtils.h"

void ASurvivor::PushFromBuildingRadius(FVector buildingLocation, float radius)
{
	FVector actorLocation = GetActorLocation();

	FVector vec = actorLocation - buildingLocation;

	FVector pushDirection = vec.GetSafeNormal2D(SMALL_NUMBER, FVector::RightVector);

	float unitRadius;
	float unitHalfHeight;
	GetSimpleCollisionCylinder(unitRadius, unitHalfHeight);

	pushDirection *= unitRadius + radius - vec.Size2D();

	actorLocation += pushDirection;

	SetActorLocation(actorLocation);
}

void ASurvivor::BeginPlay()
{
	Super::BeginPlay();

	repairWeapon = GetWeaponManager()->GetWeaponInfo(repairWeaponName);
	gatheringWeapon = GetWeaponManager()->GetWeaponInfo(gatheringWeaponName);
}

ASurvivor::ASurvivor()
{
	AddClassifications(EUnitClassification::Organism);
}

bool ASurvivor::CanBuild(const TSubclassOf<ABuilding>& buildingClass, FVector location)
{
	UClass* clazz = buildingClass.Get();
	check(clazz);

	ABuilding* clazzDefaultObject = Cast<ABuilding>(clazz->GetDefaultObject());

	if (!clazzDefaultObject->CanBeBuiltBy(GetOwningPlayerController()))
	{
		return false;
	}

	if (!clazzDefaultObject->CanBeBuiltAt(GetWorld(), location, this))
	{
		return false;
	}

	float buildingRadius;
	float buildingHalfHeight;
	clazzDefaultObject->GetSimpleCollisionCylinder(buildingRadius, buildingHalfHeight);

	float distance = (GetActorLocation() - location).Size2D() - buildingRadius;
	float range = repairWeapon->GetRange();

	if (range < distance)
	{
		return false;
	}

	return true;
}

ABuilding* ASurvivor::Build(const TSubclassOf<ABuilding>& buildingClass, FVector location)
{
	UClass* clazz = buildingClass.Get();
	check(clazz);

	if (!CanBuild(buildingClass, location))
	{
		return nullptr;
	}

	UWorld* world = GetWorld();
	check(world);

	float buildingRadius;
	float buildingHalfHeight;

	ABuilding* buildingDefaultObject = Cast<ABuilding>(clazz->GetDefaultObject());
	check(buildingDefaultObject);
	buildingDefaultObject->GetSimpleCollisionCylinder(buildingRadius, buildingHalfHeight);

	PushFromBuildingRadius(location, buildingRadius * 1.1f);

	FActorSpawnParameters spawnParams;
	ABuilding* building = world->SpawnActor<ABuilding>(clazz, location, FRotator::ZeroRotator, spawnParams);

	if (building)
	{
		AZombieSiegePlayerController* player = GetOwningPlayerController();
		if (player)
		{
			AZombieSiegePlayerState* playerState = player->GetPlayerState<AZombieSiegePlayerState>();
			bool resoursesTaken = playerState->TakeResourcesFromStorage(building->GetRequiredResources());
			check(resoursesTaken);
		}
		else
		{
			// This case must be already addressed in CanBuild()
		}

		AZombieSiegePlayerController* pc = GetOwningPlayerController();
		building->SetOwningPlayer(pc);

		FUnitStartedBuildingEventArgs args(this, building);

		onUnitStartedBuildingEvent.Broadcast(args);
	}
	else
	{
		FString name = GetUnitTypeName().ToString();
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn a Building %s at %s!"), *name, *location.ToString());
	}

	return building;
}

bool ASurvivor::CanRepairTarget(ABuilding* target, FAttackTestParameters testParams)
{
	if (UZombieSiegeUtils::AreEnemies(this, target))
	{
		return false;
	}

	testParams.bTestAffilation = false;
	bool superOk = AUnitBase::CanAttackTargetWithWeapon(target, repairWeapon, testParams);
	return superOk;
}

float ASurvivor::RepairTarget(ABuilding* target)
{
	return BeginAttackTargetWithWeapon(target, repairWeapon);
}

bool ASurvivor::CanGatherTarget(ADoodad* gatherTarget, FAttackTestParameters testParams)
{
	testParams.bTestAffilation = false;
	bool superOk = Super::CanAttackTargetWithWeapon(gatherTarget, gatheringWeapon, testParams);
	return superOk;
}

float ASurvivor::GatherTarget(ADoodad* gatherTarget)
{
	return BeginAttackTargetWithWeapon(gatherTarget, gatheringWeapon);
}
