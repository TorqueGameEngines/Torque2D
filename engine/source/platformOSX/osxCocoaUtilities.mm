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
#import "platformOSX/platformOSX.h"
#import "platform/nativeDialogs/fileDialog.h"
#import "osxCocoaUtilities.h"

@implementation NSSavePanel (TorqueFileDialogs)

//-----------------------------------------------------------------------------
- (void)setFilters:(StringTableEntry)filters
{
    FileDialogFileTypeList fileTypes (filters);

    NSMutableArray *types = [NSMutableArray arrayWithCapacity:10];

    char *token = fileTypes.filterData;
    char *place = fileTypes.filterData;

    // scan the filter list until we hit a null.
    // when we see the separator '|', replace it with a null, and save the token
    // format is description|extension|description|extension
    bool isDesc = true;
    for (; *place; place++)
    {
        if (*place != '|')
        {
            continue;
        }

        *place = '\0';

        if (isDesc)
        {
            fileTypes.names.push_back(token);
        }
        else
        {
            // detect *.*
            if (dStrstr(token, "*.*"))
            {
                fileTypes.any = true;
            }

            fileTypes.exts.push_back([self getFileExtensionsFromString:token]);
        }


        isDesc = !isDesc;
        ++place;
        token = place;
    }

    fileTypes.exts.push_back([self getFileExtensionsFromString:token]);

    for (U32 i = 0; i < fileTypes.exts.size(); i++)
    {
        for (U32 j = 0; j < fileTypes.exts[i]->list.size(); j++)
        {
            char *ext = fileTypes.exts[i]->list[j];
            if (ext)
            {
                // MP: Added for 1.4.1
                // Passing *.*, *., .*, or just . does nothing on OS X 10.6
                // Manually adding the extensions supported by the engine for now
                if (dStrncmp(ext, "*.*", 3) == 0)
                {
                    [types addObject:[NSString stringWithUTF8String:"png"]];
                    [types addObject:[NSString stringWithUTF8String:"jpg"]];
                    [types addObject:[NSString stringWithUTF8String:"eff"]];
                    [types addObject:[NSString stringWithUTF8String:"lyr"]];
                    [types addObject:[NSString stringWithUTF8String:"gui"]];
                    [types addObject:[NSString stringWithUTF8String:"bmp"]];
                    continue;
                }

                if (dStrncmp(ext, "*.", 2) == 0)
                {
                    ext += 2;
                }

                [types addObject:[NSString stringWithUTF8String:ext]];
            }
        }
    }

    if ([types count] > 0)
    {
        [self setAllowedFileTypes:types];
    }

    if (fileTypes.any)
    {
        [self setAllowsOtherFileTypes:YES];
    }
}

//-----------------------------------------------------------------------------
- (void)setAttributesFromData:(FileDialogData *)data
{
    [self setCanCreateDirectories:YES];
    [self setCanSelectHiddenExtension:YES];
    [self setTreatsFilePackagesAsDirectories:YES];
}

//-----------------------------------------------------------------------------
- (FileDialogFileExtList *)getFileExtensionsFromString:(char const *)filter
{
    FileDialogFileExtList *list = new FileDialogFileExtList(filter);

    char *token = list->data;
    char *place = list->data;

    for (; *place; place++)
    {
        if (*place != ';')
        {
            continue;
        }

        *place = '\0';

        list->list.push_back(token);

        ++place;
        token = place;
    }
    // last token
    list->list.push_back(token);

    return list;
}

+ (NSArray *)showSavePanel:(FileDialogData *)withData
{
    U32 runResult;

    NSMutableArray *array = [NSMutableArray arrayWithCapacity:10];

    NSSavePanel *panel = [NSSavePanel savePanel];

    NSString *dir;

    if (dStrlen(withData->mDefaultPath) < 1)
        dir = [[NSString stringWithUTF8String:withData->mDefaultFile] stringByDeletingLastPathComponent];
    else
        dir = [NSString stringWithUTF8String:withData->mDefaultPath];

    [panel setDirectoryURL:[NSURL fileURLWithPath:dir]];

    [panel setFilters:withData->mFilters];

    runResult = (U32) [panel runModal];

    if (runResult != NSFileHandlingPanelCancelButton)
        [array addObject:[panel URL]];

    return array;
}
@end

@implementation NSOpenPanel (TorqueFileDialogs)

//-----------------------------------------------------------------------------
- (void)setAttributesFromData:(FileDialogData *)data
{
    bool chooseDir = (data->mStyle & FileDialogData::FDS_BROWSEFOLDER);

    [self setCanCreateDirectories:YES];
    [self setCanSelectHiddenExtension:YES];
    [self setTreatsFilePackagesAsDirectories:YES];
    [self setAllowsMultipleSelection:(data->mStyle & FileDialogData::FDS_MULTIPLEFILES)];
    [self setCanChooseFiles:!chooseDir];
    [self setCanChooseDirectories:chooseDir];

    if (chooseDir)
    {
        [self setPrompt:@"Choose"];
        [self setTitle:@"Choose Folder"];
    }
}

//-----------------------------------------------------------------------------
+ (NSArray *)showOpenPanel:(FileDialogData *)withData
{
    U32 runResult;

    NSMutableArray *array = [NSMutableArray arrayWithCapacity:10];

    NSOpenPanel *panel = [NSOpenPanel openPanel];

    [panel setAttributesFromData:withData];

    NSString *dir;

    if (dStrlen(withData->mDefaultPath) < 1)
        dir = [[NSString stringWithUTF8String:withData->mDefaultFile] stringByDeletingLastPathComponent];
    else
        dir = [NSString stringWithUTF8String:withData->mDefaultPath];

    [panel setDirectoryURL:[NSURL fileURLWithPath:dir]];

    [panel setFilters:withData->mFilters];

    runResult = (U32) [panel runModal];

    if (runResult != NSFileHandlingPanelCancelButton)
    {
        if ([panel allowsMultipleSelection])
            [array addObjectsFromArray:[panel URLs]];
        else
            [array addObject:[panel URL]];
    }

    return array;
}

@end

@implementation osxPopupMenuController

@synthesize menu = _menu;
@synthesize menuItem = _menuItem;
@synthesize owner = _owner;

-(id) init
{
    self = [super init];

    if (self)
    {
        _owner = NULL;
        _menu = [[NSMenu allocWithZone:[NSMenu menuZone]] initWithTitle:@"New Menu"];
        _menuItem = [[NSMenuItem allocWithZone:[NSMenu menuZone]] initWithTitle:@"New Menu Item" action:NULL keyEquivalent:@""];
        [_menuItem setSubmenu:_menu];
    }

    return self;
}

-(void) dealloc
{
    if (_menu)
        [_menu release];

    if (_menuItem)
        [_menuItem release];

    [super dealloc];
}

-(void)handleSelect:(id)sender
{
    if(!_owner)
    {
        Con::errorf("osxPopupMenuController::handleSelect error: _owner is invalid");
        return;
    }

    // Get the title
    NSString* itemTitle = [sender title];

    // Get the index
    NSMenu* menu = [sender menu];

    S32 index = (S32)[menu indexOfItemWithTitle:itemTitle];

    _owner->handleSelect(index, [[_menu title] UTF8String]);
}

@end