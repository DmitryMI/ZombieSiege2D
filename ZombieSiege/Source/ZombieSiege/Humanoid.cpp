// Fill out your copyright notice in the Description page of Project Settings.


#include "Humanoid.h"

// Sets default values
AHumanoid::AHumanoid()
{
	PrimaryActorTick.bCanEverTick = true;
}

AWeaponManager* AHumanoid::GetWeaponManager()
{
	if (!weaponManager)
	{
		weaponManager = AWeaponManager::GetInstance(GetWorld());
		check(weaponManager);
	}
	
	return weaponManager;
}

void AHumanoid::BeginPlay()
{
	Super::BeginPlay();
	
	graphicsComponent = Cast<UUnitGraphicsComponent>(GetComponentByClass(UUnitGraphicsComponent::StaticClass()));
	check(graphicsComponent);

	weaponManager = AWeaponManager::GetInstance(GetWorld());
	check(weaponManager);	

	weaponDefault = weaponManager->GetWeaponInfo(weaponDefaultName);
	check(weaponDefault);
}

EFaceDirection AHumanoid::GetDirectionFromVector(const FVector& vec)
{
	if (vec.IsNearlyZero())
	{
		return EFaceDirection::NONE;
	}

	EFaceDirection directionFlags = EFaceDirection::NONE;

	if (FMath::Abs(vec.X) > FMath::Abs(vec.Y))
	{
		if (vec.X > 0)
		{
			directionFlags = EFaceDirection::Right;
		}
		else
		{
			directionFlags = EFaceDirection::Left;
		}
	}
	else
	{
		if (vec.Y > 0)
		{
			directionFlags = EFaceDirection::Down;
		}
		else
		{
			directionFlags = EFaceDirection::Up;
		}
	}	

	return directionFlags;
}

void AHumanoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector movementVector = GetVelocity();

	EFaceDirection directionFlags = GetDirectionFromVector(movementVector);

	if (CanMove())
	{
		SetMovementComponentSpeedCap(GetMaxSpeed());
	}
	else
	{
		SetMovementComponentSpeedCap(0);
	}
	
	if (!movementVector.IsNearlyZero())
	{
		if (CanMove())
		{
			SetFacingDirection(directionFlags);			
			SetUnitState(EUnitState::Moving);
		}
	}
	else
	{
		if (GetUnitState() == EUnitState::Moving)
		{
			SetUnitState(EUnitState::None);
		}
	}
}

bool AHumanoid::CanMove() const
{
	return
		GetUnitState() == EUnitState::Moving ||
		GetUnitState() == EUnitState::None;
}

float AHumanoid::GetAttackRange()
{
	return weaponDefault->GetRange();
}

bool AHumanoid::CanEverAttackTarget(AUnitBase* target)
{
	return weaponDefault->CanThisWeaponEverAttackTarget(target);
}

bool AHumanoid::CanAttackTarget(AUnitBase* target)
{
	return CanAttackTargetWithWeapon(target, weaponDefault);
}

bool AHumanoid::AttackTarget(AUnitBase* target)
{
	FVector vec = target->GetActorLocation() - GetActorLocation();
	EFaceDirection direction = GetDirectionFromVector(vec);
	SetFacingDirection(direction);

	return AttackTargetWithWeapon(target, weaponDefault);
}





