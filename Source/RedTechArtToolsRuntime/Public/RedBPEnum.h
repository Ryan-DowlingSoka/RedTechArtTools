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
#include "RedBPEnum.generated.h"

/**
 * Wrapper class for an blueprint defined enum with a details customization to let C++ create properties with it.
 * Use meta=(UserCanSetEnum) to allow the user to define which BP based Enum object is used in editor.
 *
 * WARNING: This struct is a bandage for dealing with Blueprint based Enums inside of C++. In almost every case the
 *			correct action is to move the BP based enum into C++ before using it inside of C++.
 *			Sometimes that may not be desirable, in those cases this struct may be a suitable workaround.
 *			Use with caution. Be smart. Be safe.
 *
 * Redirection in the Enum source is handled whenever the value is gotten through one of the accessors.
 * Redirection on Get() is disabled at Shipping, so ensure all values have been properly updated and serialized before
 * cooking for Shipping.
 *
 * The values for a selection of a given enum are saved, Index, Value, and Name. When handling redirections the order
 * of resolution is:
 * === Find Matching Enum Entry by Name
 * === Find Matching Enum Entry by Index
 * === Find Matching Enum Entry by Value
 *
 * At runtime values should be compared using the given accessors GetIndex, Get Value, or Get Name.
 *
 */
USTRUCT(BlueprintType)
struct REDTECHARTTOOLSRUNTIME_API FRedBPEnum
{
	GENERATED_BODY()

	FRedBPEnum() {}
	explicit FRedBPEnum(const UEnum* InEnum)
	{
		SetEnum(InEnum);
	}
	explicit FRedBPEnum(const FSoftObjectPath InEnumSoftObjectPath)
	{
		SetEnumByPath(InEnumSoftObjectPath);
	}
	explicit FRedBPEnum(const FString InEnumObjectPath)
	{
		SetEnumByPath(FSoftObjectPath(*InEnumObjectPath));
	}

	UPROPERTY(Transient, EditAnywhere, meta=(DisplayThumbnail="false"), Category="Blueprint Enum")
	TObjectPtr<const UEnum> SourceEnum = nullptr;
	
	const FSoftObjectPath& GetEnumPath() const
	{
		return SourceEnumPath;
	}
	
	const UEnum* GetEnum() const
	{
#if !UE_BUILD_SHIPPING
		FRedBPEnum* NonConstThis = const_cast<FRedBPEnum*>(this);
		NonConstThis->SourceEnum = Cast<UEnum>(SourceEnumPath.TryLoad());
		return SourceEnum;
#else
		return nullptr;
#endif
	}

	void SetEnum(const UEnum* NewEnum)
	{
#if !UE_BUILD_SHIPPING
		SourceEnumPath = FSoftObjectPath(*NewEnum->GetPathName());
		SourceEnum = NewEnum;
#endif
	}

	void SetEnumByPath(const FSoftObjectPath& NewEnumPath)
	{
#if !UE_BUILD_SHIPPING
		SourceEnumPath = NewEnumPath;
		SourceEnum = Cast<UEnum>(NewEnumPath.TryLoad());
#endif
	}
	
	int32 GetIndex() const
	{
#if !UE_BUILD_SHIPPING
		FRedBPEnum* NonConstThis = const_cast<FRedBPEnum*>(this);
		NonConstThis->UpdateEnum();
#endif
		return Index;
	}

	int32 GetValue() const
	{
#if !UE_BUILD_SHIPPING
		FRedBPEnum* NonConstThis = const_cast<FRedBPEnum*>(this);
		NonConstThis->UpdateEnum();
#endif
		return Value;
	}

	FName GetName() const
	{
#if !UE_BUILD_SHIPPING
		FRedBPEnum* NonConstThis = const_cast<FRedBPEnum*>(this);
		NonConstThis->UpdateEnum();
#endif
		return Name;
	}

	void SetIndex(const int32 NewIndex)
	{
		Index = NewIndex;
		Name = NAME_None;
		UpdateEnum();
	}

	void SetName(const FName NewName)
	{
		Name = NewName;
		Index = INDEX_NONE;
		UpdateEnum();
	}

	void SetValue(const int64 NewValue)
	{
		Value = NewValue;
		Name = NAME_None;
		Index = INDEX_NONE;
		UpdateEnum();
	}
	

protected:
	UPROPERTY(Config)
	FSoftObjectPath SourceEnumPath;
	
	UPROPERTY(Config);
	int64 Value = 0;

	UPROPERTY(Config)
	int32 Index = INDEX_NONE;

	UPROPERTY(Config)
	FName Name = NAME_None;

	//Fixup any potential changes to the Enum.
	void UpdateEnum()
	{
		if(const UEnum* Enum = GetEnum(); IsValid(Enum))
		{
			if(Name != NAME_None)
			{
				//Search by Name first.
				const int32 NewIndex = Enum->GetIndexByName(Name);
				if(NewIndex != INDEX_NONE)
				{
					Index = NewIndex;
					const int64 NewValue = Enum->GetValueByIndex(Index);
					Value = NewValue;
					return;
				}
			}
				
			if (Index != INDEX_NONE)
			{
				const FName NewName = Enum->GetNameByIndex(Index);
				if(NewName != NAME_None)
				{
					Name = NewName;
					const int64 NewValue = Enum->GetValueByIndex(Index);
					Value = NewValue;
					return;
				}	
			}

			const int32 NewValueIndex = Enum->GetIndexByValue(Value);
			if(NewValueIndex != INDEX_NONE)
			{
				Index = NewValueIndex;
				const FName NewName = Enum->GetNameByIndex(Index);
				Name = NewName;
			}
		}
	}
};

UCLASS()
class REDTECHARTTOOLSRUNTIME_API URedBPEnumBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// Returns the currently set enum Element Index of the given RedBPEnum.
	UFUNCTION(BlueprintCallable, Category="RedBPEnum")
	static int32 GetIndex(UPARAM(ref) const FRedBPEnum& RedBPEnum)
	{
		return RedBPEnum.GetIndex();
	}

	// Returns the currently set enum Element Value of the given RedBPEnum.
	UFUNCTION(BlueprintCallable, Category="RedBPEnum")
	static int64 GetValue(UPARAM(ref) const FRedBPEnum& RedBPEnum)
	{
		return RedBPEnum.GetValue();
	}

	// Returns the currently set enum Element Fully qualified name of the given RedBPEnum. e.g. MyEnum::MyEnumValue0
	// This is not the display name.
	UFUNCTION(BlueprintCallable, Category="RedBPEnum")
	static FName GetName(UPARAM(ref) const FRedBPEnum& RedBPEnum)
	{
		return RedBPEnum.GetName();
	}

	// Sets the currently set enum Element Index of the given RedBPEnum. This will also set Value and Name by Index.
	UFUNCTION(BlueprintCallable, Category="RedBPEnum")
	static void SetIndex(UPARAM(ref) FRedBPEnum& RedBPEnum, int32 NewIndex)
	{
		RedBPEnum.SetIndex(NewIndex);
	}

	// Sets the currently set enum Element Value of the given RedBPEnum. This will also set Index and Name by Value.
	UFUNCTION(BlueprintCallable, Category="RedBPEnum")
	static void SetValue(UPARAM(ref) FRedBPEnum& RedBPEnum, int64 NewValue)
	{
		RedBPEnum.SetValue(NewValue);
	}

	// Sets the currently set enum Element Name of the given RedBPEnum. This will also set Index and Value by Name.
	UFUNCTION(BlueprintCallable, Category="RedBPEnum")
	static void SetName(UPARAM(ref) FRedBPEnum& RedBPEnum, FName NewName)
	{
		RedBPEnum.SetName(NewName);
	}
};