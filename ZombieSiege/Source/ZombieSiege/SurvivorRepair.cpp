// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorRepair.h"
#include "Building.h"
#include "HealingInstance.h"
#include "ZombieSiegePlayerState.h"
#include "ZombieSiegeUtils.h"

TMap<EResourceType, float> USurvivorRepair::CalculateRequiredResourceAmount(ABuilding* building, float repairAmount) const
{
	TMap<EResourceType, float> map;

	TMap<EResourceType, float> requiredResources = building->GetRequiredResources();

	float repairFraction = repairAmount / building->GetMaxHealth();

	for (auto requiredResource : requiredResources)
	{
		EResourceType resource = requiredResource.Key;
		float amount = requiredResource.Value;

		if (amount > 0)
		{
			float resultingAmount = repairFraction * amount;
			map.Add(resource, resultingAmount);
		}
	}

	return map;
}

void USurvivorRepair::AttackTarget(AUnitBase* attacker, AActor* target, const FAttackParameters& params)
{
	if (!CanAttackTarget(attacker, target))
	{
		return;
	}

	AZombieSiegePlayerState* playerState = attacker->GetOwningPlayerState();

	check(playerState);

	ABuilding* building = Cast<ABuilding>(target);
	check(building);

	float requiredRepair = FMath::Max(building->GetMaxHealth() - building->GetHealth(), building->GetMaxHealth() - building->GetBuildingProgress());

	float repairAmount = FMath::Min(requiredRepair, damageMinMax.X);

	if (building->IsFullyBuilt())
	{
		TMap<EResourceType, float> requiredResources = CalculateRequiredResourceAmount(building, repairAmount);

		for (auto requiredResource : requiredResources)
		{
			EResourceType resource = requiredResource.Key;
			float amount = requiredResource.Value;

			bool resourceTaken = playerState->TakeResourceFromStorage(resource, amount);
			check(resourceTaken);
		}
	}

	FHealingInstance healingInstance(building, repairAmount);
	building->ReceiveHealing(healingInstance);
}

bool USurvivorRepair::CanAttackTarget(AUnitBase* attacker, AActor* target)
{
	check(attacker);
	check(target);

	ABuilding* building = Cast<ABuilding>(target);
	if (!building)
	{
		return false;
	}

	float distance = UZombieSiegeUtils::GetDistance2DBetweenSimpleCollisions(attacker, building);

	if (distance > range)
	{
		return false;
	}

	AZombieSiegePlayerState* playerState = attacker->GetOwningPlayerState();

	if (!playerState)
	{
		return false;
	}

	float requiredRepair = FMath::Max(building->GetMaxHealth() - building->GetHealth(), building->GetMaxHealth() - building->GetBuildingProgress());
	float repairAmount = FMath::Min(requiredRepair, damageMinMax.X);

	if (!building->NeedsRepair())
	{
		return false;
	}

	if (building->IsFullyBuilt())
	{
		TMap<EResourceType, float> requiredResources = CalculateRequiredResourceAmount(building, repairAmount);

		for (auto requiredResource : requiredResources)
		{
			EResourceType resource = requiredResource.Key;
			float amount = requiredResource.Value;

			if (playerState->GetStoredResourceAmount(resource) < amount)
			{
				return false;
			}
		}
	}

	return true;
}
