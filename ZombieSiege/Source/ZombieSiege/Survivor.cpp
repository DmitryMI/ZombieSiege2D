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

	if (!clazzDefaultObject->CanBeBuildAt(GetWorld(), location))
	{
		return false;
	}

	float distanceSqr = (GetActorLocation() - location).SizeSquared2D();
	float range = repairWeapon->GetRange();

	if (FMath::Square(range) < distanceSqr)
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
