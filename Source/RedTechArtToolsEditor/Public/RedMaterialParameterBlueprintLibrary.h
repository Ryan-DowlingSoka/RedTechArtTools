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

#pragma once

#include "CoreMinimal.h"
#include "BlueprintEditorLibrary.h"
#include "MaterialTypes.h"
#include "RedMaterialParameterBlueprintLibrary.generated.h"

class UCurveLinearColor;
class UCurveLinearColorAtlas;

DECLARE_LOG_CATEGORY_EXTERN(LogRedTechArtToolsBlueprintEditorLib, Warning, All);

USTRUCT(BlueprintType)
struct FRedParameterChannelNamesWrapper
{
	GENERATED_USTRUCT_BODY()

	FRedParameterChannelNamesWrapper() = default;
	FRedParameterChannelNamesWrapper(const FText& InR, const FText& InG, const FText& InB, const FText& InA) : R(InR), G(InG), B(InB), A(InA) {}
	FRedParameterChannelNamesWrapper(const FParameterChannelNames& ChannelNames)
	{
		R = ChannelNames.R;
		G = ChannelNames.G;
		B = ChannelNames.B;
		A = ChannelNames.A;
	}

	FParameterChannelNames ToParameterChannelNames() const
	{
		return FParameterChannelNames(R,G,B,A);
	}
	
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = MaterialExpressionVectorParameter)
	FText R;

	UPROPERTY(BlueprintReadWrite,  Category = MaterialExpressionVectorParameter)
	FText G;

	UPROPERTY(BlueprintReadWrite,  Category = MaterialExpressionVectorParameter)
	FText B;

	UPROPERTY(BlueprintReadWrite,  Category = MaterialExpressionVectorParameter)
	FText A;
};

USTRUCT(BlueprintType)
struct REDTECHARTTOOLSEDITOR_API FRedMaterialParameterInfo
{
	GENERATED_BODY()

	FRedMaterialParameterInfo() = default;
	explicit FRedMaterialParameterInfo(UMaterialExpression* InExpression)
	{
		if (!IsValid(InExpression))
			return;

		OwningMaterialExpression = InExpression;
		ParameterName = InExpression->GetParameterName();
		ParameterType = InExpression->GetParameterType();
		InExpression->GetParameterValue(ParameterInfo);
	}
	
	UPROPERTY()
	TWeakObjectPtr<UMaterialExpression> OwningMaterialExpression = nullptr;

	/** Parameter Name for the given material expression. Modify with caution.*/
	UPROPERTY()
	FName ParameterName = NAME_None;
	
	EMaterialParameterType ParameterType = EMaterialParameterType::None;

	FMaterialParameterMetadata ParameterInfo;
};

UCLASS()
class REDTECHARTTOOLSEDITOR_API URedMaterialParameterBlueprintLibrary : public UBlueprintEditorLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void RecompileMaterials(TArray<UMaterialInterface*> Materials);

	UFUNCTION(BlueprintCallable)
	static void RecompileFunctions(TArray<UMaterialFunction*> Functions);
	
	UFUNCTION(BlueprintCallable)
	static UPARAM(DisplayName="Success?")bool OpenAndFocusMaterialExpression(UMaterialExpression* MaterialExpression);

	UFUNCTION(BlueprintCallable)
	static UObject* GetMaterialExpression_OwningObject(UMaterialExpression* MaterialExpression);

	UFUNCTION(BlueprintCallable)
	static TArray<UMaterialExpression*> GetAllMaterialParameterExpressions(UMaterialInterface* MaterialInterface, bool bExcludeFunctions=false);

	UFUNCTION(BlueprintCallable)
	static TArray<UMaterialExpression*> GetAllMaterialParameterExpressionsInFunction(UMaterialFunction* MaterialFunction);
	
	UFUNCTION(BlueprintCallable)
	static FRedMaterialParameterInfo GetMaterialParameterInfo(UMaterialExpression* ParameterExpression);

	/** Gets all commonly used information in the RedMaterialParameterInfo all at once. */
	UFUNCTION(BlueprintCallable)
	static UPARAM(DisplayName="Owning Material Expression") UMaterialExpression* BreakMaterialParameterInfo(UPARAM(ref) const FRedMaterialParameterInfo& Info, FName& Name, FName& Group, FString& Description, int32& SortPriority);
	
	UFUNCTION(BlueprintCallable)
	static UMaterialExpression* GetMaterialParameter_OwningMaterialExpression(UPARAM(ref) const FRedMaterialParameterInfo& Info);
	
	UFUNCTION(BlueprintCallable)
	static FName GetMaterialParameter_Name(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable)
	static FName GetMaterialParameter_TypeName(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable)
	static FRedParameterChannelNamesWrapper GetMaterialParameter_ChannelNames(UPARAM(ref) const FRedMaterialParameterInfo& Info);
	
	UFUNCTION(BlueprintCallable)
	static FName GetMaterialParameter_Group(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable)
	static FString GetMaterialParameter_Description(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable)
	static int32 GetMaterialParameter_SortPriority(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable)
	static void GetMaterialParameter_ScalarMinMax(UPARAM(ref) const FRedMaterialParameterInfo& Info, float& OutScalarMin, float& OutScalarMax);

	UFUNCTION(BlueprintCallable)
	static FGuid GetMaterialParameter_ExpressionGUID(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable)
	static bool GetMaterialParameter_UsedAsAtlasPosition(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable)
	static bool GetMaterialParameter_UsedAsChannelMask(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable)
	static bool GetMaterialParameter_Override(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable)
	static TSoftObjectPtr<UCurveLinearColor> GetMaterialParameter_ScalarCurve(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable)
	static TSoftObjectPtr<UCurveLinearColorAtlas> GetMaterialParameter_ScalarAtlas(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable)
	static void SetMaterialParameter_Name(UPARAM(ref) FRedMaterialParameterInfo& Info, FName Name);

	UFUNCTION(BlueprintCallable)
	static void SetMaterialParameter_ChannelNames(UPARAM(ref) FRedMaterialParameterInfo& Info, FRedParameterChannelNamesWrapper ChannelNamesWrapper);
	
	UFUNCTION(BlueprintCallable)
	static void SetMaterialParameter_Group(UPARAM(ref) FRedMaterialParameterInfo& Info, FName Group);

	UFUNCTION(BlueprintCallable)
	static void SetMaterialParameter_Description(UPARAM(ref) FRedMaterialParameterInfo& Info, FString Description);

	UFUNCTION(BlueprintCallable)
	static void SetMaterialParameter_SortPriority(UPARAM(ref) FRedMaterialParameterInfo& Info, int32 SortPriority);

	UFUNCTION(BlueprintCallable)
	static void SetMaterialParameter_ScalarMinMax(UPARAM(ref) FRedMaterialParameterInfo& Info, float ScalarMin, float ScalarMax);

	UFUNCTION(BlueprintCallable)
	static void UpdateMaterialParameter_ExpressionGUID(UPARAM(ref) FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable)
	static void SetMaterialParameter_ScalarCurve(UPARAM(ref) FRedMaterialParameterInfo& Info, UCurveLinearColor* ScalarCurve);

	UFUNCTION(BlueprintCallable)
	static void SetMaterialParameter_ScalarAtlas(UPARAM(ref) FRedMaterialParameterInfo& Info, UCurveLinearColorAtlas* ScalarAtlas);
};
