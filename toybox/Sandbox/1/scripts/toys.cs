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

Sandbox.ActiveToy = "";

//-----------------------------------------------------------------------------

function scanForToys()
{
    // Find the toy modules.
    %toyModules = ModuleDatabase.findModuleTypes( "toy", false );

    // Do we have an existing set of sandbox toys?
    if ( !isObject(SandboxToys) )
    {
        // No, so create one.
        new SimSet(SandboxToys);
    }
    
    // Clear the sandbox toys.
    SandboxToys.clear();
    
    // Fetch toy module count.
    %toyModuleCount = getWordCount( %toyModules );
    
    // Add toys.
    for ( %i = 0; %i < %toyModuleCount; %i++ )
    {
        // Fetch module definition.
        %moduleDefinition = getWord( %toyModules, %i );
        
        // Add to toy sandbox toys.
        SandboxToys.add( %moduleDefinition );       
    }
}

//-----------------------------------------------------------------------------

function loadToy( %moduleDefinition )
{   
    // Sanity!
    if ( !isObject( %moduleDefinition ) )
    {
        error( "Cannot load toy as the specified toy is not available." );
        return;
    }
            
    // Unload the active toy.
    unloadToy();
        
    // Create a sandbox scene.
    createSandboxScene();
    
    // Now is a good time to purge any idle assets.
    AssetDatabase.purgeAssets();    
        
    // Set active toy.
    // This must be done here in-case a toy depends on it during its initialization.
    Sandbox.ActiveToy = %moduleDefinition;
        
    // Load the toy.
    if ( !ModuleDatabase.loadExplicit( %moduleDefinition.ModuleId, %moduleDefinition.VersionId ) )
    {
        error( "Failed to load the toy '" @ %moduleDefinition.ModuleId @ "'." );
        return;
    }
    
    // Add the scene so it's unloaded when the toy module is.
    %moduleDefinition.ScopeSet.add( SandboxScene );    
    
    // Add toy scope-set as a listener.
    SandboxWindow.addInputListener( %moduleDefinition.ScopeSet );        
}

//-----------------------------------------------------------------------------

function unloadToy()
{
    // Finish if no active toy loaded.
    if ( !isObject(Sandbox.ActiveToy) )
        return;
        
    // Delete any custom controls added by the toy.
    ToyCustomControls.deleteObjects();

    resetCustomControls();

    // Unload the toy.
    if ( !ModuleDatabase.unloadExplicit( Sandbox.ActiveToy.moduleId ) )
    {
        error( "Failed to unload the toy '" @ Sandbox.ActiveToy.ModuleId @ "'." );
    }
    
    // Reset active toy.
    Sandbox.ActiveToy = "";  
}
