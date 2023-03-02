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

#include "RedMaterialExpression_IsEditorStaticBool.h"

#if WITH_EDITOR
#include "Editor.h"
#include "MaterialCompiler.h"
#include "MaterialHLSLGenerator.h"
#endif

#define LOCTEXT_NAMESPACE "RedMaterialExpression_IsEditorStaticBool"

//
//	URedMaterialExpression_IsEditorStaticBool
//
URedMaterialExpression_IsEditorStaticBool::URedMaterialExpression_IsEditorStaticBool(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FText NAME_Functions;
		FConstructorStatics()
			: NAME_Functions(LOCTEXT( "Functions", "Functions" ))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	bHidePreviewWindow = true;

	MenuCategories.Add(ConstructorStatics.NAME_Functions);

	bShaderInputData = true;
#endif
}

#if WITH_EDITOR
int32 URedMaterialExpression_IsEditorStaticBool::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	const bool bIsCooking = IsRunningCookCommandlet() || IsRunningCookOnTheFly();
	return Compiler->StaticBool(!bIsCooking);
}

int32 URedMaterialExpression_IsEditorStaticBool::CompilePreview(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	return INDEX_NONE;
}

void URedMaterialExpression_IsEditorStaticBool::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString(TEXT("Is Editor")));
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
