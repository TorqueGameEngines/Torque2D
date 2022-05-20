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

ConsoleMethodGroupBeginWithDocs(ModuleManager, SimObject)

/*! Set the module extension used to scan for modules.  The default is 'module'.
    @param moduleExtension The module extension used to scan for modules.  Do not use a period character.
    @return Whether setting the module extension was successful or not.
*/
ConsoleMethodWithDocs(ModuleManager, setModuleExtension, ConsoleBool, 3, 3, (moduleExtension))
{
    // Set module extension.
    return object->setModuleExtension( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Scans for modules which are sub-directories of the specified path.
    @param moduleRootPath The root directory to scan for sub-directories containing modules.
    @param rootOnly [Optional] - Specifies whether to only scan the root path or not when searching for modules.
    @return Whether the scan was successful or not.  A successful scan can still find zero modules.
*/
ConsoleMethodWithDocs(ModuleManager, scanModules, ConsoleBool, 3, 4, (moduleRootPath, [rootOnly = false]))
{
    // Fetch root path.
    const char* pRootPath = argv[2];

    // Did we specify whether to include the root?
    if( argc == 3 )
    {
        // No, so scan modules.
        return object->scanModules( pRootPath );
    }

    // Fetch root only.
    const bool rootOnly = argc > 3 ? dAtob( argv[3] ) : false;

    // Scan modules.
    return object->scanModules( pRootPath, rootOnly );
}

//-----------------------------------------------------------------------------

/*! Unregister the specified module.
    @param moduleId The module Id to unregister.
    @param versionId The version Id to unregister.
    @return Whether the module was unregister or not.
*/
ConsoleMethodWithDocs(ModuleManager, unregisterModule, ConsoleBool, 4, 4, (moduleId, versionId))
{
    // Fetch the module Id.
    const char* pModuleId = argv[2];

    // Fetch the version Id.
    const U32 versionId = (U32)dAtoi(argv[3]);

    // Unregister the module.
    return object->unregisterModule( pModuleId, versionId );
}

//-----------------------------------------------------------------------------

/*! Load the specified module group.
    @param moduleGroup The module group to load.
    @return Whether the module group was loaded or not.
*/
ConsoleMethodWithDocs(ModuleManager, loadGroup, ConsoleBool, 3, 3, (moduleGroup))
{
    // Load module group.
    return object->loadModuleGroup( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Unload the specified module group.
    @param moduleGroup The module group to unload.
    @return Whether the module group was unloaded or not.
*/
ConsoleMethodWithDocs(ModuleManager, unloadGroup, ConsoleBool, 3, 3, (moduleGroup))
{
    // Unload module group.
    return object->unloadModuleGroup( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Load the specified module explicitly.
    @param moduleId The module Id to load.
    @param versionId The version Id to load.  Optional:  Will load the latest version.
    @return Whether the module Id was loaded or not.
*/
ConsoleMethodWithDocs(ModuleManager, loadExplicit, ConsoleBool, 3, 4, (moduleId, [versionId]))
{
    // Fetch the module Id.
    const char* pModuleId = argv[2];

    // Load module Id without a version if not specified.
    if ( argc == 3 )
        return object->loadModuleExplicit( pModuleId );

    // Fetch the version Id.
    const U32 versionId = (U32)dAtoi(argv[3]);

    // Load the module Id with a version.
    return object->loadModuleExplicit( pModuleId, versionId );
}

//-----------------------------------------------------------------------------

/*! Unload the specified module explicitly.
    @param moduleId The module Id to unload.
    @return Whether the module Id was unloaded or not.
*/
ConsoleMethodWithDocs(ModuleManager, unloadExplicit, ConsoleBool, 3, 3, (moduleId))
{
    // Unload module Id explicitly.
    return object->unloadModuleExplicit( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Checks to see if a module is loaded or not.
    @param moduleId The module Id to check.
    @return True if the module is loaded. False otherwise.
*/
ConsoleMethodWithDocs(ModuleManager, isModuleLoaded, ConsoleBool, 3, 3, (moduleId))
{
    ModuleDefinition* pLoadedModule = object->findLoadedModule(argv[2]);

    return pLoadedModule != NULL;
}

//-----------------------------------------------------------------------------

/*! Find the specific module Id optionally at the specified version Id.
    @param moduleId The module Id to find.
    @param versionId The version Id to find.
    @return The module definition object or NULL if not found.
*/
ConsoleMethodWithDocs(ModuleManager, findModule, ConsoleString, 4, 4, (moduleId, versionId))
{
    // Fetch module Id.
    const char* pModuleId = argv[2];

    // Fetch version Id.
    const U32 versionId = dAtoi(argv[3]);

    // Find module definition.
    ModuleDefinition* pModuleDefinition = object->findModule( pModuleId, versionId );

    // Return nothing if not found.
    if ( pModuleDefinition == NULL )
        return StringTable->EmptyString;

    return pModuleDefinition->getIdString();
}

//-----------------------------------------------------------------------------

/*! Find all the modules registered with the specified loaded state.
    @param loadedOnly Whether to return only modules that are loaded or not.
    @return A list of space-separated module definition object Ids.
*/
ConsoleMethodWithDocs(ModuleManager, findModules, ConsoleString, 3, 3, (loadedOnly?))
{
    // Fetch loaded only.
    const bool loadedOnly = dAtob(argv[2]);

    // Find module type definitions.
    Vector<const ModuleDefinition*> moduleDefinitions;

    // Find modules.
    object->findModules( loadedOnly, moduleDefinitions );

    // Fetch module definition count.
    const U32 moduleDefinitionCount = (U32)moduleDefinitions.size();

    // Finish if no module definition were found.
    if ( moduleDefinitionCount == 0 )
        return StringTable->EmptyString;

    // Create a return buffer.
    S32 bufferSize = 4096;
    char* pReturnBuffer = Con::getReturnBuffer( bufferSize );
    char* pBufferWrite = pReturnBuffer;

    // Iterate module definitions.
    for ( ModuleManager::typeConstModuleDefinitionVector::const_iterator moduleDefinitionItr = moduleDefinitions.begin(); moduleDefinitionItr != moduleDefinitions.end(); ++moduleDefinitionItr )
    {
        // Fetch module definition.
        const ModuleDefinition* pModuleDefinition = *moduleDefinitionItr;

        // Format module definition.
        const U32 offset = dSprintf( pBufferWrite, bufferSize, "%d ", pModuleDefinition->getId() );
        pBufferWrite += offset;
        bufferSize -= offset;

        // Are we out of buffer space?
        if ( bufferSize <= 0 )
        {
            // Yes, so warn.
            Con::warnf( "ModuleManager::findModules() - Ran out of buffer space." );
            break;
        }
    }

    return pReturnBuffer;
}

//-----------------------------------------------------------------------------

/*! Find the modules registered with the specified module type.
    @param moduleType The module type to search for.
    @param loadedOnly Whether to return only modules that are loaded or not.
    @return A list of space-separated module definition object Ids.
*/
ConsoleMethodWithDocs(ModuleManager, findModuleTypes, ConsoleString, 4, 4, (moduleType, loadedOnly?))
{
    // Fetch module type.
    const char* pModuleType = argv[2];

    // Fetch loaded only.
    const bool loadedOnly = dAtob(argv[3]);

    // Find module type definitions.
    Vector<const ModuleDefinition*> moduleDefinitions;

    // Find module types.
    object->findModuleTypes( pModuleType, loadedOnly, moduleDefinitions );

    // Fetch module definition count.
    const U32 moduleDefinitionCount = (U32)moduleDefinitions.size();

    // Finish if no module definition were found.
    if ( moduleDefinitionCount == 0 )
        return StringTable->EmptyString;

    // Create a return buffer.
    S32 bufferSize = 4096;
    char* pReturnBuffer = Con::getReturnBuffer( bufferSize );
    char* pBufferWrite = pReturnBuffer;

    // Iterate module definitions.
    for ( ModuleManager::typeConstModuleDefinitionVector::const_iterator moduleDefinitionItr = moduleDefinitions.begin(); moduleDefinitionItr != moduleDefinitions.end(); ++moduleDefinitionItr )
    {
        // Fetch module definition.
        const ModuleDefinition* pModuleDefinition = *moduleDefinitionItr;

        // Format module definition.
        const U32 offset = dSprintf( pBufferWrite, bufferSize, "%d ", pModuleDefinition->getId() );
        pBufferWrite += offset;
        bufferSize -= offset;

        // Are we out of buffer space?
        if ( bufferSize <= 0 )
        {
            // Yes, so warn.
            Con::warnf( "ModuleManager::findTypes() - Ran out of buffer space." );
            break;
        }
    }

    return pReturnBuffer;
}

//-----------------------------------------------------------------------------

/*! Copy the module to a new location with a new module Id.
    @param sourceModuleDefinition The module definition to copy.
    @param sourceModuleVersion The module version to copy.
    @param targetModuleId The module Id to rename the copied module to including all references to the source module Id.  It is valid to specifiy the source module Id to produce an identical copy.
    @param targetPath The target path to copy the module to.  Addition folders will be created depending on whether 'useVersionPathing' is used or not.
    @param useVersionPathing Whether to add a '/targetModuleId/versionId' folder to the target path or not.  This allows copying multiple versions of the same module Id.
    @return The new module definition file if copy was successful or NULL if not.
*/
ConsoleMethodWithDocs(ModuleManager, copyModule, ConsoleString, 7, 7, (sourceModuleDefinition, sourceModuleVersion, targetModuleId, targetPath, useVersionPathing?))
{
    // Find the source module definition.
    ModuleDefinition* pSourceModuleDefinition = object->findModule(argv[2], dAtoi(argv[3]));

    // Was the module definition found?
    if ( pSourceModuleDefinition == NULL )
    {
        // No, so warn.
        Con::warnf( "ModuleManager::copyModule() - Could not find source module definition '%s'.", argv[2] );
        return NULL;
    }

    // Fetch the target module Id.
    const char* pTargetModuleId = argv[4];
    
    // Fetch the target path.
    const char* pTargetPath = argv[5];

    // Fetch the 'useVersionPathing' flag.
    const bool useVersionPathing = dAtob(argv[6]);

    // Copy module.
    return object->copyModule( pSourceModuleDefinition, pTargetModuleId, pTargetPath, useVersionPathing );
}

//-----------------------------------------------------------------------------

/*! Synchronize the module dependencies of a module definition to a target dependency folder.
    @param rootModuleDefinition The module definition used to determine dependencies.
    @param targetDependencyPath The target dependency folder to copy dependencies to.
    @return Whether the module dependencies were synchronized correctly or not.
*/
ConsoleMethodWithDocs(ModuleManager, synchronizeDependencies, ConsoleBool, 4, 4, (rootModuleDefinition, targetDependencyPath))
{
    // Find the root module definition.
    ModuleDefinition* pRootModuleDefinition = dynamic_cast<ModuleDefinition*>( Sim::findObject( argv[2] ) );

    // Was the module definition found?
    if ( pRootModuleDefinition == NULL )
    {
        // No, so warn.
        Con::warnf( "ModuleManager::synchronizeModules() - Could not find root module definition '%s'.", argv[2] );
        return false;
    }

    // Fetch target dependency folder.
    const char* pTargetDependencyFolder = argv[3];

    // Synchronize dependencies.
    return object->synchronizeDependencies( pRootModuleDefinition, pTargetDependencyFolder );
}

//-----------------------------------------------------------------------------

/*! Checks whether a module merge definition file is available or not.
    @return Whether a module merge definition file is available or not.
*/
ConsoleMethodWithDocs(ModuleManager, isModuleMergeAvailable, ConsoleBool, 2, 2, ())
{
    // Check if module merge is available or not.
    return object->isModuleMergeAvailable();
}

//-----------------------------------------------------------------------------

/*! Checks whether a module merge using the modules in the source path can current happen or not.
    @param mergeSourcePath The path where modules to be merged are located.
    @return Whether a module merge using the modules in the source path can current happen or not.
*/
ConsoleMethodWithDocs(ModuleManager, canMergeModules, ConsoleBool, 3, 3, (mergeSourcePath))
{
    // Check whether the merge modules can current happen or not.
    return object->canMergeModules( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Performs a module merge into the selected target path.
    @param mergeTargetPath The path where modules will be merged into.
    @param removeMergeDefinition Whether to remove any merge definition found or not if merge is successful.
    @param registerNewModules Whether new (not replaced or updated) modules should be registered or not.
    @return Whether the module merge was successful or not.  Failure here could result in a corrupt module state.  Reinstall is recommended or at least advised to the user is recommended.
*/
ConsoleMethodWithDocs(ModuleManager, mergeModules, ConsoleBool, 5, 5, (mergeTargetPath, removeMergeDefinition, registerNewModules))
{
    // Fetch merge target path.
    const char* pMergeTargetPath = argv[2];

    // Fetch remove merge definition flag.
    const bool removeMergeDefinition = dAtob(argv[3]);

    // Fetch register new module flag.
    const bool registerNewModules = dAtob(argv[4]);

    // Merge modules.
    return object->mergeModules( pMergeTargetPath, removeMergeDefinition, registerNewModules );
}

//-----------------------------------------------------------------------------

/*! Registers the specified object as a listener for module notifications.
    @param listenerObject The object to start receiving module notifications.
    @return No return value.
*/
ConsoleMethodWithDocs(ModuleManager, addListener, ConsoleVoid, 3, 3, (listenerObject))
{
    // Find object.
    SimObject* pListener = Sim::findObject( argv[2] );

    // Did we find the listener object?
    if ( pListener == NULL )
    {
        // No, so warn.
        Con::warnf( "ModuleManager::addNotifications() - Could not find the listener object '%s'.", argv[2] );
        return;
    }

    object->addListener( pListener );
}

//-----------------------------------------------------------------------------

/*! Unregisters the specified object as a listener for module notifications.
    @param listenerObject The object to stop receiving module notifications.
    @return No return value.
*/
ConsoleMethodWithDocs(ModuleManager, removeListener, ConsoleVoid, 3, 3, (listenerObject))
{
    // Find object.
    SimObject* pListener = Sim::findObject( argv[2] );

    // Did we find the listener object?
    if ( pListener == NULL )
    {
        // No, so warn.
        Con::warnf( "ModuleManager::removeNotifications() - Could not find the listener object '%s'.", argv[2] );
        return;
    }

    object->removeListener( pListener );
}

//-----------------------------------------------------------------------------

/*! Removes everything from the module database
    @return No return value
*/
ConsoleMethodWithDocs(ModuleManager, clearDatabase, ConsoleVoid, 2, 2, ())
{
    // Check if module merge is available or not.
    return object->clearDatabase();
}

ConsoleMethodGroupEndWithDocs(ModuleManager)
