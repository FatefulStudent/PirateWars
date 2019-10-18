// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StaticFunctions.generated.h"

/**
 * 
 */
UCLASS()
class PIRATEWARS_API UStaticFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Calculate angle difference in the range [-180, 180]. */
	UFUNCTION(BlueprintCallable, Category = "Math")
	static float FindDeltaAngleDegrees(float A1, float A2);


	/** Calculate y coordinate of the intersect with Oy axis with a line, parametrized by slope and origin point */
	UFUNCTION(BlueprintCallable, Category = "Math")
	static float FindYIntersectBySlopeAndPointOfLine(float SlopeAngle, FVector2D OriginPoint);
	
	/** Calculate pseudoscalar product of two vectors */
	UFUNCTION(BlueprintCallable, Category = "Math")
	static float PseudoScalarMultiplication(FVector2D OriginalVector, FVector2D TargetedVector);

	/** Calculate whether the targeted point is on the left of line parametrized by slope and origin */
	UFUNCTION(BlueprintCallable, Category = "Math")
	static bool TargetedPointIsOnLeftSideOfTheLine(float SlopeAngle, FVector2D OriginPointOfTheLine, FVector2D TargetedPoint);

	/** Finds the angle between two 2D points. */
	UFUNCTION(BlueprintPure, Category = "Math|Rotator", meta = (Keywords = "rotation rotate"))
	static bool FindLookAtAngle2D(const FVector2D& Start, const FVector2D& Target, float& Angle);

	/** Return normal number with loc and sclae */
	UFUNCTION(BlueprintPure, Category = "Math", meta = (Keywords = "rotation rotate"))
	static float GetRandomNormal(float loc = 0, float scale = 10);
};
