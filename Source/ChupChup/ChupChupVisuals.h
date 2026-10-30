#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace ChupChupVisuals
{
	/** Engine basic shapes (/Engine/BasicShapes/*) use BasicShapeMaterial, which exposes a "Color" vector parameter. */
	inline UMaterialInstanceDynamic* ApplyColor(UPrimitiveComponent* Component, const FLinearColor& Color)
	{
		if (Component == nullptr)
		{
			return nullptr;
		}

		UMaterialInstanceDynamic* Material = Component->CreateDynamicMaterialInstance(0);
		if (Material != nullptr)
		{
			Material->SetVectorParameterValue(TEXT("Color"), Color);
		}
		return Material;
	}
}
