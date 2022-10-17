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

	collisionComponent = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	collisionComponent->SetCollisionProfileName("Projectile");
	collisionComponent->SetSphereRadius(collisionRadius);

	collisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnOverlapBegin);

	RootComponent = collisionComponent;

	flipbookRenderer = CreateDefaultSubobject<UPaperFlipbookComponent>("FlipbookRenderer");
	flipbookRenderer->SetWorldRotation(FRotator(-90, 0, 0));
	flipbookRenderer->SetFlipbook(movingFlipbook);
	FAttachmentTransformRules attachmentRules(EAttachmentRule::KeepWorld, false);
	flipbookRenderer->SetupAttachment(RootComponent);
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	movementComponent = Cast<UProjectileMovementComponent>(GetComponentByClass(UProjectileMovementComponent::StaticClass()));
	check(movementComponent);

	movementComponent->MaxSpeed = maxSpeed;
	movementComponent->bConstrainToPlane = true;
	movementComponent->bSnapToPlaneAtStart = true;
	movementComponent->SetPlaneConstraintOrigin(FVector(0, 0, 300));
	movementComponent->SetPlaneConstraintNormal(FVector(0, 0, 1));
}

bool AProjectileBase::IsTargetLocationReached()
{
	FVector deltaVec = targetPoint - GetActorLocation();
	float deltaSqr = deltaVec.Size2D();

	return deltaSqr <= FMath::Square(reachibilityTestRadius);
}

void AProjectileBase::BeginProjectileDeath()
{
}

void AProjectileBase::FinishProjectileDeath()
{
	Destroy();
}

void AProjectileBase::KillProjectile()
{
	BeginProjectileDeath();
	bIsProjectileAlive = false;
	movementComponent->MaxSpeed = 0;
	FinishProjectileDeath();
}

void AProjectileBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AUnitBase* unit = Cast<AUnitBase>(OtherActor);
	if (!unit)
	{
		return;
	}

	AUnitBase* instigator = GetInstigator<AUnitBase>();

	if (!UZombieSiegeUtils::AreEnemies(instigator, unit))
	{
		return;
	}

	float damage = FMath::RandRange(impactDamageMinMax.X, impactDamageMinMax.Y);
	
	FDamageInstance damageInstance(instigator, damage, weaponInfo);
	unit->ReceiveDamage(damageInstance);

	if (bDiesOnCollision)
	{
		KillProjectile();
	}
}

void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsProjectileAlive)
	{
		if (IsTargetLocationReached())
		{
			KillProjectile();
		}
	}
}

void AProjectileBase::MoveTowards(const FVector& targetPointArg)
{
	targetPoint = targetPointArg;

	FVector direction = targetPoint - GetActorLocation();
	direction.Z = 0;
	ensure(direction.Normalize());

	movementComponent->Velocity = direction * maxSpeed;
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

