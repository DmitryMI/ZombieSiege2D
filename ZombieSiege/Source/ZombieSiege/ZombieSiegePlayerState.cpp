// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSiegePlayerState.h"

#define CHECK_RESOURCE_VALID(resourceType) check(resourceType != EResourceType::RESOURCETYPE_MAX);

float AZombieSiegePlayerState::GetStoredResourceAmount(EResourceType type)
{
	CHECK_RESOURCE_VALID(type);

	int value = 0;

	if (storedResources.Contains(type))
	{
		value = storedResources[type];
	}
	else
	{
		storedResources.Add(type, 0);		
	}

	return value;
}

void AZombieSiegePlayerState::AddResourceToStorage(EResourceType type, float amount)
{
	CHECK_RESOURCE_VALID(type);

	float currentAmount = GetStoredResourceAmount(type);
	SetStoredResourceAmount(type, currentAmount + amount);
}

bool AZombieSiegePlayerState::TakeResourceFromStorage(EResourceType type, float amount)
{
	CHECK_RESOURCE_VALID(type);

	float currentAmount = GetStoredResourceAmount(type);
	if (currentAmount < amount)
	{
		return false;
	}

	SetStoredResourceAmount(type, currentAmount - amount);
	return true;
}

bool AZombieSiegePlayerState::TakeResourcesFromStorage(TMap<EResourceType, float> resources)
{
	if (!HasEnoughResourcesInStorage(resources))
	{
		return false;
	}

	for (auto resourcePair : resources)
	{
		EResourceType resource = resourcePair.Key;
		float amount = resourcePair.Value;
		bool resourceTaken = TakeResourceFromStorage(resource, amount);
		check(resourceTaken);
	}

	return true;
}

bool AZombieSiegePlayerState::HasEnoughResourcesInStorage(TMap<EResourceType, float> resources)
{
	for (auto resourcePair : resources)
	{
		float stored = GetStoredResourceAmount(resourcePair.Key);
		if (stored < resourcePair.Value)
		{
			return false;
		}
	}
	return true;
}

void AZombieSiegePlayerState::SetStoredResourceAmount(EResourceType type, float amount)
{
	CHECK_RESOURCE_VALID(type);

	// This function can set negative amounts. May be useful in future.

	if (storedResources.Contains(type))
	{
		storedResources[type] = amount;
	}
	else
	{
		storedResources.Add(type, amount);
	}
}
