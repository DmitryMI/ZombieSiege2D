// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSiegePlayerState.h"

#define CHECK_RESOURCE_VALID(resourceType) check(resourceType != EResourceType::RESOURCETYPE_MAX);

int AZombieSiegePlayerState::GetStoredResourceAmount(EResourceType type)
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

void AZombieSiegePlayerState::AddResourceToStorage(EResourceType type, int amount)
{
	CHECK_RESOURCE_VALID(type);

	int currentAmount = GetStoredResourceAmount(type);
	SetStoredResourceAmount(type, currentAmount + amount);
}

bool AZombieSiegePlayerState::TakeLumberFromStorage(EResourceType type, int amount)
{
	CHECK_RESOURCE_VALID(type);

	int currentAmount = GetStoredResourceAmount(type);
	if (currentAmount < amount)
	{
		return false;
	}

	SetStoredResourceAmount(type, currentAmount - amount);
	return true;
}

void AZombieSiegePlayerState::SetStoredResourceAmount(EResourceType type, int amount)
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
