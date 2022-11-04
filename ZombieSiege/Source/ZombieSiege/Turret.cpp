// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "UnitBase.h"
#include "WeaponManager.h"

ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoSetupWeapon)
	{
		weaponInfo = AWeaponManager::GetInstance(GetWorld())->GetWeaponInfo(weaponDefaultName);
	}

	if (!attackDispatcher)
	{
		attackDispatcher = NewObject<UAttackDispatcher>(this);
	}
}

bool ATurret::CanAttackAnything(FAttackTestParameters testParams)
{
	if (weaponInfo == nullptr)
	{
		return false;
	}

	if (!testParams.GetPhysicalStateFlag())
	{
		return true;
	}

	if (!bIsTurretActive)
	{
		return false;
	}

	if (GetOwningUnit() == nullptr)
	{
		return false;
	}

	return true;
}

void ATurret::SetWeaponInfo(UWeaponInfo* weapon)
{
	weaponInfo = weapon;
}

bool ATurret::IsOnCooldown()
{
	check(attackDispatcher);
	return attackDispatcher->IsOnCooldown();
}

bool ATurret::CanAttackTarget(AActor* targetActor, FAttackTestParameters testParams)
{
	if (!CanAttackAnything(testParams))
	{
		return false;
	}

	if (testParams.GetCooldownFlag() && IsOnCooldown())
	{
		return false;
	}

	if (testParams.GetAffiliationFlag() && !UZombieSiegeUtils::AreEnemies(GetOwningUnit(), targetActor))
	{
		return false;
	}

	if (testParams.GetPhysicalStateFlag() && GetOwningUnit()->IsHidden())
	{
		return false;
	}

	if (!testParams.GetRangeFlag())
	{
		return weaponInfo->CanThisWeaponEverAttackTarget(targetActor);
	}

	return weaponInfo->CanAttackTarget(GetOwningUnit(), targetActor);
}

bool ATurret::CanAttackPoint(const FVector& targetPoint, FAttackTestParameters testParams)
{
	if (!CanAttackAnything(testParams))
	{
		return false;
	}

	if (testParams.GetCooldownFlag() && IsOnCooldown())
	{
		return false;
	}

	if (testParams.GetPhysicalStateFlag() && GetOwningUnit()->IsHidden())
	{
		return false;
	}

	if (!testParams.GetRangeFlag())
	{
		return weaponInfo->CanThisWeaponEverAttackPoint();
	}

	return weaponInfo->CanAttackPoint(GetOwningUnit(), targetPoint);
}


bool ATurret::CanBeginAttackTarget(AActor* targetActor)
{
	if (!CanAttackTarget(targetActor, FAttackTestParameters(true, true, false)))
	{
		return false;
	}

	return attackDispatcher->CanBeginAttackTarget(GetOwningUnit(), weaponInfo, targetActor);
}

bool ATurret::CanBeginAttackPoint(const FVector& targetPoint)
{
	if (!CanAttackPoint(targetPoint, FAttackTestParameters(true, true, false)))
	{
		return false;
	}

	return attackDispatcher->CanBeginAttackPoint(GetOwningUnit(), weaponInfo, targetPoint);
}

bool ATurret::BeginAttackTarget(AActor* targetActor)
{
	if (!CanBeginAttackTarget(targetActor))
	{
		return false;
	}

	FAttackParameters attackParams;
	FVector ownerLocation = GetOwningUnit()->GetActorLocation();
	FVector turretLocation = GetActorLocation();
	attackParams.projectileSpawnRelativeLocation = turretLocation - ownerLocation;

	return attackDispatcher->BeginAttackTarget(GetOwningUnit(), weaponInfo, targetActor, attackParams);
}

bool ATurret::BeginAttackPoint(const FVector& targetPoint)
{
	if (!CanBeginAttackPoint(targetPoint))
	{
		return false;
	}

	FAttackParameters attackParams;
	attackParams.projectileSpawnRelativeLocation = GetActorLocation() - GetOwningUnit()->GetActorLocation();
	attackParams.projectileSpawnRelativeLocation.Z = 0;

	return attackDispatcher->BeginAttackPoint(GetOwningUnit(), weaponInfo, targetPoint, attackParams);
}

void ATurret::SetTurrectActive(bool active)
{
	bIsTurretActive = active;
}

bool ATurret::IsTurretActive()
{
	return bIsTurretActive;
}

void ATurret::SetOwningUnit(AUnitBase* unit)
{
	if (unit != owningUnit)
	{
		FDetachmentTransformRules rules(EDetachmentRule::KeepRelative, true);
		DetachFromActor(rules);
	}

	owningUnit = unit;

	FAttachmentTransformRules attachmentRules(EAttachmentRule::KeepRelative, false);
	AttachToActor(owningUnit, attachmentRules);
}

int ATurret::GetTurretIndex()
{
	return turretIndex;
}

void ATurret::SetTurretIndex(int index)
{
	turretIndex = index;
}

AUnitBase* ATurret::GetOwningUnit()
{
	return owningUnit;
}

