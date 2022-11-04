// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeaponInfo.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

AProjectileBase* UProjectileWeaponInfo::SpawnProjectile(AUnitBase* instigator, const FAttackParameters& params)
{
	UWorld* world = GetWorld();
	
	FVector location = instigator->GetActorLocation() + params.projectileSpawnRelativeLocation;

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = instigator;

	AProjectileBase* projectile = world->SpawnActor<AProjectileBase>(projectileClass, location, FRotator::ZeroRotator, spawnParameters);

	if (!projectile)
	{
		return nullptr;
	}

	projectile->SetImpactDamageRange(damageMinMax);
	projectile->SetWeaponInfo(this);

	if (bOverrideProjectileMaxSpeed)
	{
		projectile->SetMaxSpeed(overrideProjectileMaxSpeed);
	}

	return projectile;
}

UProjectileWeaponInfo::UProjectileWeaponInfo()
{
	bCanEverAttackPoint = true;
}

void UProjectileWeaponInfo::AttackTarget(AUnitBase* attacker, AActor* target, const FAttackParameters& params)
{
	float projectileMaxSpeed;	
	if (bOverrideProjectileMaxSpeed)
	{
		projectileMaxSpeed = overrideProjectileMaxSpeed;		
	}
	else
	{
		projectileMaxSpeed = projectileClass.GetDefaultObject()->GetMaxSpeed();
	}

	float projectileCollisionRadius = projectileClass.GetDefaultObject()->GetSimpleCollisionRadius();	

	AActor* targetActor = Cast<AActor>(target);
	if (!targetActor)
	{
		return;
	}

	FVector targetVelocity = targetActor->GetVelocity();

	FVector targetLocation = targetActor->GetActorLocation();
	FVector launchLocation = attacker->GetActorLocation() + params.projectileSpawnRelativeLocation;
	FVector projectileVelocity;
	FVector interceptionLocation;
	float interceptionTime;

	bool ok = SuggestProjectileVelocity(
		targetLocation,
		targetVelocity,
		launchLocation,
		projectileMaxSpeed,
		interceptionTime,
		interceptionLocation,
		projectileVelocity);

	FAttackParameters paramsClone = params;

	// TODO Make proper 3D prediction!
	interceptionLocation.Z = targetLocation.Z;

	if (ok)
	{
		FVector suggestedArcVelocity;
		bool arcSuggestOk = UGameplayStatics::SuggestProjectileVelocity(
			this,
			suggestedArcVelocity,
			launchLocation,
			interceptionLocation,
			projectileMaxSpeed,
			false,
			projectileCollisionRadius,
			0.0f,
			ESuggestProjVelocityTraceOption::DoNotTrace
		);
		paramsClone.projectileInitialLocalSpaceVelocity = suggestedArcVelocity;

		AttackPoint(attacker, interceptionLocation, paramsClone);
	}
	else
	{
		FVector suggestedArcVelocity;
		bool arcSuggestOk = UGameplayStatics::SuggestProjectileVelocity(
			this,
			suggestedArcVelocity,
			launchLocation,
			targetLocation,
			projectileMaxSpeed,
			false,
			projectileCollisionRadius,
			0.0f,
			ESuggestProjVelocityTraceOption::DoNotTrace
		);
		paramsClone.projectileInitialLocalSpaceVelocity = suggestedArcVelocity;

		AttackPoint(attacker, targetActor->GetActorLocation(), paramsClone);
	}
}

void UProjectileWeaponInfo::AttackPoint(AUnitBase* attacker, const FVector& targetPoint, const FAttackParameters& params)
{
	if (!CanAttackPoint(attacker, targetPoint))
	{
		return;
	}

	AProjectileBase* projectile = SpawnProjectile(attacker, params);
	ensure(projectile);
	if (!projectile)
	{
		return;
	}

	FVector vectorToTarget = (targetPoint - projectile->GetActorLocation());

	float scatter = FMath::RandRange(-scatterAngleDeg, scatterAngleDeg);
	vectorToTarget = vectorToTarget.RotateAngleAxis(scatter, FVector::UpVector);
	vectorToTarget *= 1 + shootBehindTargetFactor;

	FVector targetPointMod = projectile->GetActorLocation() + vectorToTarget;

	// TODO Is it really local space velocity?
	projectile->MoveTowards(targetPointMod, params.projectileInitialLocalSpaceVelocity);
}

bool UProjectileWeaponInfo::CanAttackTarget(AUnitBase* attacker, AActor* target)
{
	if (!CanThisWeaponEverAttackTarget(target))
	{
		return false;
	}

	AActor* targetActor = Cast<AActor>(target);
	if (!targetActor)
	{
		return false;
	}

	return CanAttackPoint(attacker, targetActor->GetActorLocation());
}

bool UProjectileWeaponInfo::CanAttackPoint(AUnitBase* attacker, const FVector& targetPoint)
{
	if (!CanThisWeaponEverAttackPoint())
	{
		return false;
	}

	float distance = (attacker->GetActorLocation() - targetPoint).Size2D();

	if (distance > range)
	{
		return false;
	}

	return true;
}

bool UProjectileWeaponInfo::SuggestProjectileVelocity
(
	const FVector& targetLocation,
	const FVector& targetVelocity,
	const FVector& launchLocation,
	float projectileSpeed,
	float& outInterceptionTime,
	FVector& outInterceptionLocation,
	FVector& outLaunchVelocity
)
{
#define TX(vx) (dx / (vx - targetVelocity.X))
#define VY(vx, t) (dy / t + targetVelocity.Y)

	float speed2 = projectileSpeed * projectileSpeed;

	float xt = targetLocation.X;
	float yt = targetLocation.Y;
	float xp = launchLocation.X;
	float yp = launchLocation.Y;

	float dx = xt - xp;
	float dy = yt - yp;

	float dx2 = dx * dx;
	float dy2 = dy * dy;

	float invTx = targetVelocity.X / dx;
	float invTy = targetVelocity.Y / dy;
	float dInvT = invTx - invTy;
	float dInvT2 = dInvT * dInvT;

	//float s = projectileSpeed * projectileSpeed / dy2 - dInvT2;

	float a = 1.0f / dx2 + 1.0f / dy2;
	float b = -2 * dInvT / dx;
	// float c = -s;
	float c = dInvT2 - speed2 / dy2;
	float disc = b * b - 4 * a * c;

	bool bHasSolution = false;
	float time = -1;
	float velocityX = 0;
	float velocityY = 0;
	
	if (disc == 0)
	{
		velocityX = -b / (2 * a);		
		time = TX(velocityX);
		velocityY = VY(velocityX, time);

		bHasSolution = true;
	}
	else if (disc > 0)
	{
		float sqDisc = FMath::Sqrt(disc);
		float velocityX1 = (-b + sqDisc) / (2 * a);
		time = TX(velocityX1);
		
		if (time > 0)
		{
			velocityX = velocityX1;
			velocityY = VY(velocityX1, time);

			bHasSolution = true;
		}
		else
		{
			velocityX = (-b - sqDisc) / (2 * a);
			time = TX(velocityX);
			velocityY = VY(velocityX, time);

			bHasSolution = true;
		}
	}
	else
	{
		bHasSolution = false;
	}

	if (bHasSolution)
	{
		outLaunchVelocity = FVector(velocityX, velocityY, 0);
		outInterceptionTime = time;
		outInterceptionLocation = launchLocation + outLaunchVelocity * time;

#if WITH_EDITOR
		if (bDebugDrawProjectilePrediction)
		{
			DrawDebugLine(GetWorld(), launchLocation, outInterceptionLocation, FColor::Green, false, time, 0, 2.0f);
			DrawDebugLine(GetWorld(), targetLocation, outInterceptionLocation, FColor::Red, false, time, 0, 2.0f);
		}
#endif

	}

	return bHasSolution;
}
