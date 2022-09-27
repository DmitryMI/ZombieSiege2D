// Fill out your copyright notice in the Description page of Project Settings.


#include "Survivor.h"
#include "Building.h"

void ASurvivor::BeginPlay()
{
	Super::BeginPlay();

	repairWeapon = GetWeaponManager()->GetWeaponInfo(repairWeaponName);
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

	if (!clazzDefaultObject->CanBeBuildAt(GetWorld(), location, this))
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

	FActorSpawnParameters spawnParams;
	ABuilding* building = world->SpawnActor<ABuilding>(clazz, location, FRotator::ZeroRotator, spawnParams);

	AZombieSiegePlayerController* pc = GetOwningPlayerController();
	building->SetOwningPlayer(pc);

	FUnitStartedBuildingEventArgs args(this, building);

	onUnitStartedBuildingEvent.Broadcast(args);

	return building;
}

bool ASurvivor::CanRepairTarget(ABuilding* target)
{
	return CanAttackTargetWithWeapon(target, repairWeapon);
}

float ASurvivor::RepairTarget(ABuilding* target)
{
	return AttackTargetWithWeapon(target, repairWeapon);
}
