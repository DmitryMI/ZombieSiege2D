// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaponInfo.h"
#include "AttackDispatcher.h"
#include "Components/CapsuleComponent.h"

FGenericTeamId AUnitBase::GetGenericTeamId() const
{
	return unitTeamId;
}

bool AUnitBase::CanFinishAttackTargetWithWeapon(AActor* targetActor, UWeaponInfo* weapon)
{
	return CanAttackTargetWithWeapon(targetActor, weapon, FAttackTestParameters(false, true, false));
}

bool AUnitBase::CanFinishAttackPointWithWeapon(const FVector targetPoint, UWeaponInfo* weapon)
{
	return CanAttackPointWithWeapon(targetPoint, weapon, FAttackTestParameters(false, true, false));
}

bool AUnitBase::IsOnCooldown()
{
	check(attackDispatcher);
	return attackDispatcher->IsOnCooldown();
}

void AUnitBase::SetUnitState(EUnitState nextState)
{
	EUnitState oldState = currentState;
	currentState = nextState;
	if (oldState != currentState)
	{
		onUnitStateChangedEvent.Broadcast(oldState, nextState);
	}
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

const TArray<AUnitBase*>& AUnitBase::GetPassengers()
{
	return passengers;
}

bool AUnitBase::CanEnterPassengerCarrier(AUnitBase* carrier)
{
	if (!carrier)
	{
		return false;
	}

	if (this->GetTotalPassengerSeats() != 0)
	{
		return false;
	}

	if (!carrier->CanBeEnteredByPassenger(this))
	{
		return false;
	}

	float distSqr2D = UZombieSiegeUtils::GetDistance2DBetweenSimpleCollisions(this, carrier);

	if (distSqr2D > FMath::Square(enterPassengerCarrierRadius))
	{
		return false;
	}

	return true;
}

void AUnitBase::SetPassengerCarrier(AUnitBase* carrier)
{
	if (carrier == passengerCarrier)
	{
		return;
	}
	AUnitBase* oldCarrier = passengerCarrier;
	passengerCarrier = carrier;

	if (passengerCarrier)
	{
		check(GetTotalPassengerSeats() == 0);

		SetUnitHidden(true);

		FAttachmentTransformRules attachmentRules(EAttachmentRule::KeepRelative, false);
		AttachToActor(passengerCarrier, attachmentRules);
	}
	else
	{
		check(oldCarrier);

		FDetachmentTransformRules detachmentRules(EDetachmentRule::KeepRelative, false);
		DetachFromActor(detachmentRules);

		SetUnitHidden(false);

		FVector center = oldCarrier->GetActorLocation();
		float radius = oldCarrier->GetSimpleCollisionRadius() + enterPassengerCarrierRadius;
		FVector location;
		bool hasPoint = UZombieSiegeUtils::GetRandomReachableLocation(GetWorld(), center, radius, location);
		if (hasPoint)
		{
			SetActorLocation(location);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[UniBase: SetPassengerCarrier] Cannot find a point to dispatch a passenger!"));
		}
	}
}

bool AUnitBase::EnterPassengerCarrier(AUnitBase* carrier)
{
	if (!CanEnterPassengerCarrier(carrier))
	{
		return false;
	}

	carrier->AddPassenger(this);
	SetPassengerCarrier(carrier);

	return true;
}

void AUnitBase::LeavePassengerCarrier()
{
	if (!passengerCarrier)
	{
		return;
	}

	passengerCarrier->RemovePassenger(this);
	SetPassengerCarrier(nullptr);
}

int AUnitBase::GetTotalPassengerSeats() const
{
	return passengerSeats;
}

int AUnitBase::GetOccupiedPassengerSeats() const
{
	return passengers.Num();
}

int AUnitBase::GetFreePassengerSeats() const
{
	return GetTotalPassengerSeats() - GetOccupiedPassengerSeats();
}

bool AUnitBase::CanBeEnteredByPassenger(AUnitBase* passenger) const
{
	if (GetFreePassengerSeats() == 0)
	{
		return false;
	}

	return true;
}

void AUnitBase::AddPassenger(AUnitBase* passenger)
{
	//check(HasClassifications(EUnitClassification::PassengerCarrier));

	check(GetFreePassengerSeats() > 0);

	passengers.Add(passenger);

	PostAddPassenger(passenger);
}

void AUnitBase::PostAddPassenger(AUnitBase* passenger)
{
}

void AUnitBase::RemovePassenger(AUnitBase* passenger)
{
	//check(HasClassifications(EUnitClassification::PassengerCarrier));

	passengers.Remove(passenger);

	PostRemovePassenger(passenger);
}

void AUnitBase::PostRemovePassenger(AUnitBase* passenger)
{
}

void AUnitBase::OnHealthRegenerationTimerElapsed()
{
	float regenerationAmount = healthRegenerationPerSecond * healthRegenerationInterval;
	
	if (regenerationAmount < 0)
	{
		if (!IsAlive())
		{
			return;
		}

		float damage = -regenerationAmount;
		FDamageInstance negativeRegenerationDamageInstance(this, damage, nullptr);
		ReceiveDamage(negativeRegenerationDamageInstance);
	}
	else if(regenerationAmount > 0)
	{
		float healthDelta = GetMaxHealth() - GetHealth();

		if (FMath::IsNearlyZero(healthDelta))
		{
			return;
		}

		if (regenerationAmount > healthDelta)
		{
			regenerationAmount = healthDelta;
		}

		FHealingInstance regenerationInstance(this, regenerationAmount);
		ReceiveHealing(regenerationInstance);
	}
	else
	{
		// Do nothing for Zero regeneration
	}
}

#if WITH_EDITOR  
void AUnitBase::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FString propName;
	PropertyChangedEvent.Property->GetName(propName);
	if (propName == "health")
	{
		SetHealth(health);
	}
	else if (propName == "maxHealth")
	{
		SetMaxHealth(maxHealth);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AUnitBase::MakeAllPassengersLeave()
{
	TArray<AUnitBase*> passengersCopy = passengers;
	for (AUnitBase* passenger : passengersCopy)
	{
		passenger->LeavePassengerCarrier();
	}
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
	AZombieSiegePlayerController* oldController = owningPlayerController;

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

	uint8 teamId = TEAM_NEUTRAL_PASSIVE;
	FGenericTeamId unitTeam;

	if (owningPlayerController)
	{
		teamId = owningPlayerController->GetGenericTeamId().GetId();
	}
	
	unitTeam = FGenericTeamId(teamId);
	SetGenericTeamId(unitTeam);

	onOwningPlayerChangedEvent.Broadcast(this, oldController, owningPlayerController);
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

float AUnitBase::GetHealthRegenerationPerSecond() const
{
	return healthRegenerationPerSecond;
}

void AUnitBase::SetHealthRegenerationPerSecond(float value)
{
	healthRegenerationPerSecond = value;
}

bool AUnitBase::CanAttackPointWithWeapon(const FVector& targetPoint, UWeaponInfo* weapon, FAttackTestParameters testParams)
{
	if (weapon == nullptr)
	{
		return false;
	}

	if (testParams.GetCooldownFlag() && IsOnCooldown())
	{
		return false;
	}

	if (testParams.GetPhysicalStateFlag() && IsHidden())
	{
		return false;
	}

	if (!testParams.GetRangeFlag())
	{
		return weapon->CanThisWeaponEverAttackPoint();
	}

	return weapon->CanAttackPoint(this, targetPoint);
}

bool AUnitBase::CanAttackPoint(const FVector& targetPoint, FAttackTestParameters testParams)
{
	return false;
}

bool AUnitBase::CanAttackTargetWithWeapon(AActor* targetUnit, UWeaponInfo* weapon, FAttackTestParameters testParams)
{
	if (weapon == nullptr)
	{
		return false;
	}

	if (testParams.GetCooldownFlag() && IsOnCooldown())
	{
		return false;
	}

	if (testParams.GetAffiliationFlag() && !UZombieSiegeUtils::AreEnemies(this, targetUnit))
	{
		return false;
	}

	if (testParams.GetPhysicalStateFlag() && IsHidden())
	{
		return false;
	}

	if (testParams.GetReachabilityFlag())
	{
		float attackRange = weapon->GetRange();
		FVector unused;
		bool reachable = UZombieSiegeUtils::GetBestLocationNearUnitToArrive(this, this, targetUnit, attackRange, unused);
		if (!reachable)
		{
			return false;
		}
	}

	if (!testParams.GetRangeFlag())
	{
		return weapon->CanThisWeaponEverAttackTarget(targetUnit);
	}

	return weapon->CanAttackTarget(this, targetUnit);
}


bool AUnitBase::CanAttackTarget(AActor* targetUnit, FAttackTestParameters testParams)
{
	return false;
}

float AUnitBase::GetAttackRange()
{
	return 200.0f;
}

void AUnitBase::FinishDying(const FDamageInstance& killingDamageInstance)
{
	PreFinishDying(killingDamageInstance);

	bIsAlive = false;
	MakeAllPassengersLeave();
	SetActorEnableCollision(false);	
	SetUnitState(EUnitState::Dead);

	FUnitDiedEventArgs diedArgs(this, killingDamageInstance.source);
	onUnitDiedEvent.Broadcast(diedArgs);

	PostFinishDying(killingDamageInstance);
}

void AUnitBase::SetMovementComponentSpeedCap(float speedCap)
{
	if (movementComponent)
	{
		//movementComponent->MaxSpeed = speedCap;
		movementComponent->MaxWalkSpeed = speedCap;
		movementComponent->MaxFlySpeed = speedCap;
	}
}

bool AUnitBase::BeginAttackTargetWithWeapon(AActor* targetActor, UWeaponInfo* weapon)
{
	check(attackDispatcher);

	//FAttackTestParameters testParams(true, true, false);
	FAttackTestParameters testParams(EAttackTestFlags::Cooldown | EAttackTestFlags::Range | EAttackTestFlags::PhysicalState);

	if (!CanAttackTargetWithWeapon(targetActor, weapon, testParams))
	{
		return false;
	}

	return attackDispatcher->BeginAttackTarget(this, weapon, targetActor);
}

bool AUnitBase::BeginAttackPointWithWeapon(const FVector& point, UWeaponInfo* weapon)
{
	check(attackDispatcher);

	if (!CanAttackPointWithWeapon(point, weapon, FAttackTestParameters(true, true, false)))
	{
		return false;
	}

	return attackDispatcher->BeginAttackPoint(this, weapon, point);
}

void AUnitBase::OnAttackStateChanged(const FAttackDispatcherStateChangedEventArgs& args)
{
	switch (args.stateNew)
	{
	case EAttackState::None:
		SetUnitState(EUnitState::None);
		break;
	case EAttackState::Backswing:
		SetUnitState(EUnitState::AttackingBackswing);
		break;
	case EAttackState::Relaxation:
		SetUnitState(EUnitState::AttackingRelaxation);
		break;
	case EAttackState::Cooldown:
		SetUnitState(EUnitState::None);
		break;
	}
}

void AUnitBase::OnDecayTimerExpiredHandler()
{
	Destroy();
}

AUnitBase::AUnitBase() : Super()
{
	
}

void AUnitBase::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	unitTeamId = TeamID;
}

float AUnitBase::GetVisionRadius()
{
	return visionRadius;
}

EArmorNature AUnitBase::GetArmorNature()
{
	return armorNature;
}

void AUnitBase::SetFacingDirection(EFaceDirection direction)
{
	facingDirection = direction;
}

EFaceDirection AUnitBase::GetFacingDirection()
{
	return facingDirection;
}


bool AUnitBase::BeginAttackTarget(AActor* target)
{
	return false;
}

bool AUnitBase::BeginAttackPoint(const FVector& point)
{
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

	UCapsuleComponent* capsule = GetCapsuleComponent();
	check(capsule);
	capsule->SetCapsuleRadius(collisionRadius);
	capsule->SetCapsuleHalfHeight(collisionHeight / 2);

	movementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	check(movementComponent);

	SetMovementComponentSpeedCap(GetMaxSpeed());

	if (attackDispatcher == nullptr)
	{
		attackDispatcher = NewObject<UAttackDispatcher>(this);

		attackDispatcher->OnAttackDispatcherStateChanged().AddUObject(this, &AUnitBase::OnAttackStateChanged);
	}

	check(healthRegenerationInterval >= 0);

	GetWorld()->GetTimerManager().SetTimer(
		healthRegenerationTimerHandle,		
		this,
		&AUnitBase::OnHealthRegenerationTimerElapsed,
		healthRegenerationInterval,
		true,
		healthRegenerationInterval
	);
}

void AUnitBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (passengerCarrier)
	{
		LeavePassengerCarrier();
	}

	// If an alive unit is somehow deleted, run the death sequence, because other objects can rely on it
	if (IsAlive())
	{
		FinishDying(FDamageInstance());
	}

	onUnitDestroyedEvent.Broadcast(this);

	Super::EndPlay(EndPlayReason);
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

void AUnitBase::PreFinishDying(const FDamageInstance& killingDamageInstance)
{
	// Can be overridden in child classes to play some animations
	SetActorHiddenInGame(true);
}

void AUnitBase::PostFinishDying(const FDamageInstance& killingDamageInstance)
{
	SetActorHiddenInGame(true);

	UWorld* world = GetWorld();

	check(world);
	check(decayTime > 0);

	world->GetTimerManager().SetTimer(decayTimerHandle, this, &AUnitBase::OnDecayTimerExpiredHandler, decayTime, false, decayTime);
}

float AUnitBase::ReceiveDamage(const FDamageInstance& damage)
{
	if (bIsDying || !bIsAlive)
	{
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

void AUnitBase::SetUnitHidden(bool bIsHidden)
{
	if (IsUnitHidden() == bIsHidden)
	{
		return;
	}

	const float zOffset = -10000.0f;

	FVector actorLocation = GetActorLocation();
	if (bIsHidden)
	{
		actorLocation.Z += zOffset;
		
		movementComponent->GravityScale = 0.0f;
		movementComponent->MaxFlySpeed = 0;
		movementComponent->MaxWalkSpeed = 0;
	}
	else
	{
		actorLocation.Z -= zOffset;

		movementComponent->GravityScale = 1.0f;
		movementComponent->MaxFlySpeed = GetMaxSpeed();
		movementComponent->MaxWalkSpeed = GetMaxSpeed();
	}

	SetActorLocation(actorLocation);

	SetActorHiddenInGame(bIsHidden);
	SetActorEnableCollision(!bIsHidden);

	onUnitHiddenStateChangedEvent.Broadcast(this, bIsHidden);
}

bool AUnitBase::IsUnitHidden()
{
	return IsHidden();
}


