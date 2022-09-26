// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingInstance.h"

FHealingInstance::FHealingInstance()
{
}

FHealingInstance::FHealingInstance(AUnitBase* source, float amount)
{
	this->source = source;
	this->amount = amount;

	check(this->amount >= 0);
}

FHealingInstance::~FHealingInstance()
{
}
