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

#include "RedTechArtToolsBlueprintLibrary.h"

#include "IContentBrowserSingleton.h"
#include "Blueprint/UserWidget.h"
#include "Dialogs/DlgPickPath.h"
#include "Engine/Blueprint.h"
#include "Engine/DataTable.h"
#include "GameFramework/GameStateBase.h"
#include "Misc/ConfigCacheIni.h"
#include "UObject/PropertyAccessUtil.h"

#define LOCTEXT_NAMESPACE "RedTechArtToolsBlueprintLibrary"

/** Output entire contents of a DataTable as a string */
FString URedTechArtToolsBlueprintLibrary::GetTableAsString(const UDataTable* DataTable,
                                                           const bool bUseJsonObjectsForStructs,
                                                           const bool bUseSimpleText)
{
	if (IsValid(DataTable))
	{
		EDataTableExportFlags Flags = EDataTableExportFlags::None;
		if (bUseJsonObjectsForStructs)
			Flags = EDataTableExportFlags::UseJsonObjectsForStructs;

		if (bUseSimpleText)
			Flags = Flags | EDataTableExportFlags::UseSimpleText;

		return DataTable->GetTableAsString(Flags);
	}
	return "";
}


/** Output entire contents of DataTable as CSV */
FString URedTechArtToolsBlueprintLibrary::GetTableAsCSV(const UDataTable* DataTable,
                                                        const bool bUseJsonObjectsForStructs, const bool bUseSimpleText)
{
	if (IsValid(DataTable))
	{
		EDataTableExportFlags Flags = EDataTableExportFlags::None;
		if (bUseJsonObjectsForStructs)
			Flags = EDataTableExportFlags::UseJsonObjectsForStructs;

		if (bUseSimpleText)
			Flags = Flags | EDataTableExportFlags::UseSimpleText;

		return DataTable->GetTableAsString(Flags);
	}
	return "";
}


/** Output entire contents of DataTable as JSON */
FString URedTechArtToolsBlueprintLibrary::GetTableAsJSON(const UDataTable* DataTable,
                                                         const bool bUseJsonObjectsForStructs,
                                                         const bool bUseSimpleText)
{
	if (IsValid(DataTable))
	{
		EDataTableExportFlags Flags = EDataTableExportFlags::None;
		if (bUseJsonObjectsForStructs)
			Flags = EDataTableExportFlags::UseJsonObjectsForStructs;

		if (bUseSimpleText)
			Flags = Flags | EDataTableExportFlags::UseSimpleText;

		return DataTable->GetTableAsString(Flags);
	}
	return "";
}

bool URedTechArtToolsBlueprintLibrary::AlphaNumericLessThan(FString& A, FString& B)
{
	return A < B;
}

bool URedTechArtToolsBlueprintLibrary::AlphaNumericLessThanOrEqual(FString& A, FString& B)
{
	return A <= B;
}

bool URedTechArtToolsBlueprintLibrary::AlphaNumericGreaterThan(FString& A, FString& B)
{
	return A > B;
}

bool URedTechArtToolsBlueprintLibrary::AlphaNumericGreaterThanOrEqual(FString& A, FString& B)
{
	return A >= B;
}

FString URedTechArtToolsBlueprintLibrary::PickContentPath(bool& bWasPathPicked, const FString DialogTitle,
                                                          const FString DefaultPath)
{
	const TSharedRef<SDlgPickPath> PickContentPathDlg = SNew(SDlgPickPath).Title(FText::FromString(DialogTitle)).
	                                                                       DefaultPath(FText::FromString(DefaultPath));

	switch (const EAppReturnType::Type ReturnType = PickContentPathDlg->ShowModal())
	{
	case EAppReturnType::Cancel:
		bWasPathPicked = false;
		return "";
	case EAppReturnType::Ok:
	default:
		bWasPathPicked = true;
		return PickContentPathDlg->GetPath().ToString();
	}
}

void URedTechArtToolsBlueprintLibrary::ConvertPackagePathToLocalPath(const FString PackagePath, bool& bOutSuccess,
                                                                     FString& OutLocalPath)
{
	bOutSuccess = FPackageName::TryConvertGameRelativePackagePathToLocalPath(PackagePath, OutLocalPath);
}


void URedTechArtToolsBlueprintLibrary::GetConfigArrayValue(const FString ConfigName, const FString SectionName,
                                                           const FString KeyName,
                                                           TArray<FString>& OutArrayValues)
{
	if (const FConfigFile* ConfigFile = GConfig->FindConfigFile(*GConfig->GetConfigFilename(*ConfigName)))
	{
		ConfigFile->GetArray(*SectionName, *KeyName, OutArrayValues);
	}
}

void URedTechArtToolsBlueprintLibrary::SetConfigArrayValue(const FString ConfigName, const FString SectionName,
                                                           const FString KeyName, const TArray<FString>& InArrayValues)
{
	if (FConfigFile* ConfigFile = GConfig->FindConfigFile(*GConfig->GetConfigFilename(*ConfigName)))
	{
		ConfigFile->SetArray(*SectionName, *KeyName, InArrayValues);
		ConfigFile->Dirty = true;
		GConfig->Flush(false);
	}
}

TMap<FString, FString> URedTechArtToolsBlueprintLibrary::GetEditorSettableVariables(UObject* Object)
{
	TMap<FString, FString> Result;
	if (IsValid(Object))
	{
		UClass* Class;
		if( const UBlueprint* Blueprint = Cast<UBlueprint>(Object); IsValid(Blueprint))
		{
			Class = Blueprint->GetBlueprintClass();
		}
		else 
		{
			Class = Object->GetClass();
		}

		if(IsValid(Class))
		{
			for (TFieldIterator<FProperty> PropertyIt(Class); PropertyIt; ++PropertyIt)
			{
				if( const FProperty* Property = *PropertyIt; Property != nullptr)
				{
					if(Property->HasAnyPropertyFlags(EPropertyFlags::CPF_Edit) && !Property->HasAnyPropertyFlags(PropertyAccessUtil::EditorReadOnlyFlags))
					{
						FString PropertyName = Property->GetNameCPP();
						FString ExtendedType;
						FString PropertyType = Property->GetCPPType(&ExtendedType);
						Result.Add(PropertyName, PropertyType + ExtendedType);
					}
				}
			}
		}	
	}
	return Result;
}

/** Dialog widget used to display an object its properties */
class SRedWidgetParamDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRedWidgetParamDialog) {}
	SLATE_END_ARGS()	

	void Construct(const FArguments& InArgs, TWeakPtr<SWindow> InParentWindow, const TSharedRef<SWidget> InnerWidget, const FShowWidgetDialogOptions& Options)
	{
		bOKPressed = false;

		FPropertyEditorModule& PropertyEditorModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		TSharedPtr<SWidget> Buttons;
			
		if(Options.bUseDefaultButtons)
		{
			SAssignNew(Buttons, SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.Padding(2.0f)
					.AutoWidth()
					[
						SNew(SButton)
						.ContentPadding(FCoreStyle::Get().GetMargin("StandardDialog.ContentPadding"))
						.Text(Options.OkayButtonText)
						.HAlign(HAlign_Center)
						.OnClicked_Lambda([this, InParentWindow, InArgs]()
						{
							if(InParentWindow.IsValid())
							{
								InParentWindow.Pin()->RequestDestroyWindow();
							}
							bOKPressed = true;
							return FReply::Handled(); 
						})
					]
					+SHorizontalBox::Slot()
					.Padding(2.0f)
					.AutoWidth()
					[
						SNew(SButton)
						.ContentPadding(FCoreStyle::Get().GetMargin("StandardDialog.ContentPadding"))
						.Text(Options.CancelButtonText)
						.HAlign(HAlign_Center)
						.OnClicked_Lambda([InParentWindow]()
						{ 
							if(InParentWindow.IsValid())
							{
								InParentWindow.Pin()->RequestDestroyWindow();
							}
							return FReply::Handled(); 
						})
					]
				];
		}
		else
		{
			SAssignNew(Buttons, SBorder)
				.Visibility(EVisibility::Collapsed);
		}
		
		ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				InnerWidget
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				Buttons.ToSharedRef()
			]
		];
	}

	bool WasOkPressed() const { return bOKPressed; }
protected:
	bool bOKPressed = false;
};

bool URedTechArtToolsBlueprintLibrary::ShowWidgetDialog(const FText& Title, UUserWidget* InOutWidget, const FShowWidgetDialogOptions& Options)
{
	if (!FApp::IsUnattended() && !GIsRunningUnattendedScript)
	{
		if (IsValid(InOutWidget))
		{
			TSharedRef<SWindow> Window = SNew(SWindow)
				.Title(Title)
				.SizingRule(Options.bAutoSized ? ESizingRule::Autosized : ESizingRule::UserSized)
				.AutoCenter(EAutoCenter::PrimaryWorkArea)
				.SupportsMinimize(false)
				.SupportsMaximize(false)
				.HasCloseButton(Options.bHasCloseButton)
				.MinWidth(Options.MinimumSize.X)
				.MinHeight(Options.MinimumSize.Y);
			
			InOutWidget->Initialize();
			TSharedRef<SWidget> TakenWidget = InOutWidget->TakeWidget();
			TSharedPtr<SRedWidgetParamDialog> Dialog;
			Window->SetContent(SAssignNew(Dialog, SRedWidgetParamDialog, Window, TakenWidget, Options));
			Window->ShowWindow();
			GEditor->EditorAddModalWindow(Window);
			return Dialog->WasOkPressed();
		}
	}

	return false;
}

bool URedTechArtToolsBlueprintLibrary::CloseWidgetParentWindow(UWidget* Widget)
{
	if(ensure(IsValid(Widget)))
	{
		if(const TSharedPtr<SWidget> CachedWidget = Widget->GetCachedWidget())
		{
			const TSharedPtr<SWindow> Window = FSlateApplication::Get().FindWidgetWindow(CachedWidget.ToSharedRef());
			if(Window.IsValid())
			{
				Window.Get()->RequestDestroyWindow();
			}
			return true;
		}
	}	
	return false;
}

UObject* URedTechArtToolsBlueprintLibrary::GetDefaultObjectFromBlueprint(const UObject* Blueprint)
{
	if(!IsValid(Blueprint)) return nullptr;
	
	const UBlueprint* BlueprintObj = Cast<UBlueprint>(Blueprint);
	if(IsValid(BlueprintObj))
	{
		if(IsValid(BlueprintObj->GeneratedClass))
		{
			return BlueprintObj->GeneratedClass->GetDefaultObject();
		}
		
		FName ClassName;
		FName SkeletonClassName;
		BlueprintObj->GetBlueprintCDONames(ClassName, SkeletonClassName);
		return FindObject<UObject>(nullptr, *ClassName.ToString());
	}
	
	if(IsValid(Blueprint->GetClass()))
	{
		return Blueprint->GetClass()->GetDefaultObject();
	}
	
	return nullptr;
}

#undef LOCTEXT_NAMESPACE
