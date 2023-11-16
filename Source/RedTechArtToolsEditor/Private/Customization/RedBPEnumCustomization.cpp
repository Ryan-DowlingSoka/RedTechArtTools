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

#include "Customization/RedBPEnumCustomization.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "RedBPEnum.h"
#include "SSearchableComboBox.h"

#define LOCTEXT_NAMESPACE "RedRDEnumCustomization"

TSharedRef<IPropertyTypeCustomization> FRedBPEnumCustomization::MakeInstance()
{
	// Create the instance and returned a SharedRef
	return MakeShareable(new FRedBPEnumCustomization());
}

void FRedBPEnumCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
                                                      class FDetailWidgetRow& HeaderRow,
                                                      IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	void* StructData = nullptr;
	const FPropertyAccess::Result Result = StructPropertyHandle.Get().GetValueData(StructData);
	if (Result == FPropertyAccess::Success)
	{
		check(StructData);
		FRedBPEnum* CurrentValue = static_cast<FRedBPEnum*>(StructData);
		
		TSharedRef<SWidget> CurrentSelectionText = SNew(STextBlock)
		.Text_Lambda([=]()
		{
			const UEnum* Enum = CurrentValue->GetEnum();
			if(IsValid(Enum))
			{
				return Enum->GetDisplayNameTextByIndex(CurrentValue->GetIndex());
			}
			
			return FText::Format(LOCTEXT("Error_EnumNotLoaded", "{0} is not loaded."), FText::FromString(CurrentValue->GetEnumPath().ToString()));
		})
		.ToolTip(
			SNew(SToolTip)
			.Text_Lambda([=]()
			{
				const UEnum* Enum = CurrentValue->GetEnum();
				if(IsValid(Enum))
				{
					return Enum->GetToolTipTextByIndex(CurrentValue->GetIndex());
				}
				return FText::FromString("");
			})
		);

		HeaderRow.NameContent()[StructPropertyHandle->CreatePropertyNameWidget()]
		.ValueContent()
		[
			SAssignNew(CachedSearchableComboBox,SSearchableComboBox)
			.OptionsSource(GetEnumOptions(CurrentValue->GetEnum()))
			.OnComboBoxOpening_Lambda([this, CurrentValue](){GetEnumOptions(CurrentValue->GetEnum());CachedSearchableComboBox->RefreshOptions();})
			.OnSelectionChanged_Lambda([=](const TSharedPtr<FString> NewChoice, ESelectInfo::Type SelectType)
						                 {
											 const UEnum* Enum = CurrentValue->GetEnum();
											 if(!IsValid(Enum))
											 {
												 return;
											 }
						                     if (NewChoice.IsValid())
						                     {
						                        const FScopedTransaction Transaction(LOCTEXT("SetBPEnumValue", "Set BPEnum Value"));
						                        StructPropertyHandle.Get().NotifyPreChange();
							                    for(int i = 0; i < Enum->NumEnums(); i++)
							                    {
							                     	if(Enum->GetDisplayNameTextByIndex(i).ToString() == *NewChoice.Get())
							                     	{
							                     		CurrentValue->SetIndex(i);
							                     		break;
							                     	}
							                    }
							                    
						                        StructPropertyHandle.Get().NotifyPostChange(
													 EPropertyChangeType::ValueSet);
						                     }
						                 })
			.OnGenerateWidget_Lambda([=](TSharedPtr<FString> Choice)
									{
										const UEnum* Enum = CurrentValue->GetEnum();
										const FText DisplayText = FText::FromString(*Choice.Get());
										FText Tooltip;
										for(int i = 0; i < Enum->NumEnums(); i++)
										{
											if(Enum->GetDisplayNameTextByIndex(i).ToString() == *Choice.Get())
											{
												Tooltip = Enum->GetToolTipTextByIndex(i);
												break;
											}
										}
				
										return SNew(STextBlock)
										.Text(DisplayText)
										.ToolTip(
											SNew(SToolTip)
											.Text(Tooltip)
											);
									})
			.Content()
				[
					CurrentSelectionText
				]
		];
	}
}

void FRedBPEnumCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
                                                        class IDetailChildrenBuilder& StructBuilder,
                                                        IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	void* StructData = nullptr;
	const FPropertyAccess::Result Result = StructPropertyHandle.Get().GetValueData(StructData);
	if (Result == FPropertyAccess::Success)
	{
		check(StructData);
		FRedBPEnum* CurrentValue = static_cast<FRedBPEnum*>(StructData);
		const TSharedPtr<IPropertyHandle> SourceEnumHandle = StructPropertyHandle.Get().GetChildHandle(
			GET_MEMBER_NAME_CHECKED(FRedBPEnum, SourceEnum)
		);

		SourceEnumHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([this, CurrentValue]()
		{
			CachedSearchableComboBox->RefreshOptions();
			CurrentValue->SetEnum(CurrentValue->SourceEnum);
			CurrentValue->SetIndex(CurrentValue->GetIndex());
		}));
		auto& Property = StructBuilder.AddProperty(SourceEnumHandle.ToSharedRef());
		bool bUserCanSetEnum = StructPropertyHandle->HasMetaData(("UserCanSetEnum"));
		Property.IsEnabled(TAttribute<bool>::CreateLambda([CurrentValue, bUserCanSetEnum]
		{
			return bUserCanSetEnum || !IsValid(CurrentValue->GetEnum());
		}));
	}
	



}

TArray<TSharedPtr<FString>>* FRedBPEnumCustomization::GetEnumOptions(const UEnum* Enum)
{
	if(IsValid(Enum))
	{
		CachedEnumOptions.Empty(Enum->NumEnums()-1);
		for(int i = 0; i < Enum->NumEnums()-1; i++)
		{
			CachedEnumOptions.Add(MakeShared<FString>(Enum->GetDisplayNameTextByIndex(i).ToString()));
		}
	}
	return &CachedEnumOptions;
}


#undef LOCTEXT_NAMESPACE