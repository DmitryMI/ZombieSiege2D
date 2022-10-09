// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitBase.h"

void AUnitBase::SetUnitState(EUnitState nextState)
{
	EUnitState oldState = currentState;
	currentState = nextState;
	onUnitStateChangedEvent.Broadcast(oldState, nextState);
}

FName AUnitBase::GetUnitTypeName() const
{
	return unitTypeName;
}

FName AUnitBase::GetUnitGraphicsDataName() const
{
	return unitGraphicsDataName;
}

EUnitState AUnitBase::GetUnitState() const
{
	return currentState;
}

UTexture2D* AUnitBase::GetPreviewTexture() const
{
	return previewTexture;
}

const TMap<EResourceType, float>& AUnitBase::GetRequiredResources() const
{
	return requiredResources;
}

AUnitBase* AUnitBase::GetPassengerCarrier() const
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

int AUnitBase::GetFreePassengerSeats() const
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

EUnitClassification AUnitBase::GetClassifications() const
{
	return static_cast<EUnitClassification>(classificationFlags);
}

bool AUnitBase::HasClassifications(EUnitClassification flags) const
{
	return (classificationFlags & static_cast<int>(flags)) == static_cast<int>(flags);
}

bool AUnitBase::HasAnyClassification(EUnitClassification flags) const
{
	return (classificationFlags & static_cast<int>(flags));
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

AZombieSiegePlayerState* AUnitBase::GetOwningPlayerState() const
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

AZombieSiegePlayerController* AUnitBase::GetOwningPlayerController() const
{
	if (owningPlayerController)
	{
		return owningPlayerController;
	}
	else
	{
		//FString name;
		//GetName(name);
		//UE_LOG(LogTemp, Warning, TEXT("GetOwningPlayerController(): Unit %s is not controlled by a player, returning nullptr"), *name);

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

float AUnitBase::GetMaxSpeed() const
{
	return speed;
}

bool AUnitBase::CanMove() const
{
	return false;
}

void AUnitBase::KillUnit(bool bForceDeath)
{
	//health = 0;
	SetHealth(0);

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

bool AUnitBase::IsDying() const
{
	return bIsDying;
}

bool AUnitBase::IsAlive() const
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

	if (!FMath::IsNearlyEqual(health, healthValue))
	{
		FHealthChangedEventArgs args(this, health, healthValue);		
		health = healthValue;

		onHealthChangedEvent.Broadcast(args);

		/*
		if (FMath::IsNearlyZero(health))
		{
			FDamageInstance emptyDamage;
			BeginDying(emptyDamage);
		}
		*/
	}
}

float AUnitBase::GetHealth() const
{
	return health;
}

void AUnitBase::SetMaxHealth(float maxHealthValue)
{
	if (maxHealthValue < 0)
	{
		maxHealthValue = 0;
	}

	if (GetHealth() > maxHealthValue)
	{
		//health = maxHealthValue;
		SetHealth(maxHealthValue);
	}

	if (FMath::IsNearlyZero(GetHealth()))
	{
		FDamageInstance emptyDamage;
		BeginDying(emptyDamage);
	}
}

float AUnitBase::GetMaxHealth() const
{
	return maxHealth;
}

bool AUnitBase::CanAttackTarget(AUnitBase* targetUnit)
{
	return false;
}

bool AUnitBase::CanEverAttackTarget(AUnitBase* targetUnit)
{
	return false;
}

float AUnitBase::GetAttackRange()
{
	return 200.0f;
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

float AUnitBase::GetVisionRadius()
{
	return visionRadius;
}

EArmorNature AUnitBase::GetArmorNature()
{
	return armorNature;
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
	FString name;
	GetName(name);

	if (bIsDying || !bIsAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("BeginDying invoked for a unit %s with bIsDying == %d and bIsAlive == %d"), *name, bIsDying, bIsAlive);
		return;
	}

	bIsDying = true;

	FUnitIsDyingEventArgs args(this, killingDamageInstance.source);
	
	onUnitIsDyingEvent.Broadcast(args);

	// Check if unit is still dying (no one has prevented that)
	if (bIsDying)
	{
		FString killerName = "<NO KILLER>";
		if (killingDamageInstance.source)
		{
			killingDamageInstance.source->GetName(killerName);
		}
		
		UE_LOG(LogTemp, Display, TEXT("Unit %s was killed by %s"), *name, *killerName);
		FinishDying(killingDamageInstance);
	}
	else
	{
		// Alive units should never have 0 health. Usually, the death preventer must directly set health to non-zero value
		if (FMath::IsNearlyZero(GetHealth()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Death of %s was prevented, but it's health is still zero. Preventer should always directly set health to non-zero."));
			//health = 1.0f;
			SetHealth(1.0f);
		}
	}
}

float AUnitBase::ReceiveDamage(const FDamageInstance& damage)
{
	// Simple implementation, should be overriden in child classes

	if (bIsDying || !bIsAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("ReceiveDamage invoked for a unit with bIsDying == %d and bIsAlive == %d"), bIsDying, bIsAlive);
		return 0.0f;
	}

	float currentHealth = GetHealth();
	float healthOld = currentHealth;

	currentHealth -= damage.amount;

	if (currentHealth < 0)
	{
		SetHealth(0);
	}
	else
	{
		SetHealth(currentHealth);
	}

	FDamageReceivedEventArgs damageEventArgs(this, damage.source, damage.amount, damage.weaponInfo);
	onDamageReceivedEvent.Broadcast(damageEventArgs);

	if (FMath::IsNearlyZero(GetHealth()))
	{
		BeginDying(damage);
	}

	return healthOld - GetHealth();
}

float AUnitBase::ReceiveHealing(const FHealingInstance& healing)
{
	if (bIsDying || !bIsAlive)
	{
		UE_LOG(LogTemp, Warning, TEXT("ReceiveHealing invoked for a unit with bIsDying == %d and bIsAlive == %d"), bIsDying, bIsAlive);
		return 0.0f;
	}

	float currentHealth = GetHealth();
	float healthOld = currentHealth;

	currentHealth += healing.amount;

	if (currentHealth >= maxHealth)
	{
		SetHealth(maxHealth);
	}
	else
	{
		SetHealth(currentHealth);
	}


	return GetHealth() - healthOld;
}


