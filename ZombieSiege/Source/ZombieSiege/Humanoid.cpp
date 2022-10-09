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

bool AHumanoid::IsOnCooldown()
{
	return bIsOnCooldown;
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

bool AHumanoid::CanAttackTargetWithWeapon(AUnitBase* target, UWeaponInfo* weapon)
{
	if (bIsOnCooldown)
	{
		return false;
	}

	if (GetUnitState() != EUnitState::None)
	{
		return false;
	}

	if (!CanCommitAttackTargetWithWeapon(target, weapon))
	{
		return false;
	}

	return true;
}

bool AHumanoid::CanCommitAttackTargetWithWeapon(AUnitBase* target, UWeaponInfo* weapon)
{
	if (!weapon)
	{
		return false;
	}

	if (!weapon->CanThisWeaponEverAttackTarget(target))
	{
		return false;
	}

	if (!weapon->CanAttackTarget(this, target))
	{
		return false;
	}

	return true;
}

void AHumanoid::OnBackswingTimerElapsed(AUnitBase* target)
{
	attackBackswingTimerDelegate.Unbind();

	if (CanCommitAttackTargetWithWeapon(target, activeWeapon))
	{
		//Commit the attack
		activeWeapon->AttackTarget(this, target);
	}

	SetUnitState(EUnitState::AttackingRelaxation);

	check(activeWeapon);

	FTimerManager& timerManager = GetWorld()->GetTimerManager();

	float relaxationDuration = activeWeapon->GetRelaxationDuration();
	attackRelaxationTimerDelegate.BindUObject(this, &AHumanoid::OnRelaxationTimerElapsed);
	timerManager.SetTimer(attackRelaxationTimerHandle, attackRelaxationTimerDelegate, relaxationDuration, false, relaxationDuration);
}

void AHumanoid::OnRelaxationTimerElapsed()
{
	attackRelaxationTimerDelegate.Unbind();

	SetUnitState(EUnitState::None);

	if (activeWeapon == nullptr)
	{
		// If weaponInfo is suddenly nullptr, we don't have any option than to reset cooldown right away
		OnCooldownTimerElapsed();
		return;
	}

	float cooldownDuration = activeWeapon->GetCooldownDuration();

	if (!FMath::IsNearlyZero(cooldownDuration))
	{

		FTimerManager& timerManager = GetWorld()->GetTimerManager();

		attackCooldownTimerDelegate.BindUObject(this, &AHumanoid::OnCooldownTimerElapsed);

		timerManager.SetTimer(
			attackCooldownTimerHandle,
			attackCooldownTimerDelegate,
			cooldownDuration,
			false,
			cooldownDuration);
	}
	else
	{
		OnCooldownTimerElapsed();
	}
}

void AHumanoid::OnCooldownTimerElapsed()
{
	bIsOnCooldown = false;
}

bool AHumanoid::AttackTarget(AUnitBase* target)
{
	FVector vec = target->GetActorLocation() - GetActorLocation();
	EFaceDirection direction = GetDirectionFromVector(vec);
	SetFacingDirection(direction);

	return AttackTargetWithWeapon(target, weaponDefault);
}

bool AHumanoid::AttackTargetWithWeapon(AUnitBase* target, UWeaponInfo* weapon)
{
	activeWeapon = weapon;

	if (!CanAttackTargetWithWeapon(target, activeWeapon))
	{
		return false;
	}

	check(activeWeapon);

	SetUnitState(EUnitState::AttackingBackswing);

	bIsOnCooldown = true;

	FTimerManager& timerManager = GetWorld()->GetTimerManager();

	float backswingDuration = activeWeapon->GetBackswingDuration();
	attackBackswingTimerDelegate.BindUObject(this, &AHumanoid::OnBackswingTimerElapsed, target);
	timerManager.SetTimer(attackBackswingTimerHandle, attackBackswingTimerDelegate, backswingDuration, false, backswingDuration);

	return true;
}



