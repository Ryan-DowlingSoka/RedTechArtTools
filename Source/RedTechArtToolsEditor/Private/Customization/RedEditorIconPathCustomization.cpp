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

#include "Customization/RedEditorIconPathCustomization.h"
#include "DetailWidgetRow.h"
#include "RedEditorIconWidget.h"
#include "RedDeveloperSettings.h"
#include "SSearchableComboBox.h"
#include "Brushes/SlateImageBrush.h"

#define LOCTEXT_NAMESPACE "RedEditorIconCustomization"

TSharedRef<IPropertyTypeCustomization> FRedEditorIconPathCustomization::MakeInstance()
{
	// Create the instance and returned a SharedRef
	return MakeShareable(new FRedEditorIconPathCustomization());
}

void FRedEditorIconPathCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
                                                      class FDetailWidgetRow& HeaderRow,
                                                      IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	void* StructData = nullptr;
	const FPropertyAccess::Result Result = StructPropertyHandle.Get().GetValueData(StructData);
	if (Result == FPropertyAccess::Success)
	{
		check(StructData);
		FRedEditorIconPath* CurrentValue = static_cast<FRedEditorIconPath*>(StructData);
		const FString CurrentValuePath = CurrentValue->Path;
		const FString ItemName = FPaths::GetCleanFilename(*CurrentValuePath);
		const auto CurrentSelectionText = SNew(STextBlock)
			.Text_Lambda([=]() { return FText::FromString(FPaths::GetCleanFilename(CurrentValue->Path)); });
		HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()]
			.ValueContent()
			[
				SNew(SSearchableComboBox)
			.OptionsSource(GetIconOptionsPointer())
			.OnGenerateWidget(this, &FRedEditorIconPathCustomization::HandleGenerateWidget)
			.OnSelectionChanged_Lambda([=](TSharedPtr<FString> NewChoice, ESelectInfo::Type SelectType)
				                         {
					                         if (NewChoice.IsValid())
					                         {
					                         	const FScopedTransaction Transaction(LOCTEXT("SetEditorIcon", "Set Editor Icon Path"));
						                         StructPropertyHandle.Get().NotifyPreChange();
						                         CurrentValue->Path = *NewChoice;
						                         StructPropertyHandle.Get().NotifyPostChange(
							                         EPropertyChangeType::ValueSet);
					                         }
				                         })
			.ToolTip(SNew(SToolTip).Text_Lambda([=]()
				                         {
					                         return FText::FromString(
						                         FPaths::ConvertRelativePathToFull(CurrentValue->Path));
				                         }))
			.Content()
				[
					CurrentSelectionText
				]
			];
	}
}

TSharedRef<SWidget> FRedEditorIconPathCustomization::HandleGenerateWidget(TSharedPtr<FString> InItem)
{
	const int EngineIndex = InItem->Find(TEXT("/Engine/"));
	const int EditorIndex = InItem->Find(TEXT("/Editor/"));
	int RightChopIndex = 0;
	if (EngineIndex >= 0 || EditorIndex >= 0)
	{
		RightChopIndex = FMath::Max(EngineIndex + 7, EditorIndex + 7);
	}

	static FVector2D IconSize = FVector2D(24, 24);
	const FString AbsolutePath = FPaths::ConvertRelativePathToFull(*InItem);
	const FString ItemName = FPaths::GetCleanFilename(*InItem);
	const FString Category = InItem->RightChop(RightChopIndex).LeftChop(ItemName.Len() + 1).
	                                 Replace(TEXT("/Content/"),TEXT("/")).
	                                 Replace(TEXT("/Slate/"),TEXT("/")).
	                                 Replace(TEXT("/Plugins/"),TEXT("/")).
	                                 Replace(TEXT("/Experimental/"),TEXT("/")).RightChop(1);

	if (!InItem->IsEmpty() && IFileManager::Get().FileExists(*AbsolutePath))
	{
		TUniquePtr<FSlateBrush>& Item = GeneratedBrushes.FindOrAdd(*InItem);
		if (!Item.IsValid())
		{
			if (const FString Ext = FPaths::GetExtension(*InItem); Ext == "svg")
			{
				Item.Reset(new FSlateVectorImageBrush(*InItem, IconSize));
			}
			else if (Ext == "png")
			{
				Item.Reset(new FSlateDynamicImageBrush(FName(**InItem), IconSize));
			}
			else
			{
				return SNew(STextBlock).Text(FText::FromString(FString("Path is not a .png or .svg file.")));
			}
		}
		return SNew(SHorizontalBox)
			.ToolTip(SNew(SToolTip).Text(FText::FromString(*AbsolutePath)))
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(SImage)
				.Image(Item.Get())
			]
			+ SHorizontalBox::Slot()
			  .AutoWidth()
			  .Padding(16.0f, 1.0f)
			  .VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
			.Text(FText::FromString(ItemName))
			.MinDesiredWidth(200.0f)
			]
			+ SHorizontalBox::Slot()
			  .FillWidth(1.0f)
			  .Padding(16.0f, 1.0f, 0.0f, 1.0f)
			  .VAlign(EVerticalAlignment::VAlign_Center)
			  .HAlign(EHorizontalAlignment::HAlign_Right)
			[
				SNew(STextBlock)
			.Text(FText::FromString(Category))
			.MinDesiredWidth(300.0f)
			.ColorAndOpacity(FLinearColor(0.5, 0.5, 0.5, 0.5))
			.Justification(ETextJustify::Right)
			];
	}

	return SNew(STextBlock).Text(FText::FromString(FString("Path doesn't exist.")));
}

void FRedEditorIconPathCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
                                                        class IDetailChildrenBuilder& StructBuilder,
                                                        IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

TArray<FString> FRedEditorIconPathCustomization::GetIconOptionsFromPath()
{
	TArray<FString> OutFoundIcons;
	const URedDeveloperSettings* RedSettings = GetDefault<URedDeveloperSettings>();
	for (const FString& Path : RedSettings->EditorIconWidgetSearchPaths)
	{
		GetIconsFromPath(Path, OutFoundIcons);
	}
	return OutFoundIcons;
}

TArray<TSharedPtr<FString>>* FRedEditorIconPathCustomization::GetIconOptionsPointer()
{
	if (CachedIconOptions.Num() == 0)
	{
		Algo::Transform(GetIconOptionsFromPath(), CachedIconOptions, [](const FString& InString)
		{
			return MakeShared<FString>(InString);
		});
	}

	return &CachedIconOptions;
}


void FRedEditorIconPathCustomization::GetIconsFromPath(const FString& InPath, TArray<FString>& OutFoundIcons)
{
	const FString SearchDirectory = FPaths::EngineDir() / InPath;
	IFileManager::Get().FindFilesRecursive(OutFoundIcons, *SearchDirectory, TEXT("*.png"), true, false, false);
	IFileManager::Get().FindFilesRecursive(OutFoundIcons, *SearchDirectory, TEXT("*.svg"), true, false, false);
}

#undef LOCTEXT_NAMESPACE