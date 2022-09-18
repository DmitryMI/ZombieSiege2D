// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSiegeUtilsTest.h"
#include "ZombieSiegeUtils.h"

bool UZombieSiegeUtilsTest::TestInsertSortedAscending()
{
	TArray<int> arr;
	int value = 1;
	UZombieSiegeUtils::InsertSortedAscending(arr, value, [](int a, int b) {return UZombieSiegeUtils::DefaultComparator(a, b);});

	if (arr.Num() != 1 && arr[0] != value)
	{
		return false;
	}

	arr = {1, 2, 3, 4};
	TArray<int> expected = { 0, 1, 2, 3, 4 };
	value = 0;
	UZombieSiegeUtils::InsertSortedAscending(arr, value, [](int a, int b) {return UZombieSiegeUtils::DefaultComparator(a, b); });

	if (!UZombieSiegeUtils::CompareArrays(arr, expected))
	{
		return false;
	}

	arr = { 1, 2, 3, 4 };
	expected = { 1, 2, 3, 4, 5 };
	value = 5;
	UZombieSiegeUtils::InsertSortedAscending(arr, value, [](int a, int b) {return UZombieSiegeUtils::DefaultComparator(a, b); });

	if (!UZombieSiegeUtils::CompareArrays(arr, expected))
	{
		return false;
	}

	arr = { 1, 2, 4 };
	expected = { 1, 2, 3, 4 };
	value = 3;
	UZombieSiegeUtils::InsertSortedAscending(arr, value, [](int a, int b) {return UZombieSiegeUtils::DefaultComparator(a, b); });

	if (!UZombieSiegeUtils::CompareArrays(arr, expected))
	{
		return false;
	}

	arr.Empty();
	expected.Empty();

	for (int i = 0; i < 100; i++)
	{
		value = FMath::Rand() % 10;
		UZombieSiegeUtils::InsertSortedAscending(arr, value, [](int a, int b) {return UZombieSiegeUtils::DefaultComparator(a, b); });
		expected.Add(value);
	}
	expected.Sort();

	if (!UZombieSiegeUtils::CompareArrays(arr, expected))
	{
		return false;
	}

	return true;
}
