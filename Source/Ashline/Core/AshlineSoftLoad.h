#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/SoftObjectPtr.h"
#include <initializer_list>

/**
 * Null-safe content loaders. Missing Fab / Quixel / MetaHuman / StarterContent
 * paths must never assert or crash a Shipping cook or packaged run.
 *
 * Prefer FSoftObjectPath::TryLoad over LoadObject: TryLoad returns nullptr
 * without ensure() when the package is absent.
 */
namespace AshlineSoftLoad
{
	inline bool IsUsablePath(const FString& Path)
	{
		return !Path.IsEmpty() && Path.StartsWith(TEXT("/"));
	}

	template <typename T>
	T* TryLoadPath(const FString& Path)
	{
		if (!IsUsablePath(Path))
		{
			return nullptr;
		}

		const FSoftObjectPath Soft(Path);
		if (!Soft.IsValid())
		{
			return nullptr;
		}
		if (UObject* Existing = Soft.ResolveObject())
		{
			return Cast<T>(Existing);
		}
		return Cast<T>(Soft.TryLoad());
	}

	template <typename T>
	T* TryLoadSoft(const TSoftObjectPtr<T>& Soft)
	{
		if (Soft.IsNull())
		{
			return nullptr;
		}
		if (T* Existing = Soft.Get())
		{
			return Existing;
		}
		const FSoftObjectPath Path = Soft.ToSoftObjectPath();
		if (!Path.IsValid())
		{
			return nullptr;
		}
		if (UObject* Existing = Path.ResolveObject())
		{
			return Cast<T>(Existing);
		}
		return Cast<T>(Path.TryLoad());
	}

	template <typename T>
	T* TryLoadFirst(std::initializer_list<const TCHAR*> Paths)
	{
		for (const TCHAR* Path : Paths)
		{
			if (Path)
			{
				if (T* Obj = TryLoadPath<T>(FString(Path)))
				{
					return Obj;
				}
			}
		}
		return nullptr;
	}

	template <typename T>
	T* TryLoadFirst(const TArray<FString>& Paths)
	{
		for (const FString& Path : Paths)
		{
			if (T* Obj = TryLoadPath<T>(Path))
			{
				return Obj;
			}
		}
		return nullptr;
	}
}
