// Fill out your copyright notice in the Description page of Project Settings.


#include "Crosshair.h"

void UCrosshair::Hover()
{
	IconCrosshair->SetRenderOpacity(1.f);
	IsHover = true;
}

void UCrosshair::UnHover()
{
	IconCrosshair->SetRenderOpacity(.3f);
	IsHover = false;
}
