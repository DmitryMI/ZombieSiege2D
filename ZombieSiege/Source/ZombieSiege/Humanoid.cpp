// Fill out your copyright notice in the Description page of Project Settings.


#include "Humanoid.h"

// Sets default values
AHumanoid::AHumanoid()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHumanoid::BeginPlay()
{
	Super::BeginPlay();
	
	graphicsComponent = Cast<UUnitGraphicsComponent>(GetComponentByClass(UUnitGraphicsComponent::StaticClass()));
	check(graphicsComponent);

	weaponManager = AWeaponManager::GetInstance(GetWorld());
	check(weaponManager);	

	SetupDefaultWeapon();
}

void AHumanoid::SetupDefaultWeapon()
{
	weaponInfo = weaponManager->GetWeaponInfo("SurvivorFists");
	check(weaponInfo);
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
			facingDirection = directionFlags;			
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

bool AHumanoid::CanMove()
{
	return
		GetUnitState() == EUnitState::Moving ||
		GetUnitState() == EUnitState::None;
}

bool AHumanoid::CanAttackTarget(AUnitBase* target)
{
	if (bIsOnCooldown)
	{
		return false;
	}

	if (GetUnitState() != EUnitState::None)
	{
		return false;
	}

	if (!CanCommitAttackTarget(target))
	{
		return false;
	}

	return true;
}

bool AHumanoid::CanCommitAttackTarget(AUnitBase* target)
{
	if (!weaponInfo)
	{
		return false;
	}

	if (!weaponInfo->CanThisWeaponEverAttackTarget())
	{
		return false;
	}

	if (!weaponInfo->CanAttackTarget(this, target))
	{
		return false;
	}

	return true;
}

void AHumanoid::OnBackswingTimerElapsed(AUnitBase* target)
{
	attackBackswingTimerDelegate.Unbind();

	if (CanCommitAttackTarget(target))
	{
		//Commit the attack
		weaponInfo->AttackTarget(this, target);
	}

	SetUnitState(EUnitState::AttackingRelaxation);

	check(weaponInfo);

	FTimerManager& timerManager = GetWorld()->GetTimerManager();

	float relaxationDuration = weaponInfo->GetRelaxationDuration();
	attackRelaxationTimerDelegate.BindUObject(this, &AHumanoid::OnRelaxationTimerElapsed);
	timerManager.SetTimer(attackRelaxationTimerHandle, attackRelaxationTimerDelegate, relaxationDuration, false, relaxationDuration);
}

void AHumanoid::OnRelaxationTimerElapsed()
{
	attackRelaxationTimerDelegate.Unbind();

	SetUnitState(EUnitState::None);

	if (weaponInfo == nullptr)
	{
		// If weaponInfo is suddenly nullptr, we don't have any option than to reset cooldown right away
		OnCooldownTimerElapsed();
		return;
	}

	float cooldownDuration = weaponInfo->GetCooldownDuration();

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
	if (!CanAttackTarget(target))
	{
		return false;
	}

	check(weaponInfo);

	SetUnitState(EUnitState::AttackingBackswing);

	bIsOnCooldown = true;

	FTimerManager& timerManager = GetWorld()->GetTimerManager();
	
	float backswingDuration = weaponInfo->GetBackswingDuration();
	attackBackswingTimerDelegate.BindUObject(this, &AHumanoid::OnBackswingTimerElapsed, target);
	timerManager.SetTimer(attackBackswingTimerHandle, attackBackswingTimerDelegate, backswingDuration, false, backswingDuration);

	return true;
}



