// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitBase.h"

UTexture2D* AUnitBase::GetPreviewTexture()
{
	return previewTexture;
}

TMap<EResourceType, int> AUnitBase::GetRequiredResources()
{
	return requiredResources;
}

AUnitBase* AUnitBase::GetPassengerCarrier()
{
	return passengerCarrier;
}

void AUnitBase::SetPassengerCarrier(AUnitBase* carrier)
{
	if (carrier == passengerCarrier)
	{
		return;
	}

	passengerCarrier = carrier;

	if (passengerCarrier)
	{
		check(carrier->HasClassifications(EUnitClassification::PassengerCarrier));
		check(!this->HasClassifications(EUnitClassification::PassengerCarrier));

		SetActorHiddenInGame(true);
	}
	else
	{
		bool shouldBeHidden = ShouldBeHidden();
		SetActorHiddenInGame(shouldBeHidden);
	}
}

bool AUnitBase::EnterPassengerCarrier(AUnitBase* carrier)
{
	check(carrier);

	check(carrier->HasClassifications(EUnitClassification::PassengerCarrier));
	check(!this->HasClassifications(EUnitClassification::PassengerCarrier));
	
	float distSqr2D = (carrier->GetActorLocation() - GetActorLocation()).Size2D();

	if (distSqr2D > FMath::Square(enterPassengerCarrierRadius))
	{
		return false;
	}

	if (carrier->GetFreePassengerSeats() > 0)
	{
		carrier->AddPassenger(this);
		SetPassengerCarrier(carrier);
		return true;
	}
	else
	{
		return false;
	}
}

void AUnitBase::LeavePassengerCarrier()
{
	check(passengerCarrier);

	passengerCarrier->RemovePassenger(this);
	SetPassengerCarrier(nullptr);
}

int AUnitBase::GetFreePassengerSeats()
{
	return passengerSeats - passengers.Num();
}

void AUnitBase::AddPassenger(AUnitBase* passenger)
{
	check(HasClassifications(EUnitClassification::PassengerCarrier));

	check(GetFreePassengerSeats() > 0);

	passengers.Add(passenger);
}

void AUnitBase::RemovePassenger(AUnitBase* passenger)
{
	check(HasClassifications(EUnitClassification::PassengerCarrier));

	passengers.Remove(passenger);
}

EUnitClassification AUnitBase::GetClassifications()
{
	return static_cast<EUnitClassification>(classificationFlags);
}

bool AUnitBase::HasClassifications(EUnitClassification flags)
{
	return (classificationFlags & static_cast<int>(flags)) == static_cast<int>(flags);
}

void AUnitBase::AddClassifications(EUnitClassification flags)
{
	classificationFlags |= static_cast<int>(flags);
}

void AUnitBase::RemoveClassifications(EUnitClassification flags)
{
	classificationFlags &= ~static_cast<int>(flags);
}

void AUnitBase::SetClassifications(EUnitClassification flags)
{
	classificationFlags = static_cast<int>(flags);
}

void AUnitBase::GetSimpleCollisionCylinder(float& CollisionRadius, float& CollisionHalfHeight) const
{
	CollisionRadius = collisionRadius;
	CollisionHalfHeight = collisionHeight / 2.0f;
}

AZombieSiegePlayerState* AUnitBase::GetOwningPlayerState()
{
	AZombieSiegePlayerController* zombieSiegeController = GetOwningPlayerController();
	if (zombieSiegeController)
	{
		APlayerState* playerState = zombieSiegeController->GetPlayerState<APlayerState>();
		check(playerState);

		AZombieSiegePlayerState* zombieSiegePlayerState = Cast<AZombieSiegePlayerState>(playerState);
		check(zombieSiegePlayerState);

		return zombieSiegePlayerState;
	}
	else
	{
		return nullptr;
	}
}

AZombieSiegePlayerController* AUnitBase::GetOwningPlayerController()
{
	if (owningPlayerController)
	{
		return owningPlayerController;
	}
	else
	{
		FString name;
		GetName(name);

		UE_LOG(LogTemp, Warning, TEXT("GetOwningPlayerController(): Unit %s is not controlled by a player, returning nullptr"), *name);

		return nullptr;
	}
}

void AUnitBase::SetOwningPlayer(AZombieSiegePlayerController* controller)
{
	if (controller == owningPlayerController)
	{
		return;
	}

	if (owningPlayerController != nullptr)
	{
		owningPlayerController->RemoveFromControlledUnits(this);
	}

	owningPlayerController = controller;

	if (owningPlayerController != nullptr)
	{
		owningPlayerController->AddToControlledUnits(this);
	}
}

void AUnitBase::SetMaxSpeed(float speedArg)
{
	speed = speedArg;
	SetMovementComponentSpeedCap(speed);
}

float AUnitBase::GetMaxSpeed()
{
	return speed;
}

bool AUnitBase::CanMove()
{
	return false;
}

void AUnitBase::KillUnit(bool bForceDeath)
{
	health = 0;

	FDamageInstance emptyDamage;

	if (!bForceDeath)
	{
		BeginDying(emptyDamage);
	}
	else
	{
		FinishDying(emptyDamage);
	}
}

void AUnitBase::ReviveUnit()
{
	if (!bIsAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reviving alive unit!"));
		return;
	}

	bIsAlive = false;
}

void AUnitBase::ReviveUnitWithHealth(float setHealthTo)
{
	if (!bIsAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reviving alive unit! Health will not be updated"));
		return;
	}

	if (setHealthTo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reviving unit requires a non-zero positive health value"));
		setHealthTo = 1.0f;
	}

	SetHealth(setHealthTo);
	ReviveUnit();
}

bool AUnitBase::IsDying()
{
	return bIsDying;
}

bool AUnitBase::IsAlive()
{
	return bIsAlive && !bIsDying;
}

void AUnitBase::PreventDeath(float healthValue)
{
	if (bIsDying)
	{
		bIsDying = false;

		if (healthValue <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Death preventer must set a non-zero positive amount of health"));
			healthValue = 1.0f;
		}

		SetHealth(healthValue);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PreventDeath called for a not dying Unit. Always check IsDying() before calling PreventDeath()"));
	}
}

void AUnitBase::SetHealth(float healthValue)
{
	healthValue = FMath::Clamp(healthValue, 0, maxHealth);

	health = healthValue;

	if (FMath::IsNearlyZero(health))
	{
		FDamageInstance emptyDamage;
		BeginDying(emptyDamage);
	}
}

float AUnitBase::GetHealth()
{
	return health;
}

void AUnitBase::SetMaxHealth(float maxHealthValue)
{
	if (maxHealthValue < 0)
	{
		maxHealthValue = 0;
	}

	if (health > maxHealthValue)
	{
		health = maxHealthValue;
	}

	if (FMath::IsNearlyZero(health))
	{
		FDamageInstance emptyDamage;
		BeginDying(emptyDamage);
	}
}

float AUnitBase::GetMaxHealth()
{
	return maxHealth;
}

bool AUnitBase::CanAttackTarget(AUnitBase* target)
{
	return false;
}

void AUnitBase::FinishDying(const FDamageInstance& killingDamageInstance)
{
	// Should be overwritten by child classes (e.g. to start death animation).
	bIsAlive = false;
	Destroy();
}

void AUnitBase::SetMovementComponentSpeedCap(float speedCap)
{
	if (movementComponent)
	{
		movementComponent->MaxSpeed = speedCap;
	}
}


bool AUnitBase::AttackTarget(AUnitBase* target)
{
	// Should always be overriden in child classes
	checkNoEntry();

	return false;
}

void AUnitBase::BeginPlay()
{
	Super::BeginPlay();

	if (assignToLocalPlayerOnSpawn && owningPlayerController == nullptr)
	{
		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		check(playerController);

		AZombieSiegePlayerController *zsController = Cast<AZombieSiegePlayerController>(playerController);		
		check(zsController);

		SetOwningPlayer(zsController);
	}

	movementComponent = Cast<UFloatingPawnMovement>(GetComponentByClass(UFloatingPawnMovement::StaticClass()));

	SetMovementComponentSpeedCap(GetMaxSpeed());
}

bool AUnitBase::ShouldBeHidden()
{
	bool isInCarrier = GetPassengerCarrier() != nullptr;

	return isInCarrier;
}

void AUnitBase::BeginDying(const FDamageInstance& killingDamageInstance)
{
	bIsDying = true;

	FUnitIsDyingEventArgs args(this, killingDamageInstance.source);
	
	onUnitIsDyingEvent.Broadcast(args);

	// Check if unit is still dying (no one has prevented that)
	if (bIsDying)
	{
		FinishDying(killingDamageInstance);
	}
	else
	{
		// Alive units should never have 0 health. Usually, the death preventer must directly set health to non-zero value
		if (FMath::IsNearlyZero(health))
		{
			UE_LOG(LogTemp, Warning, TEXT("Death of %s was prevented, but it's health is still zero. Preventer should always directly set health to non-zero."));
			health = 1.0f;
		}
	}
}

void AUnitBase::ReceiveDamage(const FDamageInstance& damage)
{
	// Simple implementation, should be overriden in child classes

	float currentHealth = health;

	currentHealth -= damage.amount;

	if (currentHealth < 0)
	{
		health = 0;
	}
	else
	{
		health = currentHealth;
	}

	FDamageReceivedEventArgs damageEventArgs(this, damage.source, damage.amount);
	onDamageReceivedEvent.Broadcast(damageEventArgs);

	if (FMath::IsNearlyZero(health))
	{
		BeginDying(damage);
	}
}


