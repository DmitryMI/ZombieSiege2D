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

void AHumanoid::OnAttackStateChanged(const FAttackDispatcherStateChangedEventArgs& args)
{
	Super::OnAttackStateChanged(args);
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


bool AHumanoid::CanBeginAttackTarget(AUnitBase* target)
{
	bool superOk = Super::CanBeginAttackTarget(target);
	bool weaponOk = attackDispatcher->CanBeginAttackTarget(this, weaponDefault, target);
	return superOk && weaponOk;
}

bool AHumanoid::BeginAttackTarget(AUnitBase* target)
{
	FVector vec = target->GetActorLocation() - GetActorLocation();
	EFaceDirection direction = GetDirectionFromVector(vec);
	SetFacingDirection(direction);

	if (!CanBeginAttackTarget(target))
	{
		return false;
	}

	return BeginAttackTargetWithWeapon(target, weaponDefault);
}

bool AHumanoid::CanEverAttackPoint()
{
	return weaponDefault->CanThisWeaponEverAttackPoint();
}

bool AHumanoid::CanBeginAttackPoint(const FVector& point)
{
	bool superOk = Super::CanBeginAttackPoint(point);
	bool weaponOk = weaponDefault->CanAttackPoint(this, point);
	return superOk && weaponOk;
}

bool AHumanoid::BeginAttackPoint(const FVector& point)
{
	FVector vec = point - GetActorLocation();
	EFaceDirection direction = GetDirectionFromVector(vec);
	SetFacingDirection(direction);

	if (!CanBeginAttackPoint(point))
	{
		return false;
	}

	return BeginAttackPointWithWeapon(point, weaponDefault);
}





