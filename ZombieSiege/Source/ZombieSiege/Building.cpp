// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ZombieSiegeUtils.h"
#include "Macros.h"
#include "WeaponManager.h"

void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	if (bIsBuiltOnSpawn)
	{
		SetBuildingProgress(GetMaxHealth());
	}
	else
	{
		SetUnitState(EUnitState::Birth);
		SetBuildingProgress(0);
	}

	weaponDefault = AWeaponManager::GetInstance(GetWorld())->GetWeaponInfo(weaponDefaultName);
}

ABuilding::ABuilding()
{
	AddClassifications(EUnitClassification::Building);
}

float ABuilding::ReceiveHealing(const FHealingInstance& repair)
{
	float healingActual = Super::ReceiveHealing(repair);

	if (!IsFullyBuilt())
	{
		AddBuildingProgress(repair.amount);
	}

	return healingActual;
}

bool ABuilding::CanAttackTarget(AUnitBase* attackTarget)
{
	int totalSeats = GetTotalPassengerSeats();

	if (totalSeats > 0 && GetOccupiedPassengerSeats() == 0)
	{
		return false;
	}

	return CanAttackTargetWithWeapon(attackTarget, weaponDefault);
}

bool ABuilding::CanAttackPoint(const FVector& targetPoint)
{
	int totalSeats = GetTotalPassengerSeats();

	if (totalSeats > 0 && GetOccupiedPassengerSeats() == 0)
	{
		return false;
	}

	return CanAttackPointWithWeapon(targetPoint, weaponDefault);
}

bool ABuilding::CanEverAttackPoint()
{
	if (!weaponDefault)
	{
		return false;
	}

	return weaponDefault->CanThisWeaponEverAttackPoint();
}

bool ABuilding::CanEverAttackTarget(AUnitBase* targetUnit)
{
	if (!weaponDefault)
	{
		return false;
	}

	return weaponDefault->CanThisWeaponEverAttackPoint();
}

float ABuilding::GetBuildingProgressFraction() const
{
	return buildingProgress / GetMaxHealth();
}

float ABuilding::GetBuildingProgress() const
{
	return buildingProgress;
}

void ABuilding::SetBuildingProgressFraction(float progressFraction)
{
	progressFraction = FMath::Clamp(progressFraction, 0.0f, 1.0f);

	SetBuildingProgress(progressFraction * GetMaxHealth());
}

void ABuilding::SetBuildingProgress(float progress)
{
	float progressOld = buildingProgress;

	progress = FMath::Clamp(progress, 0, GetMaxHealth());
	buildingProgress = progress;

	if (!FMath::IsNearlyEqual(progressOld, buildingProgress))
	{
		if (GetUnitState() == EUnitState::Birth && IsFullyBuilt())
		{
			buildingProgress = GetMaxHealth();
			SetUnitState(EUnitState::None);
		}
		else if (GetUnitState() != EUnitState::Birth && buildingProgress < GetMaxHealth())
		{
			UE_LOG(LogTemp, Fatal, TEXT("SetBuildingProgress decreased building progress of a fully-built building! This is not supported."));
		}

		FBuildingProgressChangedEventArgs args(this, progressOld, buildingProgress);
		onBuildingProgressChangedEvent.Broadcast(args);
	}

}

void ABuilding::AddBuildingProgress(float addProgress)
{
	check(addProgress >= 0);
	float progress = GetBuildingProgress() + addProgress;

	SetBuildingProgress(progress);
}

bool ABuilding::IsBuiltOnSpawn() const
{
	return bIsBuiltOnSpawn;
}

bool ABuilding::IsFullyBuilt() const
{
	return FMath::IsNearlyEqual(buildingProgress, GetMaxHealth());
}

bool ABuilding::NeedsRepair() const
{
	return !IsFullyBuilt() || GetHealth() < GetMaxHealth();
}

bool ABuilding::CanBeBuiltAt(UWorld* world, const FVector& location, AUnitBase* builder) const
{
	float radius;
	float halfHeight;

	GetSimpleCollisionCylinder(radius, halfHeight);

	FCollisionShape shape = FCollisionShape::MakeCapsule(radius, halfHeight);

	FCollisionObjectQueryParams queryObjectParams;

	queryObjectParams.AddObjectTypesToQuery(BUILDING_TRACE);
	queryObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	queryObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Destructible);

	FCollisionQueryParams queryParams;
	if (builder)
	{
		queryParams.AddIgnoredActor(builder);
	}

	bool overlap = world->OverlapAnyTestByObjectType(location, FQuat::Identity, queryObjectParams, shape, queryParams);

	DrawDebugCapsule(world, location, halfHeight, radius, FQuat::Identity, FColor::Blue);

	return !overlap;
}

bool ABuilding::CanBeBuiltBy(AZombieSiegePlayerController* playerController) const
{
	const auto& required = GetRequiredResources();

	if (!playerController)
	{
		return UZombieSiegeUtils::IsFree(required);
	}

	AZombieSiegePlayerState* playerState = playerController->GetPlayerState<AZombieSiegePlayerState>();
	check(playerState);

	bool hasEnough = playerState->HasEnoughResourcesInStorage(required);
	return hasEnough;
}

bool ABuilding::CanBeBuilt(UWorld* world, const FVector& location, AZombieSiegePlayerController* playerController, AUnitBase* builder)
{
	return CanBeBuiltBy(playerController) && CanBeBuiltAt(world, location, builder);
}

void ABuilding::SetIsBuiltOnSpawn(bool isBuiltOnSpawn)
{
	bIsBuiltOnSpawn = isBuiltOnSpawn;
}
