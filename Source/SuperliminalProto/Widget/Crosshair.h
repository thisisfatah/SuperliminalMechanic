// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include <Components/HorizontalBox.h>
#include <Components/Image.h>
#include "Crosshair.generated.h"

/**
 * 
 */
UCLASS()
class SUPERLIMINALPROTO_API UCrosshair : public UUserWidget
{
	GENERATED_BODY()
	
public:
	bool IsHover;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* IconCrosshair;

public:
	void Hover();
	void UnHover();
};
