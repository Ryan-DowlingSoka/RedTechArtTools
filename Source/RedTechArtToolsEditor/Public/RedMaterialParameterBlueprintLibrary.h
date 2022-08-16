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
#include "Materials/MaterialExpression.h"
#include "RedMaterialParameterBlueprintLibrary.generated.h"

class UCurveLinearColor;
class UCurveLinearColorAtlas;

DECLARE_LOG_CATEGORY_EXTERN(LogRedTechArtToolsBlueprintEditorLib, Warning, All);

USTRUCT(BlueprintType)
struct FRedParameterChannelNamesWrapper
{
	GENERATED_USTRUCT_BODY()

	FRedParameterChannelNamesWrapper() = default;

	FRedParameterChannelNamesWrapper(const FText& InR, const FText& InG, const FText& InB,
	                                 const FText& InA) : R(InR), G(InG), B(InB), A(InA)
	{
	}

	FRedParameterChannelNamesWrapper(const FParameterChannelNames& ChannelNames)
	{
		R = ChannelNames.R;
		G = ChannelNames.G;
		B = ChannelNames.B;
		A = ChannelNames.A;
	}

	FParameterChannelNames ToParameterChannelNames() const
	{
		return FParameterChannelNames(R, G, B, A);
	}


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = MaterialExpressionVectorParameter)
	FText R;

	UPROPERTY(BlueprintReadWrite, Category = MaterialExpressionVectorParameter)
	FText G;

	UPROPERTY(BlueprintReadWrite, Category = MaterialExpressionVectorParameter)
	FText B;

	UPROPERTY(BlueprintReadWrite, Category = MaterialExpressionVectorParameter)
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


/**
 *	Blueprint Library for getting and setting information about Material Parameter Expressions.
 *	Mostly needed because FMaterialParameterMetadata is not a UStruct(BlueprintType).
 */
UCLASS()
class REDTECHARTTOOLSEDITOR_API URedMaterialParameterBlueprintLibrary : public UBlueprintEditorLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static UPARAM(DisplayName="Success?")bool OpenAndFocusMaterialExpression(UMaterialExpression* MaterialExpression);

	/** Gets the UObject that contains this material expression, could be a UMaterial* or a UMaterialFunction* */
	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static UObject* GetMaterialExpression_OwningObject(UMaterialExpression* MaterialExpression);

	/**
	 * Gets all the material parameter expressions from the base material and their contained functions.
	 * bExcludeFunctions will only get the expressions from the base material and ignore all function
	 * parameters.
	 */
	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static TArray<UMaterialExpression*> GetAllMaterialParameterExpressions(
		UMaterialInterface* MaterialInterface, bool bExcludeFunctions = false);

	/** Gets all material parameters in this function.
	 * bExcludeContainedFunctions will only get the parameters from this function and not recurse into children functions.
	 */
	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static TArray<UMaterialExpression*> GetAllMaterialParameterExpressionsInFunction(
		UMaterialFunction* MaterialFunction, bool bExcludeContainedFunctions = false);

	/** Gets the MaterialParameterInfo struct, this stores all the current editable data on the parameter expression,
	 * and provides an interface for changing these properties on the expression.
	 */
	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static FRedMaterialParameterInfo GetMaterialParameterInfo(UMaterialExpression* ParameterExpression);

	/** Gets all commonly used information in the RedMaterialParameterInfo all at once. */
	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static UPARAM(DisplayName="Owning Material Expression") UMaterialExpression* BreakMaterialParameterInfo(
		UPARAM(ref) const FRedMaterialParameterInfo& Info, FName& Name, FName& Group, FString& Description,
		int32& SortPriority);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static UMaterialExpression* GetMaterialParameter_OwningMaterialExpression(
		UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static FName GetMaterialParameter_Name(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static FName GetMaterialParameter_TypeName(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static FRedParameterChannelNamesWrapper GetMaterialParameter_ChannelNames(
		UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static FName GetMaterialParameter_Group(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static FString GetMaterialParameter_Description(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static int32 GetMaterialParameter_SortPriority(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static void GetMaterialParameter_ScalarMinMax(UPARAM(ref) const FRedMaterialParameterInfo& Info,
	                                              float& OutScalarMin, float& OutScalarMax);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static FGuid GetMaterialParameter_ExpressionGUID(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static bool GetMaterialParameter_UsedAsAtlasPosition(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static bool GetMaterialParameter_UsedAsChannelMask(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static bool GetMaterialParameter_Override(UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static TSoftObjectPtr<UCurveLinearColor> GetMaterialParameter_ScalarCurve(
		UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static TSoftObjectPtr<UCurveLinearColorAtlas> GetMaterialParameter_ScalarAtlas(
		UPARAM(ref) const FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static void SetMaterialParameter_Name(UPARAM(ref) FRedMaterialParameterInfo& Info, FName Name);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static void SetMaterialParameter_ChannelNames(UPARAM(ref) FRedMaterialParameterInfo& Info,
	                                              FRedParameterChannelNamesWrapper ChannelNamesWrapper);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static void SetMaterialParameter_Group(UPARAM(ref) FRedMaterialParameterInfo& Info, FName Group);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static void SetMaterialParameter_Description(UPARAM(ref) FRedMaterialParameterInfo& Info, FString Description);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static void SetMaterialParameter_SortPriority(UPARAM(ref) FRedMaterialParameterInfo& Info, int32 SortPriority);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static void SetMaterialParameter_ScalarMinMax(UPARAM(ref) FRedMaterialParameterInfo& Info, float ScalarMin,
	                                              float ScalarMax);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static void UpdateMaterialParameter_ExpressionGUID(UPARAM(ref) FRedMaterialParameterInfo& Info);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static void SetMaterialParameter_ScalarCurve(UPARAM(ref) FRedMaterialParameterInfo& Info,
	                                             UCurveLinearColor* ScalarCurve);

	UFUNCTION(BlueprintCallable, Category=MaterialExpressions)
	static void SetMaterialParameter_ScalarAtlas(UPARAM(ref) FRedMaterialParameterInfo& Info,
	                                             UCurveLinearColorAtlas* ScalarAtlas);
};
