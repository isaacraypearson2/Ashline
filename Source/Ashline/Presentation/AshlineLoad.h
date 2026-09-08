#pragma once

#include "CoreMinimal.h"
#include "Misc/PackageName.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/SoftObjectPtr.h"

/**
 * Quiet optional-content loader. Missing StarterContent / Fab / authored /Game
 * packages are skipped without LogUObject warnings. Engine paths still load.
 * Mannequin meshes under /Game/Characters remain eligible when the package exists.
 */
namespace AshlineLoad
{
	inline bool PackageExistsCached(const FString& PackageName)
	{
		static TMap<FString, bool> Cache;
		if (PackageName.IsEmpty())
		{
			return false;
		}
		if (const bool* Found = Cache.Find(PackageName))
		{
			return *Found;
		}
		const bool bExists = FPackageName::DoesPackageExist(PackageName);
		Cache.Add(PackageName, bExists);
		return bExists;
	}

	inline bool CanAttemptLoad(const FString& ObjectPath)
	{
		if (ObjectPath.IsEmpty())
		{
			return false;
		}
		const FSoftObjectPath Soft(ObjectPath);
		if (!Soft.IsValid())
		{
			return false;
		}
		if (Soft.ResolveObject() != nullptr)
		{
			return true;
		}
		const FString PackageName = Soft.GetLongPackageName();
		if (PackageName.IsEmpty())
		{
			return false;
		}
		// Engine mounts are always worth a quiet LoadObject (BasicShapes, WorldGrid, etc.).
		if (PackageName.StartsWith(TEXT("/Engine/")))
		{
			return true;
		}
		// Starter Content is optional; try it quietly even before DoesPackageExist warms.
		if (PackageName.StartsWith(TEXT("/Game/StarterContent/")))
		{
			return true;
		}
		return PackageExistsCached(PackageName);
	}

	template <typename T>
	T* Object(const FString& Path)
	{
		if (!CanAttemptLoad(Path))
		{
			return nullptr;
		}
		return LoadObject<T>(nullptr, *Path, nullptr, LOAD_NoWarn | LOAD_Quiet);
	}

	template <typename T>
	T* Object(const TCHAR* Path)
	{
		return Object<T>(FString(Path));
	}

	template <typename T>
	T* Soft(const TSoftObjectPtr<T>& Ptr)
	{
		if (Ptr.IsNull())
		{
			return nullptr;
		}
		if (T* Loaded = Ptr.Get())
		{
			return Loaded;
		}
		return Object<T>(Ptr.ToSoftObjectPath().ToString());
	}
}
