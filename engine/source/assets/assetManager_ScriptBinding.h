//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

ConsoleMethodGroupBeginWithDocs(AssetManager, SimObject)

/*! Compile the referenced assets determined by the specified module definition.
    @param moduleDefinition The module definition specifies the asset manifest.
    @return Whether the compilation was successful or not.
*/
ConsoleMethodWithDocs( AssetManager, compileReferencedAssets, ConsoleBool, 3, 3, (moduleDefinition))
{
    // Fetch module definition.
    ModuleDefinition* pModuleDefinition = Sim::findObject<ModuleDefinition>( argv[2] );

    // Did we find the module definition?
    if ( pModuleDefinition == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::compileReferencedAssets() - Could not find the module definition '%s'.", argv[2] );        
        return false;
    }

    // Compile referenced assets.
    return object->compileReferencedAssets( pModuleDefinition );
}

//-----------------------------------------------------------------------------

/*! Add any the declared assets specified by the module definition.
    @param moduleDefinition The module definition specifies the asset manifest.
    @return Whether adding declared assets was successful or not.
*/
ConsoleMethodWithDocs( AssetManager, addModuleDeclaredAssets, ConsoleBool, 3, 3, (moduleDefinition))
{
    // Fetch module definition.
    ModuleDefinition* pModuleDefinition = Sim::findObject<ModuleDefinition>( argv[2] );

    // Did we find the module definition?
    if ( pModuleDefinition == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::addDeclaredAssets() - Could not find the module definition '%s'.", argv[2] );        
        return false;
    }

    // Add module declared assets.
    return object->addModuleDeclaredAssets( pModuleDefinition );
}

//-----------------------------------------------------------------------------

/*! Add the specified asset against the specified module definition.
    @param moduleDefinition The module definition that may contain declared assets.
    @return Whether adding declared assets was successful or not.
*/
ConsoleMethodWithDocs( AssetManager, addDeclaredAsset, ConsoleBool, 4, 4, (moduleDefinition, assetFilePath))
{
    // Fetch module definition.
    ModuleDefinition* pModuleDefinition = Sim::findObject<ModuleDefinition>( argv[2] );

    // Did we find the module definition?
    if ( pModuleDefinition == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::addDeclaredAsset() - Could not find the module definition '%s'.", argv[2] );        
        return false;
    }

    // Fetch asset file-path.
    const char* pAssetFilePath = argv[3];

    // Add declared asset.
    return object->addDeclaredAsset( pModuleDefinition, pAssetFilePath );
}

//-----------------------------------------------------------------------------

/*! Adds a private asset object.
    @param assetObject The asset object to add as a private asset.
    @return The allocated private asset Id.
*/
ConsoleMethodWithDocs( AssetManager, addPrivateAsset, ConsoleString, 3, 3, (assetObject))
{
    // Fetch asset.
    AssetBase* pAssetBase = Sim::findObject<AssetBase>( argv[2] );

    // Did we find the asset?
    if ( pAssetBase == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::addPrivateAsset() - Could not find the asset '%s'.", argv[2] );        
        return StringTable->EmptyString;
    }

    // Add private asset.
    return object->addPrivateAsset( pAssetBase );
}

//-----------------------------------------------------------------------------

/*! Remove any the declared assets specified by the module definition.
    @param moduleDefinition The module definition that may contain declared assets.
    @return Whether removing declared assets was successful or not.
*/
ConsoleMethodWithDocs( AssetManager, removeDeclaredAssets, ConsoleBool, 3, 3, (moduleDefinition))
{
    // Fetch module definition.
    ModuleDefinition* pModuleDefinition = Sim::findObject<ModuleDefinition>( argv[2] );

    // Did we find the module definition?
    if ( pModuleDefinition == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::removeDeclaredAssets() - Could not find the module definition '%s'.", argv[2] );        
        return false;
    }

    // Remove declared assets.
    return object->removeDeclaredAssets( pModuleDefinition );
}

//-----------------------------------------------------------------------------

/*! Remove the specified declared asset Id.
    @param assetId The selected asset Id.
    @return Whether removing the declared asset was successful or not.
*/
ConsoleMethodWithDocs( AssetManager, removeDeclaredAsset, ConsoleBool, 3, 3, (assetId))
{
    // Remove the declared asset Id.
    return object->removeDeclaredAsset( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the asset name from the specified asset Id.
    @param assetId The selected asset Id.
    @return The asset name from the specified asset Id.
*/
ConsoleMethodWithDocs( AssetManager, getAssetName, ConsoleString, 3, 3, (assetId))
{
    return object->getAssetName( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the asset description from the specified asset Id.
    @param assetId The selected asset Id.
    @return The asset description from the specified asset Id.
*/
ConsoleMethodWithDocs( AssetManager, getAssetDescription, ConsoleString, 3, 3, (assetId))
{
    return object->getAssetDescription( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the asset category from the specified asset Id.
    @param assetId The selected asset Id.
    @return The asset category from the specified asset Id.
*/
ConsoleMethodWithDocs( AssetManager, getAssetCategory, ConsoleString, 3, 3, (assetId))
{
    return object->getAssetCategory( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the asset type from the specified asset Id.
    @param assetId The selected asset Id.
    @return The asset type from the specified asset Id.
*/
ConsoleMethodWithDocs( AssetManager, getAssetType, ConsoleString, 3, 3, (assetId))
{
    return object->getAssetType( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the asset file-path from the specified asset Id.
    @param assetId The selected asset Id.
    @return The asset file-path from the specified asset Id.
*/
ConsoleMethodWithDocs( AssetManager, getAssetFilePath, ConsoleString, 3, 3, (assetId))
{
    return object->getAssetFilePath( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the asset path (not including the asset file) from the specified asset Id.
    @param assetId The selected asset Id.
    @return The asset path (not including the asset file) from the specified asset Id.
*/
ConsoleMethodWithDocs( AssetManager, getAssetPath, ConsoleString, 3, 3, (assetId))
{
    return object->getAssetPath( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the module definition where the the specified asset Id is located.
    @param assetId The selected asset Id.
    @return The module definition where the the specified asset Id is located
*/
ConsoleMethodWithDocs( AssetManager, getAssetModule, ConsoleString, 3, 3, (assetId))
{
    // Fetch module definition.
    ModuleDefinition* pModuleDefinition = object->getAssetModuleDefinition( argv[2] );

    return pModuleDefinition == NULL ? StringTable->EmptyString : pModuleDefinition->getIdString();
}

//-----------------------------------------------------------------------------

/*! Check whether the specified asset Id is internal or not.
    @param assetId The selected asset Id.
    @return Whether the specified asset Id is internal or not.
*/
ConsoleMethodWithDocs( AssetManager, isAssetInternal, ConsoleBool, 3, 3, (assetId))
{
    return object->isAssetInternal( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Check whether the specified asset Id is private or not.
    @param assetId The selected asset Id.
    @return Whether the specified asset Id is private or not.
*/
ConsoleMethodWithDocs( AssetManager, isAssetPrivate, ConsoleBool, 3, 3, (assetId))
{
    return object->isAssetPrivate( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Check whether the specified asset Id is auto-unload or not.
    @param assetId The selected asset Id.
    @return Whether the specified asset Id is auto-unload or not.
*/
ConsoleMethodWithDocs( AssetManager, isAssetAutoUnload, ConsoleBool, 3, 3, (assetId))
{
    return object->isAssetAutoUnload( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Check whether the specified asset Id is loaded or not.
    @param assetId The selected asset Id.
    @return Whether the specified asset Id is loaded or not.
*/
ConsoleMethodWithDocs( AssetManager, isAssetLoaded, ConsoleBool, 3, 3, (assetId))
{
    return object->isAssetLoaded( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Check whether the specified asset Id is declared or not.
    @param assetId The selected asset Id.
    @return Whether the specified asset Id is declared or not.
*/
ConsoleMethodWithDocs( AssetManager, isDeclaredAsset, ConsoleBool, 3, 3, (assetId))
{
    return object->isDeclaredAsset( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Check whether the specified asset Id is referenced or not.
    @param assetId The selected asset Id.
    @return Whether the specified asset Id is referenced or not.
*/
ConsoleMethodWithDocs( AssetManager, isReferencedAsset, ConsoleBool, 3, 3, (assetId))
{
    return object->isReferencedAsset( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Rename declared asset Id.
    @param assetIdFrom The selected asset Id to rename from.
    @param assetIdFrom The selected asset Id to rename to.
    @return Whether the rename was successful or not.
*/
ConsoleMethodWithDocs( AssetManager, renameDeclaredAsset, ConsoleBool, 4, 4, (assetIdFrom, assetIdTo))
{
    return object->renameDeclaredAsset( argv[2], argv[3] );
}

//-----------------------------------------------------------------------------

/*! Rename referenced asset Id. 
    @param assetIdFrom The selected asset Id to rename from.
    @param assetIdFrom The selected asset Id to rename to.
    @return Whether the rename was successful or not.
*/
ConsoleMethodWithDocs( AssetManager, renameReferencedAsset, ConsoleBool, 4, 4, (assetIdFrom, assetIdTo))
{
    return object->renameReferencedAsset( argv[2], argv[3] );
}

//-----------------------------------------------------------------------------

/*! Acquire the specified asset Id.
    You must release the asset once you're finish with it using 'releaseAsset'.
    @param assetId The selected asset Id.
    @param asPrivate Whether to acquire the asset Id as a private asset.
    @return The acquired asset or NULL if not acquired.
*/
ConsoleMethodWithDocs( AssetManager, acquireAsset, ConsoleString, 3, 4, (assetId, [asPrivate?]))
{
    // Fetch asset Id.
    const char* pAssetId = argv[2];

    // Fetch private asset flag.
    const bool asPrivate = argc >= 4 ? dAtob(argv[3]) : false;

    // Reset asset reference.
    AssetBase* pAssetBase = NULL;

    // Acquire private asset?
    if ( asPrivate )
    {
        // Acquire private asset.
        pAssetBase = object->acquireAsPrivateAsset<AssetBase>( pAssetId );
    }
    else
    {
        // Acquire public asset.
        pAssetBase = object->acquireAsset<AssetBase>( pAssetId );
    }

    return pAssetBase != NULL ? pAssetBase->getIdString() : StringTable->EmptyString;
}

//-----------------------------------------------------------------------------

/*! Release the specified asset Id.
    The asset should have been acquired using 'acquireAsset'.
    @param assetId The selected asset Id.
    @return Whether the asset was released or not.
*/
ConsoleMethodWithDocs( AssetManager, releaseAsset, ConsoleBool, 3, 3, (assetId))
{
    // Release asset.
    return object->releaseAsset( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Ensures an asset is loaded even if it has no references.
	The asset is also set to not auto unload to prevent it from unloading. Use purgeAssets to unload the asset.
	@param assetId The selected asset Id.
	@return No return value.
*/
ConsoleMethodWithDocs(AssetManager, preloadAsset, ConsoleVoid, 3, 3, (assetId))
{
	// Fetch asset Id.
	const char* pAssetId = argv[2];

	// Reset asset reference.
	AssetBase* pAssetBase = NULL;

	// Acquire public asset.
	pAssetBase = object->acquireAsset<AssetBase>(pAssetId);

	// Did we find the asset?
	if (pAssetBase == NULL)
	{
		// No, so warn
		Con::warnf("AssetManager::preloadAsset() - Could not find the assetId '%s'.", argv[2]);
		return;
	}

	// Set the asset to auto-unload false
	pAssetBase->setAssetAutoUnload(false);

	// Release asset.
	object->releaseAsset(argv[2]);
}

//-----------------------------------------------------------------------------

/*! Purge all assets that are not referenced even if they are set to not auto-unload.
    Assets can be in this state because they are either set to not auto-unload or the asset manager has/is disabling auto-unload.
    @return No return value.
*/
ConsoleMethodWithDocs( AssetManager, purgeAssets, ConsoleVoid, 2, 2, ())
{
    // Purge assets.
    object->purgeAssets();
}

//-----------------------------------------------------------------------------

/*! Deletes the specified asset Id and optionally its loose files and asset dependencies.
    @param assetId The selected asset Id.
    @param deleteLooseFiles Whether to delete an assets loose files or not.
    @param deleteDependencies Whether to delete assets that depend on this asset or not.
    @return Whether the asset deletion was successful or not.  A failure only indicates that the specified asset was not deleted but dependent assets and their loose files may have being deleted.
*/
ConsoleMethodWithDocs( AssetManager, deleteAsset, ConsoleBool, 5, 5, (assetId, deleteLooseFiles, deleteDependencies))
{
    // Fetch asset Id.
    const char* pAssetId = argv[2];

    // Fetch delete loose-files flag.
    const bool deleteLooseFiles = dAtob(argv[3]);

    // Fetch delete dependencies flag.
    const bool deleteDependencies = dAtob(argv[4]);

    // Delete asset.
    return object->deleteAsset( pAssetId, deleteLooseFiles, deleteDependencies );
}

//-----------------------------------------------------------------------------

/*! Refresh the specified asset Id.
    @param assetId The selected asset Id.
    @return No return value.
*/
ConsoleMethodWithDocs( AssetManager, refreshAsset, ConsoleVoid, 3, 3, (assetId))
{
    object->refreshAsset( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Refresh all declared assets.
    @param Whether to include currently unloaded assets in the refresh or not.  Optional: Defaults to false.
    Refreshing all assets can be an expensive (time-consuming) operation to perform.
    @return No return value.
*/
ConsoleMethodWithDocs( AssetManager, refreshAllAssets, ConsoleVoid, 2, 3, ([bool includeUnloaded]))
{
    // Refresh assets without flag.
    if ( argc == 2 )
        object->refreshAllAssets();

    // Refresh assets with flag.
    object->refreshAllAssets(dAtob(argv[2]));
}

//-----------------------------------------------------------------------------

/*! Save the currently loaded asset tags manifest.
    @return Whether the save was successful or not.
*/
ConsoleMethodWithDocs( AssetManager, saveAssetTags, ConsoleBool, 2, 2, ())
{
    // Save asset tags.
    return object->saveAssetTags();
}

//-----------------------------------------------------------------------------

/*! Restore the currently loaded asset tags manifest from disk (replace anything in memory).
    @return Whether the restore was successful or not.
*/
ConsoleMethodWithDocs( AssetManager, restoreAssetTags, ConsoleBool, 2, 2, ())
{
    // Restore asset tags.
    return object->restoreAssetTags();
}

//-----------------------------------------------------------------------------

/*! Gets the currently loaded asset tags manifest.
    @return The currently loaded asset tags manifest or zero if not loaded.
*/
ConsoleMethodWithDocs( AssetManager, getAssetTags, ConsoleInt, 2, 2, ())
{
    // Fetch the asset tags manifest.
    AssetTagsManifest* pAssetTagsManifest = object->getAssetTags();

    return pAssetTagsManifest == NULL ? 0 : pAssetTagsManifest->getId();
}

//-----------------------------------------------------------------------------

/*! Performs an asset query searching for all assets optionally ignoring internal assets.
    @param assetQuery The asset query object that will be populated with the results.
    @param ignoreInternal Whether to ignore internal assets or not.  Optional: Defaults to true.
    @param ignorePrivate Whether to ignore private assets or not.  Optional: Defaults to true.
    @return The number of asset Ids found or (-1) if an error occurred.
*/
ConsoleMethodWithDocs( AssetManager, findAllAssets, ConsoleInt, 3, 5, (assetQuery, [ignoreInternal?], [ignorePrivate?]))
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAllAssets() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Perform query if no more arguments.
    if ( argc == 3 )
        return object->findAllAssets( pAssetQuery );

    // Fetch ignore internal flag.
    const bool ignoreInternal = dAtob(argv[3]);

    // Perform query if no more arguments.
    if ( argc == 4 )
        return object->findAllAssets( pAssetQuery, ignoreInternal );

    // Fetch ignore private flag.
    const bool ignorePrivate = dAtob(argv[4]);

    // Perform query.
    return object->findAllAssets( pAssetQuery, ignoreInternal, ignorePrivate );
}

//-----------------------------------------------------------------------------

/*! Performs an asset query searching for the specified asset name.
    @param assetQuery The asset query object that will be populated with the results.
    @param assetName The asset name to search for.  This may be a partial name if 'partialName' is true.
    @param partialName Whether the asset name is to be used as a partial name or not.  Optional: Defaults to false.
    @return The number of asset Ids found or (-1) if an error occurred.
*/
ConsoleMethodWithDocs( AssetManager, findAssetName, ConsoleInt, 4, 5, (assetQuery, assetName, [partialName?]))
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetName() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset name.
    const char* pAssetName = argv[3];

    // Perform query if no more arguments.
    if ( argc == 4 )
        return object->findAssetName( pAssetQuery, pAssetName );

    // Yes, so fetch partial name flag.
    const bool partialName = dAtob( argv[4] );

    // Perform query.
    return object->findAssetName( pAssetQuery, pAssetName, partialName );
}

//-----------------------------------------------------------------------------

/*! Performs an asset query searching for the specified asset category.
    @param assetQuery The asset query object that will be populated with the results.
    @param assetCategory The asset category to search for.
    @param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.
    @return The number of asset Ids found or (-1) if an error occurred.
*/
ConsoleMethodWithDocs( AssetManager, findAssetCategory, ConsoleInt, 4, 5, (assetQuery, assetCategory, [assetQueryAsSource?]))
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetCategory() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset category.
    const char* pAssetCategory = argv[3];

    // Any more arguments?
    if ( argc == 4 )
    {
        // No, so perform query.
        return object->findAssetCategory( pAssetQuery, pAssetCategory );
    }

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findAssetCategory( pAssetQuery, pAssetCategory, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

/*! Performs an asset query searching for the specified asset auto-unload flag.
    @param assetQuery The asset query object that will be populated with the results.
    @param assetInternal The asset internal flag to search for.
    @param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.
    @return The number of asset Ids found or (-1) if an error occurred.
*/
ConsoleMethodWithDocs( AssetManager, findAssetAutoUnload, ConsoleInt, 4, 5, (assetQuery, assetAutoUnload, [assetQueryAsSource?]))
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetAutoUnload() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset auto-unload flag.
    const bool assetAutoUnload = dAtob(argv[3]);

    // Any more arguments?
    if ( argc == 4 )
    {
        // No, so perform query.
        return object->findAssetAutoUnload( pAssetQuery, assetAutoUnload );
    }

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findAssetAutoUnload( pAssetQuery, assetAutoUnload, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

/*! Performs an asset query searching for the specified asset internal flag.
    @param assetQuery The asset query object that will be populated with the results.
    @param assetInternal The asset internal flag to search for.
    @param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.
    @return The number of asset Ids found or (-1) if an error occurred.
*/
ConsoleMethodWithDocs( AssetManager, findAssetInternal, ConsoleInt, 4, 5, (assetQuery, assetInternal, [assetQueryAsSource?]))
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetInternal() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset internal flag.
    const bool assetInternal = dAtob(argv[3]);

    // Any more arguments?
    if ( argc == 4 )
    {
        // No, so perform query.
        return object->findAssetInternal( pAssetQuery, assetInternal );
    }

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findAssetInternal( pAssetQuery, assetInternal, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

/*! Performs an asset query searching for the specified asset private flag.
    @param assetQuery The asset query object that will be populated with the results.
    @param assetPrivate The asset private flag to search for.
    @param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.
    @return The number of asset Ids found or (-1) if an error occurred.
*/
ConsoleMethodWithDocs( AssetManager, findAssetPrivate, ConsoleInt, 4, 5, (assetQuery, assetPrivate, [assetQueryAsSource?]))
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetPrivate() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset private flag.
    const bool assetPrivate = dAtob(argv[3]);

    // Any more arguments?
    if ( argc == 4 )
    {
        // No, so perform query.
        return object->findAssetPrivate( pAssetQuery, assetPrivate );
    }

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findAssetInternal( pAssetQuery, assetPrivate, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

/*! Performs an asset query searching for the specified asset type.
    @param assetQuery The asset query object that will be populated with the results.
    @param assetType The asset type to search for.
    @param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.
    @return The number of asset Ids found or (-1) if an error occurred.
*/
ConsoleMethodWithDocs( AssetManager, findAssetType, ConsoleInt, 4, 5, (assetQuery, assetType, [assetQueryAsSource?]))
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetType() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset type.
    const char* pAssetType = argv[3];

    // Any more arguments?
    if ( argc == 4 )
    {
        // No, so perform query.
        return object->findAssetType( pAssetQuery, pAssetType );
    }

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findAssetType( pAssetQuery, pAssetType, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

/*! Performs an asset query searching for asset Ids that the specified asset Id depends on.
    @param assetQuery The asset query object that will be populated with the results.
    @param assetId The asset Id to query for any asset Ids that it depends on.
    @return The number of asset Ids found or (-1) if an error occurred.
*/
ConsoleMethodWithDocs( AssetManager, findAssetDependsOn, ConsoleInt, 4, 4, (assetQuery, assetId))
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetDependsOn() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset Id.
    const char* pAssetId = argv[3];

    // Perform query.
    return object->findAssetDependsOn( pAssetQuery, pAssetId );
}

//-----------------------------------------------------------------------------

/*! Performs an asset query searching for asset Ids that depend on the specified asset Id.
    @param assetQuery The asset query object that will be populated with the results.
    @param assetId The asset Id to query for any asset Ids that may depend on it.
    @return The number of asset Ids found or (-1) if an error occurred.
*/
ConsoleMethodWithDocs( AssetManager, findAssetIsDependedOn, ConsoleInt, 4, 4, (assetQuery, assetId))
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetIsDependedOn() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset Id.
    const char* pAssetId = argv[3];

    // Perform query.
    return object->findAssetIsDependedOn( pAssetQuery, pAssetId );
}

//-----------------------------------------------------------------------------

/*! Performs an asset query searching for invalid asset references.
    @param assetQuery The asset query object that will be populated with the results.
    @return The number of asset Ids found that are invalid or (-1) if an error occurred.
*/
ConsoleMethodWithDocs( AssetManager, findInvalidAssetReferences, ConsoleInt, 3, 3, (assetQuery))
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findInvalidAssetReferences() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Perform query.
    return object->findInvalidAssetReferences( pAssetQuery );
}

//-----------------------------------------------------------------------------

/*! Performs an asset query searching for the specified asset tag name(s).
    @param assetQuery The asset query object that will be populated with the results.
    @param assetTagNames The asset tag name or names to search for.  Multiple names can be specified using comma, space, tab or newline separation.  Tags use an OR operation i.e. only assets tagged with ANY of the specified tags will be returned.
    @param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.
    @return The number of asset Ids found or (-1) if an error occurred.
*/
ConsoleMethodWithDocs( AssetManager, findTaggedAssets, ConsoleInt, 4, 5, (assetQuery, assetTagNames, [assetQueryAsSource?]))
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findTaggedAssets() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset tag name(s).
    const char* pAssetTagNames = argv[3];

    // Perform query if no more arguments.
    if ( argc == 4 )
        return object->findTaggedAssets( pAssetQuery, pAssetTagNames );

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findTaggedAssets( pAssetQuery, pAssetTagNames, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

/*! Performs an asset query searching for the specified loose file.
    @param assetQuery The asset query object that will be populated with the results.
    @param assetLooseFile The loose-file used by the asset to search for.
    @param assetQueryAsSource Whether to use the asset query as the data-source rather than the asset managers database or not.  Doing this effectively filters the asset query.  Optional: Defaults to false.
    @return The number of asset Ids found or (-1) if an error occurred.
*/
ConsoleMethodWithDocs( AssetManager, findAssetLooseFile, ConsoleInt, 4, 5, (assetQuery, assetLooseFile, [assetQueryAsSource?]))
{
    // Fetch asset query.
    AssetQuery* pAssetQuery = Sim::findObject<AssetQuery>( argv[2] );

    // Did we find the asset query?
    if ( pAssetQuery == NULL )
    {
        // No, so warn.
        Con::warnf( "AssetManager::findAssetLooseFile() - Could not find the asset query object '%s'.", argv[2] );
        return -1;
    }

    // Fetch asset loose file.
    const char* pAssetLooseFile = argv[3];

    // Any more arguments?
    if ( argc == 4 )
    {
        // No, so perform query.
        return object->findAssetLooseFile( pAssetQuery, pAssetLooseFile );
    }

    // Fetch asset-query-as-source flag.
    const bool assetQueryAsSource = dAtob(argv[4]);

    // Perform query.
    return object->findAssetLooseFile( pAssetQuery, pAssetLooseFile, assetQueryAsSource );
}

//-----------------------------------------------------------------------------

/*! Gets the number of declared assets.
    @return Returns the number of declared assets.
*/
ConsoleMethodWithDocs( AssetManager, getDeclaredAssetCount, ConsoleBool, 2, 2, ())
{
    return object->getDeclaredAssetCount();
}

//-----------------------------------------------------------------------------

/*! Gets the number of asset referenced.
    @return Returns the number of asset references.
*/
ConsoleMethodWithDocs( AssetManager, getReferencedAssetCount, ConsoleBool, 2, 2, ())
{
    return object->getReferencedAssetCount();
}

//-----------------------------------------------------------------------------

/*! Gets the number of loaded internal assets.
    @return Returns the number of loaded internal assets.
*/
ConsoleMethodWithDocs( AssetManager, getLoadedInternalAssetCount, ConsoleBool, 2, 2, ())
{
    return object->getLoadedInternalAssetCount();
}

//-----------------------------------------------------------------------------

/*! Gets the maximum number of loaded internal assets.
    @return Returns the maximum number of loaded internal assets.
*/
ConsoleMethodWithDocs( AssetManager, getMaxLoadedInternalAssetCount, ConsoleBool, 2, 2, ())
{
    return object->getMaxLoadedInternalAssetCount();
}

//-----------------------------------------------------------------------------

/*! Gets the number of loaded external assets.
    @return Returns the number of loaded external assets.
*/
ConsoleMethodWithDocs( AssetManager, getLoadedExternalAssetCount, ConsoleBool, 2, 2, ())
{
    return object->getLoadedExternalAssetCount();
}

//-----------------------------------------------------------------------------

/*! Gets the maximum number of loaded external assets.
    @return Returns the maximum number of loaded external assets.
*/
ConsoleMethodWithDocs( AssetManager, getMaxLoadedExternalAssetCount, ConsoleBool, 2, 2, ())
{
    return object->getMaxLoadedExternalAssetCount();
}

//-----------------------------------------------------------------------------

/*! Dumps a breakdown of all declared assets.
    @return No return value.
*/
ConsoleMethodWithDocs( AssetManager, dumpDeclaredAssets, ConsoleVoid, 2, 2, ())
{
    return object->dumpDeclaredAssets();
}

ConsoleMethodGroupEndWithDocs(AssetManager)
