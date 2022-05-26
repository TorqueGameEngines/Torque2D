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

ConsoleMethodGroupBeginWithDocs(ModuleDefinition, SimSet)

/*! Saves the module definition to the file it was loaded from (if any).
    @return (bool success) Whether the module definition was saved or not.
*/
ConsoleMethodWithDocs(ModuleDefinition, save, ConsoleBool, 2, 2, ())
{
    // Save.
    return object->save();
}

//-----------------------------------------------------------------------------

/*! Gets the module manager which this module definition is registered with (if any).
    @return (moduleManager) The module manager which this module definition is registered with (zero if not registered).
*/
ConsoleMethodWithDocs(ModuleDefinition, getModuleManager, ConsoleInt, 2, 2, ())
{
    // Fetch module manager.
    ModuleManager* pModuleManager = object->getModuleManager();

    return pModuleManager != NULL ? pModuleManager->getId() : 0;
}

//-----------------------------------------------------------------------------

/*! Gets the number of module dependencies this module definition has.
    @return (int count) The number of module dependencies this module definition has.
*/
ConsoleMethodWithDocs(ModuleDefinition, getDependencyCount, ConsoleInt, 2, 2, ())
{
    // Get module dependency count.
    return object->getDependencyCount();
}

//-----------------------------------------------------------------------------

/*! Gets the module dependency at the specified index.
    @param dependencyIndex The module dependency index.
    @return (module-dependency) The module dependency at the specified index.
*/
ConsoleMethodWithDocs(ModuleDefinition, getDependency, ConsoleString, 3, 3, (int dependencyIndex))
{
    // Fetch dependency index.
    const U32 dependencyIndex = (U32)dAtoi(argv[2]);

    // Get module dependency.
    ModuleDefinition::ModuleDependency dependency;
    if ( object->getDependency( dependencyIndex, dependency ) == false )
        return StringTable->EmptyString;

    // Format module dependency.
    char* pReturnBuffer = Con::getReturnBuffer( 256 );
    dSprintf( pReturnBuffer, 256, "%s %d", dependency.mModuleId, dependency.mVersionId );

    return pReturnBuffer;
}

//-----------------------------------------------------------------------------

/*! Adds the specified moduleId and vesionId as a dependency.
    @param moduleId The module Id to add as a dependency.
    @param versionId The version Id to add as a dependency.  Using zero indicates any version.
    @return (bool success) Whether the module dependency was added or not.
*/
ConsoleMethodWithDocs(ModuleDefinition, addDependency, ConsoleBool, 4, 4, (moduleId, versionId))
{
    // Fetch module Id.
    const char* pModuleId = argv[2];

    // Fetch version Id.
    const U32 versionId = (U32)dAtoi(argv[3]);

    // Add dependency.
    return object->addDependency( pModuleId, versionId );
}

//-----------------------------------------------------------------------------

/*! Removes the specified moduleId as a dependency.
    @param moduleId The module Id to remove as a dependency.
    @return (bool success) Whether the module dependency was removed or not.
*/
ConsoleMethodWithDocs(ModuleDefinition, removeDependency, ConsoleBool, 3, 3, (moduleId))
{
    // Fetch module Id.
    const char* pModuleId = argv[2];

    // Remove dependency.
    return object->removeDependency( pModuleId );
}

//-----------------------------------------------------------------------------

/*! Returns the path to the module directory.
    @return A path to the module definition.
*/
ConsoleMethodWithDocs(ModuleDefinition, getModulePath, ConsoleString, 2, 2, ())
{
    return object->getModulePath();
}

ConsoleMethodGroupEndWithDocs(ModuleDefinition)
