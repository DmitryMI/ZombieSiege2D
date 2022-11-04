// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "DamageInstance.h"
#include "UnitBase.h"
#include "ZombieSiegeUtils.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	launchLocation = GetActorLocation();
	
	collisionComponent = Cast<UShapeComponent>(GetComponentByClass(UShapeComponent::StaticClass()));
	check(collisionComponent);

	movementComponent = Cast<UProjectileMovementComponent>(GetComponentByClass(UProjectileMovementComponent::StaticClass()));
	check(movementComponent);

	collisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnOverlapBegin);

	if (bOverrideMovement)
	{
		movementComponent->MaxSpeed = maxSpeed;
		movementComponent->InitialSpeed = maxSpeed;		
	}

	if (bOverrideCollision)
	{
		collisionComponent->SetCollisionProfileName("Projectile");
	}
}

bool AProjectileBase::IsTargetLocationReached()
{
	FVector deltaVec = targetPoint - GetActorLocation();
	float deltaSqr = deltaVec.Size2D();

	return deltaSqr <= FMath::Square(reachibilityTestRadius);
}

bool AProjectileBase::IsRangeExceeded()
{
	float distanceSqr = (GetActorLocation() - launchLocation).SizeSquared2D();

	return distanceSqr > FMath::Square(maxRange);
}

void AProjectileBase::BeginProjectileDeath()
{
	
}

void AProjectileBase::BeginProjectileDeathBlueprint_Implementation()
{

}

void AProjectileBase::FinishProjectileDeath()
{
	Destroy();
}

void AProjectileBase::OnDecayTimerElapsedHandler()
{
	FinishProjectileDeath();
}

void AProjectileBase::KillProjectile()
{
	bIsProjectileAlive = false;
	movementComponent->MaxSpeed = 0;
	movementComponent->Velocity = FVector::Zero();
	movementComponent->ProjectileGravityScale = 0.0f;

	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (mesh)
	{
		mesh->SetHiddenInGame(true);
	}
	else
	{
		SetActorHiddenInGame(true);
	}

	//UNiagaraComponent

	BeginProjectileDeath();
	BeginProjectileDeathBlueprint();

	if (FMath::IsNearlyZero(decayTime))
	{
		FinishProjectileDeath();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(decayTimerHandle, this, &AProjectileBase::OnDecayTimerElapsedHandler, decayTime, false, decayTime);
	}
}

void AProjectileBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsProjectileAlive)
	{
		return;
	}

	bool bIgnoreCollision = false;

	AUnitBase* unit = Cast<AUnitBase>(OtherActor);

	if (unit && unit->IsAlive() && unit->GetActorEnableCollision())
	{
		AUnitBase* instigator = GetInstigator<AUnitBase>();

		if (instigator && UZombieSiegeUtils::AreEnemies(instigator, unit))
		{
			float damage = FMath::RandRange(impactDamageMinMax.X, impactDamageMinMax.Y);
			FDamageInstance damageInstance(instigator, damage, weaponInfo);
			unit->ReceiveDamage(damageInstance);
			bIgnoreCollision = false;
		}
		else
		{
			bIgnoreCollision = true;
		}
	}

	AProjectileBase* otherProjectile = Cast<AProjectileBase>(OtherActor);
	if (otherProjectile && otherProjectile->bIsProjectileAlive)
	{
		AUnitBase* instigator = GetInstigator<AUnitBase>();
		AUnitBase* otherInstigator = otherProjectile->GetInstigator<AUnitBase>();
		if (instigator != otherInstigator && UZombieSiegeUtils::AreEnemies(instigator, otherInstigator))
		{
			bIgnoreCollision = false;
		}
		else
		{
			bIgnoreCollision = true;
		}
	}

	if (bDiesOnCollision && !bIgnoreCollision)
	{
		KillProjectile();
	}
}

void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsProjectileAlive)
	{
		if (bDiesOnTargetPointReached && IsTargetLocationReached())
		{
			KillProjectile();
		}
		else if (IsRangeExceeded())
		{
			KillProjectile();
		}
	}
}

void AProjectileBase::MoveTowards(const FVector& targetPointArg, const FVector& velocity)
{
	targetPoint = targetPointArg;

	if (velocity.IsNearlyZero())
	{

		FVector direction = targetPoint - GetActorLocation();
		direction.Z = 0;
		ensure(direction.Normalize());

		movementComponent->Velocity = direction * maxSpeed;
	}
	else
	{
		movementComponent->Velocity = velocity;
	}
}

UWeaponInfo* AProjectileBase::GetWeaponInfo()
{
	return weaponInfo;
}

void AProjectileBase::SetWeaponInfo(UWeaponInfo* weapon)
{
	weaponInfo = weapon;
}

void AProjectileBase::SetImpactDamageRange(const FVector2D& damageRange)
{
	impactDamageMinMax = damageRange;
}

float AProjectileBase::GetMaxSpeed()
{
	return maxSpeed;
}

void AProjectileBase::SetMaxSpeed(float speed)
{
	maxSpeed = speed;
	movementComponent->MaxSpeed = maxSpeed;
}

