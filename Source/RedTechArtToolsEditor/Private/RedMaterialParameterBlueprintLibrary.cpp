// MIT License
//
// Copyright (c) 2022 Ryan DowlingSoka
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "RedMaterialParameterBlueprintLibrary.h"

#include "Materials/MaterialExpressionCurveAtlasRowParameter.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Editor.h"
#include "IContentBrowserSingleton.h"
#include "IMaterialEditor.h"
#include "MaterialEditorUtilities.h"
#include "Materials/MaterialExpressionCollectionParameter.h"
#include "Materials/MaterialParameterCollection.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Toolkits/ToolkitManager.h"

bool URedMaterialParameterBlueprintLibrary::OpenAndFocusMaterialExpression(UMaterialExpression* MaterialExpression)
{
	if (UObject* OwningObject = GetMaterialExpression_OwningObject(MaterialExpression))
	{
		if (auto* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			if (AssetEditorSubsystem->OpenEditorForAsset(OwningObject))
			{
				if (const auto MaterialEditorInstance = StaticCastSharedPtr<IMaterialEditor>(
					FToolkitManager::Get().FindEditorForAsset(OwningObject)))
				{
					MaterialEditorInstance->FocusWindow(OwningObject);
					if(IsValid(MaterialExpression->GraphNode) || MaterialExpression->bIsParameterExpression)
					{
						MaterialEditorInstance->JumpToExpression(MaterialExpression);
					}
				}
			}
		}
	}
	return false;
}

UObject* URedMaterialParameterBlueprintLibrary::GetMaterialExpression_OwningObject(
	UMaterialExpression* MaterialExpression)
{
	if (IsValid(MaterialExpression))
	{
		if (IsValid(MaterialExpression->Function))
		{
			return MaterialExpression->Function;
		}
		else if (IsValid(MaterialExpression->Material))
		{
			return MaterialExpression->Material;
		}
	}
	return nullptr;
}

TArray<UMaterialExpression*> URedMaterialParameterBlueprintLibrary::GetAllMaterialParameterExpressions(
	UMaterialInterface* MaterialInterface, bool bExcludeFunctions /*=false*/)
{
	TArray<const UMaterialExpression*> ConstAllExpressions;
	TArray<UMaterialExpression*> AllExpressions;
	TArray<UMaterialExpression*> OutExpressions;

	if (bExcludeFunctions)
	{
		MaterialInterface->GetBaseMaterial()->GetAllExpressionsOfType<UMaterialExpression>(
			ConstAllExpressions);
	}
	else
	{
		MaterialInterface->GetBaseMaterial()->GetAllExpressionsInMaterialAndFunctionsOfType<UMaterialExpression>(
			AllExpressions);
	}

	for (auto* Expression : AllExpressions)
	{
		if (Expression->bIsParameterExpression)
		{
			OutExpressions.Add(Expression);
		}
		else if(Expression->IsA(UMaterialExpressionCollectionParameter::StaticClass()))
		{
			OutExpressions.Add(Expression);
		}
	}
	for (auto* Expression : ConstAllExpressions)
	{
		if (Expression->bIsParameterExpression)
		{
			OutExpressions.Add(const_cast<UMaterialExpression*>(Expression));
		}
		else if(Expression->IsA(UMaterialExpressionCollectionParameter::StaticClass()))
		{
			OutExpressions.Add(const_cast<UMaterialExpression*>(Expression));
		}
	}
	return OutExpressions;
}

TArray<UMaterialExpression*> URedMaterialParameterBlueprintLibrary::GetAllMaterialParameterExpressionsInFunction(
	UMaterialFunction* MaterialFunction, bool bExcludeContainedFunctions)
{
	TArray<UMaterialExpression*> AllExpressions;
	TArray<UMaterialExpression*> OutExpressions;
	MaterialFunction->GetAllExpressionsOfType<UMaterialExpression>(
		AllExpressions, !bExcludeContainedFunctions);
	for (auto* Expression : AllExpressions)
	{
		if (Expression->bIsParameterExpression)
		{
			OutExpressions.Add(Expression);
		}
		else if(Expression->IsA(UMaterialExpressionCollectionParameter::StaticClass()))
		{
			OutExpressions.Add(Expression);
		}
	}
	return OutExpressions;
}

FRedMaterialParameterInfo URedMaterialParameterBlueprintLibrary::GetMaterialParameterInfo(
	UMaterialExpression* ParameterExpression)
{
	return FRedMaterialParameterInfo(ParameterExpression);
}

UMaterialExpression* URedMaterialParameterBlueprintLibrary::BreakMaterialParameterInfo(
	const FRedMaterialParameterInfo& Info, FName& Name, FName& Group, FString& Description, int32& SortPriority)
{
	Name = GetMaterialParameter_Name(Info);
	Group = GetMaterialParameter_Group(Info);
	Description = GetMaterialParameter_Description(Info);
	SortPriority = GetMaterialParameter_SortPriority(Info);
	return GetMaterialParameter_OwningMaterialExpression(Info);
}

UMaterialExpression* URedMaterialParameterBlueprintLibrary::GetMaterialParameter_OwningMaterialExpression(
	const FRedMaterialParameterInfo& Info)
{
	return Info.OwningMaterialExpression.Get();
}


FName URedMaterialParameterBlueprintLibrary::GetMaterialParameter_Name(const FRedMaterialParameterInfo& Info)
{
	return Info.ParameterName;
}

FName URedMaterialParameterBlueprintLibrary::GetMaterialParameter_TypeName(const FRedMaterialParameterInfo& Info)
{
	switch (Info.ParameterType)
	{
	case EMaterialParameterType::Scalar:
		return FName("Scalar");
	case EMaterialParameterType::Vector:
		return FName("Vector");
	case EMaterialParameterType::DoubleVector:
		return FName("DoubleVector");
	case EMaterialParameterType::Texture:
		return FName("Texture");
	case EMaterialParameterType::Font:
		return FName("Font");
	case EMaterialParameterType::RuntimeVirtualTexture:
		return FName("RuntimeVirtualTexture");
	case EMaterialParameterType::StaticSwitch:
		return FName("StaticSwitch");
	case EMaterialParameterType::StaticComponentMask:
		return FName("StaticComponentMask");
	default:
		return FName("None");
	}
}

FRedParameterChannelNamesWrapper URedMaterialParameterBlueprintLibrary::GetMaterialParameter_ChannelNames(
	const FRedMaterialParameterInfo& Info)
{
	return FRedParameterChannelNamesWrapper(Info.ParameterInfo.ChannelNames);
}

FName URedMaterialParameterBlueprintLibrary::GetMaterialParameter_Group(const FRedMaterialParameterInfo& Info)
{
	return Info.ParameterInfo.Group;
}

FString URedMaterialParameterBlueprintLibrary::GetMaterialParameter_Description(const FRedMaterialParameterInfo& Info)
{
	return Info.ParameterInfo.Description;
}

int32 URedMaterialParameterBlueprintLibrary::GetMaterialParameter_SortPriority(const FRedMaterialParameterInfo& Info)
{
	return Info.ParameterInfo.SortPriority;
}

void URedMaterialParameterBlueprintLibrary::GetMaterialParameter_ScalarMinMax(const FRedMaterialParameterInfo& Info,
                                                                              float& OutScalarMin, float& OutScalarMax)
{
	OutScalarMin = Info.ParameterInfo.ScalarMin;
	OutScalarMax = Info.ParameterInfo.ScalarMax;
}

FGuid URedMaterialParameterBlueprintLibrary::GetMaterialParameter_ExpressionGUID(const FRedMaterialParameterInfo& Info)
{
	return Info.ParameterInfo.ExpressionGuid;
}

bool URedMaterialParameterBlueprintLibrary::GetMaterialParameter_UsedAsAtlasPosition(
	const FRedMaterialParameterInfo& Info)
{
	return Info.ParameterInfo.bUsedAsAtlasPosition;
}

bool URedMaterialParameterBlueprintLibrary::GetMaterialParameter_UsedAsChannelMask(
	const FRedMaterialParameterInfo& Info)
{
	return Info.ParameterInfo.bUsedAsChannelMask;
}

bool URedMaterialParameterBlueprintLibrary::GetMaterialParameter_Override(const FRedMaterialParameterInfo& Info)
{
	return Info.ParameterInfo.bOverride;
}

TSoftObjectPtr<UCurveLinearColor> URedMaterialParameterBlueprintLibrary::GetMaterialParameter_ScalarCurve(
	const FRedMaterialParameterInfo& Info)
{
	return Info.ParameterInfo.ScalarCurve;
}

TSoftObjectPtr<UCurveLinearColorAtlas> URedMaterialParameterBlueprintLibrary::GetMaterialParameter_ScalarAtlas(
	const FRedMaterialParameterInfo& Info)
{
	return Info.ParameterInfo.ScalarAtlas;
}

void URedMaterialParameterBlueprintLibrary::SetMaterialParameter_Name(FRedMaterialParameterInfo& Info, FName Name)
{
	Info.ParameterName = Name;
	if (auto* Expression = Info.OwningMaterialExpression.Get())
	{
		Expression->Modify();
		Expression->SetParameterName(Name);
	}
}

void URedMaterialParameterBlueprintLibrary::SetMaterialParameter_ChannelNames(FRedMaterialParameterInfo& Info,
                                                                              FRedParameterChannelNamesWrapper
                                                                              ChannelNamesWrapper)
{
	Info.ParameterInfo.ChannelNames = ChannelNamesWrapper.ToParameterChannelNames();
	if (auto* Expression = Cast<UMaterialExpressionVectorParameter>(Info.OwningMaterialExpression.Get()))
	{
		Expression->Modify();
		Expression->ChannelNames = Info.ParameterInfo.ChannelNames;
	}
	if (auto* Expression = Cast<UMaterialExpressionTextureSampleParameter>(Info.OwningMaterialExpression.Get()))
	{
		Expression->Modify();
		Expression->ChannelNames = Info.ParameterInfo.ChannelNames;
	}
}

void URedMaterialParameterBlueprintLibrary::SetMaterialParameter_Group(FRedMaterialParameterInfo& Info,
                                                                       FName Group)
{
	Info.ParameterInfo.Group = Group;
	if (auto* Expression = Info.OwningMaterialExpression.Get())
	{
		Expression->Modify();
		Expression->SetParameterValue(Info.ParameterName, Info.ParameterInfo,
		                              EMaterialExpressionSetParameterValueFlags::NoUpdateExpressionGuid |
		                              EMaterialExpressionSetParameterValueFlags::AssignGroupAndSortPriority);
	}
}

void URedMaterialParameterBlueprintLibrary::SetMaterialParameter_Description(FRedMaterialParameterInfo& Info,
                                                                             FString Description)
{
	Info.ParameterInfo.Description = Description;
	if (auto* Expression = Info.OwningMaterialExpression.Get())
	{
		Expression->Modify();
		Expression->Desc = Description;
	}
}

void URedMaterialParameterBlueprintLibrary::SetMaterialParameter_SortPriority(FRedMaterialParameterInfo& Info,
                                                                              int32 SortPriority)
{
	Info.ParameterInfo.SortPriority = SortPriority;
	if (auto* Expression = Info.OwningMaterialExpression.Get())
	{
		Expression->Modify();
		Expression->SetParameterValue(Info.ParameterName, Info.ParameterInfo,
		                              EMaterialExpressionSetParameterValueFlags::NoUpdateExpressionGuid |
		                              EMaterialExpressionSetParameterValueFlags::AssignGroupAndSortPriority);
	}
}

void URedMaterialParameterBlueprintLibrary::SetMaterialParameter_ScalarMinMax(FRedMaterialParameterInfo& Info,
                                                                              float ScalarMin, float ScalarMax)
{
	Info.ParameterInfo.ScalarMin = ScalarMin;
	Info.ParameterInfo.ScalarMin = ScalarMax;
	if (auto* Expression = Cast<UMaterialExpressionScalarParameter>(Info.OwningMaterialExpression.Get()))
	{
		Expression->Modify();
		Expression->SliderMin = ScalarMin;
		Expression->SliderMax = ScalarMax;
	}
}

void URedMaterialParameterBlueprintLibrary::UpdateMaterialParameter_ExpressionGUID(FRedMaterialParameterInfo& Info)
{
	if (auto* Expression = Info.OwningMaterialExpression.Get())
	{
		Expression->Modify();
		Expression->UpdateParameterGuid(true, true);
		Info.ParameterInfo.ExpressionGuid = Expression->GetParameterExpressionId();
	}
}

void URedMaterialParameterBlueprintLibrary::SetMaterialParameter_ScalarCurve(FRedMaterialParameterInfo& Info,
                                                                             UCurveLinearColor* ScalarCurve)
{
	if (auto* Expression = Cast<UMaterialExpressionCurveAtlasRowParameter>(Info.OwningMaterialExpression.Get()))
	{
		Expression->Modify();
		Expression->Curve = ScalarCurve;
	}
}

void URedMaterialParameterBlueprintLibrary::SetMaterialParameter_ScalarAtlas(FRedMaterialParameterInfo& Info,
                                                                             UCurveLinearColorAtlas* ScalarAtlas)
{
	if (auto* Expression = Cast<UMaterialExpressionCurveAtlasRowParameter>(Info.OwningMaterialExpression.Get()))
	{
		Expression->Modify();
		Expression->Atlas = ScalarAtlas;
	}
}
