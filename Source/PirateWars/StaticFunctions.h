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


	/** Calculate y coordinate of a line, parametrized by collinear vector and origin */
	UFUNCTION(BlueprintCallable, Category = "Math")
	static float FindYByPointAndVectorOfALine(FVector2D CollinearVector, FVector2D OriginPoint, float XCoord);

	/** Calculate y coordinate of a line, parametrized by collinear vector and origin */
	UFUNCTION(BlueprintCallable, Category = "Math")
	static bool TargetedPointIsOnLeftSideOfTheLine(FVector2D CollinearVector, FVector2D OriginPointOfTheLine, FVector2D TargetedPoint);

	/** Finds the angle between two 2D points. */
	UFUNCTION(BlueprintPure, Category = "Math|Rotator", meta = (Keywords = "rotation rotate"))
	static bool FindLookAtAngle2D(const FVector2D& Start, const FVector2D& Target, float& Angle);

};
