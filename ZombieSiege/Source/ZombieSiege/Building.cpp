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

	if (turretClass)
	{
		for (int i = 0; i < GetTotalPassengerSeats(); i++)
		{
			float angleStep = i * 360 / GetTotalPassengerSeats();
			FVector turretLocationVector = FVector::RightVector * turretSpawnRadius;
			FVector turretLocation = turretLocationVector.RotateAngleAxis(angleStep, FVector::UpVector);
			turretLocation.Z = GetActorLocation().Z;

			ATurret* turret = GetWorld()->SpawnActor<ATurret>(turretClass, turretLocation, FRotator::ZeroRotator);
			check(turret);
			turret->SetOwningUnit(this);
			turret->SetTurretIndex(i);

			turretLocation.Z = 0;
			turret->SetActorRelativeLocation(turretLocation);

			// TODO Active only when seat is occupied
			turret->SetTurrectActive(IsFullyBuilt());

			turrets.Add(turret);
		}
	}
}

void ABuilding::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (ATurret* turret : turrets)
	{
		turret->Destroy();
	}

	turrets.Empty();
}

bool ABuilding::CanAttackAnything(FAttackTestParameters testParams)
{
	if (!IsAlive())
	{
		return false;
	}

	if (!testParams.bTestPhysicalState)
	{
		return true;
	}

	int totalSeats = GetTotalPassengerSeats();

	/*
	if (totalSeats > 0 && GetOccupiedPassengerSeats() == 0)
	{
		return false;
	}
	*/

	if (!IsFullyBuilt())
	{
		return false;
	}

	return true;
}



ABuilding::ABuilding()
{
	AddClassifications(EUnitClassification::Building);
}

const TArray<ATurret*>& ABuilding::GetTurrets() const
{
	return turrets;
}

bool ABuilding::CanAttackPoint(const FVector& targetPoint, FAttackTestParameters testParams)
{
	if (!CanAttackAnything(testParams))
	{
		return false;
	}
	
	for (ATurret* turret : turrets)
	{
		if (turret->CanAttackPoint(targetPoint, testParams))
		{
			return true;
		}
	}

	return false;
}

bool ABuilding::CanAttackTarget(AUnitBase* targetUnit, FAttackTestParameters testParams)
{
	if (!CanAttackAnything(testParams))
	{
		return false;
	}

	for (ATurret* turret : turrets)
	{
		if (turret->CanAttackTarget(targetUnit, testParams))
		{
			return true;
		}
	}

	return false;
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

void ABuilding::FinishDying(const FDamageInstance& killingDamage)
{
	for (ATurret* turret : turrets)
	{
		turret->SetTurrectActive(false);
	}

	Super::FinishDying(killingDamage);
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
			
			for (ATurret* turret : GetTurrets())
			{
				turret->SetTurrectActive(true);
			}
			
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
