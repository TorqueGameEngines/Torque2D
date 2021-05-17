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

#include "input/actionMap.h"
#include "platform/event.h"
#include "console/console.h"
#include "platform/platform.h"
#include "platform/platformInput.h"
#include "platform/platformAssert.h"
#include "io/fileStream.h"
#include "io/resource/resourceManager.h"

// Script bindings.
#include "actionMap_ScriptBinding.h"

#define CONST_E 2.7182818284590452353602874f

IMPLEMENT_CONOBJECT(ActionMap);

// This is used for determing keys that have ascii codes for the foreign keyboards. IsAlpha doesn't work on foreign keys.
#define dIsDecentChar(c) (((char(0xa0) <= (c)) && ((c) <= char(0xff))) || (( char(0x21) <= (c)) && ((c) <= char(0x7e))) || (( char(0x91) <= (c)) && ((c) <= char(0x92))))

struct CodeMapping
{
   const char* pDescription;
   U8  type;
   U32 code;
};

struct AsciiMapping
{
   const char* pDescription;
   U16         asciiCode;
};

extern CodeMapping gVirtualMap[];
extern AsciiMapping gAsciiMap[];

//------------------------------------------------------------------------------
//-------------------------------------- Action maps
//
Vector<ActionMap::BreakEntry> ActionMap::smBreakTable(__FILE__, __LINE__);


//------------------------------------------------------------------------------
ActionMap::ActionMap()
{
   VECTOR_SET_ASSOCIATION(mDeviceMaps);
}

//------------------------------------------------------------------------------
ActionMap::~ActionMap()
{
   for (U32 i = 0; i < (U32)mDeviceMaps.size(); i++)
      delete mDeviceMaps[i];
   mDeviceMaps.clear();
}

//------------------------------------------------------------------------------
ActionMap::DeviceMap::~DeviceMap()
{
   for(U32 i = 0; i < (U32)nodeMap.size(); i++)
   {
      dFree(nodeMap[i].makeConsoleCommand);
      dFree(nodeMap[i].breakConsoleCommand);
   }
}

//------------------------------------------------------------------------------
bool ActionMap::onAdd()
{
   if (Parent::onAdd() == false)
      return false;

   Sim::getActionMapGroup()->addObject(this);

   return true;
}

//--------------------------------------------------------------------------
void ActionMap::dumpActionMap(const char* fileName, const bool append) const
{
   if (fileName != NULL) {
      // Dump the deletion, and creation script commands, followed by all the binds
      //  to a script.

      FileStream iostrm;
      if ( !ResourceManager->openFileForWrite( iostrm, fileName, append ? FileStream::WriteAppend : FileStream::Write ) )
      {
         Con::errorf( "Unable to open file '%s' for writing.", fileName );
         return;
      }

      char lineBuffer[1024];
      if ( append )
         iostrm.setPosition( iostrm.getStreamSize() );
      else
      {
         // IMPORTANT -- do NOT change the following line, it identifies the file as an input map file
         dStrcpy( lineBuffer, "// Torque Input Map File\n" );
         iostrm.write( dStrlen( lineBuffer ), lineBuffer );
      }

      dSprintf(lineBuffer, 1023, "if (isObject(%s)) %s.delete();\n"
                                 "new ActionMap(%s);\n", getName(), getName(), getName());
      iostrm.write(dStrlen(lineBuffer), lineBuffer);

      // Dump all the binds to the console...
      for (S32 i = 0; i < mDeviceMaps.size(); i++) {
         const DeviceMap* pDevMap = mDeviceMaps[i];

         char devbuffer[32];
         getDeviceName(pDevMap->deviceType, pDevMap->deviceInst, devbuffer);

         for (S32 j = 0; j < pDevMap->nodeMap.size(); j++) {
            const Node& rNode = pDevMap->nodeMap[j];

            const char* pModifierString = getModifierString(rNode.modifiers);

            char objectbuffer[64];
            if (getKeyString(rNode.action, objectbuffer) == false)
               continue;

            const char* command = (rNode.flags & Node::BindCmd) ? "bindCmd" : "bind";

            dSprintf(lineBuffer, 1023, "%s.%s(%s, \"%s%s\"",
                                        getName(),
                                        command,
                                        devbuffer,
                                        pModifierString, objectbuffer);

            if (rNode.flags & (Node::HasScale|Node::HasDeadZone|Node::Ranged|Node::Inverted)) {
               char buff[10];
               U32 curr = 0;
               buff[curr++] = ',';
               buff[curr++] = ' ';
               if (rNode.flags & Node::HasScale)
                  buff[curr++] = 'S';
               if (rNode.flags & Node::Ranged)
                  buff[curr++] = 'R';
               if (rNode.flags & Node::HasDeadZone)
                  buff[curr++] = 'D';
               if (rNode.flags & Node::Inverted)
                  buff[curr++] = 'I';
               buff[curr] = '\0';

               dStrcat(lineBuffer, buff);
            }

            if (rNode.flags & Node::HasDeadZone) {
               char buff[64];
               dSprintf(buff, 63, ", \"%g %g\"", rNode.deadZoneBegin, rNode.deadZoneEnd);
               dStrcat(lineBuffer, buff);
            }

            if (rNode.flags & Node::HasScale) {
               char buff[64];
               dSprintf(buff, 63, ", %g", rNode.scaleFactor);
               dStrcat(lineBuffer, buff);
            }

            if (rNode.flags & Node::BindCmd) {
               if (rNode.makeConsoleCommand) {
                  dStrcat(lineBuffer, ", \"");
                  U32 pos = dStrlen(lineBuffer);
                  expandEscape(lineBuffer + pos, rNode.makeConsoleCommand);
                  dStrcat(lineBuffer, "\"");
               } else {
                  dStrcat(lineBuffer, ", \"\"");
               }
               if (rNode.breakConsoleCommand) {
                  dStrcat(lineBuffer, ", \"");
                  U32 pos = dStrlen(lineBuffer);
                  expandEscape(lineBuffer + pos, rNode.breakConsoleCommand);
                  dStrcat(lineBuffer, "\"");
               }
               else
                  dStrcat(lineBuffer, ", \"\"");
            } else {
               dStrcat(lineBuffer, ", ");
               dStrcat(lineBuffer, rNode.consoleFunction);
            }

            dStrcat(lineBuffer, ");\n");
            iostrm.write(dStrlen(lineBuffer), lineBuffer);
         }
      }

      iostrm.close();
   }
   else {
      // Dump all the binds to the console...
      for (S32 i = 0; i < mDeviceMaps.size(); i++) {
         const DeviceMap* pDevMap = mDeviceMaps[i];

         char devbuffer[32];
         getDeviceName(pDevMap->deviceType, pDevMap->deviceInst, devbuffer);

         for (S32 j = 0; j < pDevMap->nodeMap.size(); j++) {
            const Node& rNode = pDevMap->nodeMap[j];

            const char* pModifierString = getModifierString(rNode.modifiers);

            char keybuffer[64];
            if (getKeyString(rNode.action, keybuffer) == false)
               continue;

            const char* command = (rNode.flags & Node::BindCmd) ? "bindCmd" : "bind";

            char finalBuffer[1024];
            dSprintf(finalBuffer, 1023, "%s.%s(%s, \"%s%s\"",
                                        getName(),
                                        command,
                                        devbuffer,
                                        pModifierString, keybuffer);

            if (rNode.flags & (Node::HasScale|Node::HasDeadZone|Node::Ranged|Node::Inverted)) {
               char buff[10];
               U32 curr = 0;
               buff[curr++] = ',';
               buff[curr++] = ' ';
               if (rNode.flags & Node::HasScale)
                  buff[curr++] = 'S';
               if (rNode.flags & Node::Ranged)
                  buff[curr++] = 'R';
               if (rNode.flags & Node::HasDeadZone)
                  buff[curr++] = 'D';
               if (rNode.flags & Node::Inverted)
                  buff[curr++] = 'I';
               buff[curr] = '\0';

               dStrcat(finalBuffer, buff);
            }

            if (rNode.flags & Node::HasDeadZone) {
               char buff[64];
               dSprintf(buff, 63, ", \"%g %g\"", rNode.deadZoneBegin, rNode.deadZoneEnd);
               dStrcat(finalBuffer, buff);
            }

            if (rNode.flags & Node::HasScale) {
               char buff[64];
               dSprintf(buff, 63, ", %g", rNode.scaleFactor);
               dStrcat(finalBuffer, buff);
            }

            if (rNode.flags & Node::BindCmd) {
               if (rNode.makeConsoleCommand) {
                  dStrcat(finalBuffer, ", \"");
                  dStrcat(finalBuffer, rNode.makeConsoleCommand);
                  dStrcat(finalBuffer, "\"");
               } else {
                  dStrcat(finalBuffer, ", \"\"");
               }
               if (rNode.breakConsoleCommand) {
                  dStrcat(finalBuffer, ", \"");
                  dStrcat(finalBuffer, rNode.breakConsoleCommand);
                  dStrcat(finalBuffer, "\"");
               }
               else
                  dStrcat(finalBuffer, ", \"\"");
            } else {
               dStrcat(finalBuffer, ", ");
               dStrcat(finalBuffer, rNode.consoleFunction);
            }

            dStrcat(finalBuffer, ");");
            Con::printf(finalBuffer);
         }
      }
   }
}

//--------------------------------------------------------------------------
bool ActionMap::createEventDescriptor(const char* pEventString, EventDescriptor* pDescriptor)
{
   char copyBuffer[256];
   dStrcpy(copyBuffer, pEventString);

   // Do we have modifiers?
   char* pSpace = dStrchr(copyBuffer, ' ');
   char* pObjectString;
   if (pSpace != NULL) {
      // Yes.  Parse them out...
      //
      pDescriptor->flags = 0;
      pObjectString      = pSpace + 1;
      pSpace[0]          = '\0';

      char* pModifier = dStrtok(copyBuffer, "-");
      while (pModifier != NULL) {
         if (dStricmp(pModifier, "shift") == 0) {
            pDescriptor->flags |= SI_SHIFT;
         } else if (dStricmp(pModifier, "lshift") == 0) {
            pDescriptor->flags |= SI_LSHIFT;
         } else if (dStricmp(pModifier, "rshift") == 0) {
            pDescriptor->flags |= SI_RSHIFT;
         } else if (dStricmp(swapCtrlForCmd(pModifier), "ctrl") == 0) {
            pDescriptor->flags |= SI_CTRL;
         } else if (dStricmp(pModifier, "lctrl") == 0) {
            pDescriptor->flags |= SI_LCTRL;
         } else if (dStricmp(pModifier, "rctrl") == 0) {
            pDescriptor->flags |= SI_RCTRL;
         } else if (dStricmp(pModifier, "alt") == 0) {
            pDescriptor->flags |= SI_ALT;
         } else if (dStricmp(pModifier, "lalt") == 0) {
            pDescriptor->flags |= SI_LALT;
         } else if (dStricmp(pModifier, "ralt") == 0) {
            pDescriptor->flags |= SI_RALT;
         } else if (dStricmp(swapCtrlForCmd(pModifier), "cmd") == 0) {
            pDescriptor->flags |= SI_ALT;
         } else if (dStricmp(pModifier, "opt") == 0) {
            pDescriptor->flags |= SI_MAC_OPT;
         } else if (dStricmp(pModifier, "lopt") == 0) {
            pDescriptor->flags |= SI_MAC_LOPT;
         } else if (dStricmp(pModifier, "ropt") == 0) {
            pDescriptor->flags |= SI_MAC_ROPT;
         }

         pModifier = dStrtok(NULL, "-");
      }
   } else {
      // No.
      pDescriptor->flags = 0;
      pObjectString      = copyBuffer;
   }

   // Now we need to map the key string to the proper KEY code from event.h
   //
   AssertFatal(dStrlen(pObjectString) != 0, "Error, no key was specified!");

   if (dStrlen(pObjectString) == 1)
   {
      if (dIsDecentChar(*pObjectString)) // includes foreign chars
      {
         U16 asciiCode = (*pObjectString);
         // clear out the FF in upper 8bits for foreign keys??
         asciiCode &= 0xFF;
         U16 keyCode = Input::getKeyCode(asciiCode);
         if ( keyCode >= KEY_0 )
         {
            pDescriptor->eventType = SI_KEY;
            pDescriptor->eventCode = keyCode;
            return true;
         }
         else if (dIsalpha(*pObjectString) == true)
         {
            pDescriptor->eventType = SI_KEY;
            pDescriptor->eventCode = KEY_A+dTolower(*pObjectString)-'a';
            return true;
         }
         else if (dIsdigit(*pObjectString) == true)
         {
            pDescriptor->eventType = SI_KEY;
            pDescriptor->eventCode = KEY_0+(*pObjectString)-'0';
            return true;
         }
      }
      return false;
   }
   else
   {
      pDescriptor->eventCode = 0;
      // Gotta search through the Ascii table...
      for (U16 i = 0; gAsciiMap[i].asciiCode != 0xFFFF; i++)
      {
         if (dStricmp(pObjectString, gAsciiMap[i].pDescription) == 0)
         {
            U16 asciiCode = gAsciiMap[i].asciiCode;
            U16 keyCode   = Input::getKeyCode(asciiCode);
            if ( keyCode >= KEY_0 )
            {
               pDescriptor->eventType = SI_KEY;
               pDescriptor->eventCode = keyCode;
               return(true);

            }
            else
            {
               break;
            }
         }
      }
      // Didn't find an ascii match. Check the virtual map table
      for (U32 j = 0; gVirtualMap[j].code != 0xFFFFFFFF; j++)
      {
         if (dStricmp(pObjectString, gVirtualMap[j].pDescription) == 0)
         {
            pDescriptor->eventType = gVirtualMap[j].type;
            pDescriptor->eventCode = gVirtualMap[j].code;
            return true;
         }
      }
   }
   return false;
}

const char* ActionMap::swapCtrlForCmd(const char* input)
{
#ifdef TORQUE_OS_OSX
	return dStrcmp(input, "Ctrl") == 0 ? "Cmd" : input;
#endif
	return input;
}

//------------------------------------------------------------------------------
ActionMap::Node* ActionMap::getNode(const U32 inDeviceType, const U32 inDeviceInst,
                   const U32 inModifiers,  const U32 inAction,SimObject* object /*= NULL*/)
{
   // DMMTODO - Slow INITIAL implementation.  Replace with a faster version...
   //
   DeviceMap* pDeviceMap = NULL;
   U32 i;
   for (i = 0; i < (U32)mDeviceMaps.size(); i++) 
   {
      if (mDeviceMaps[i]->deviceType == inDeviceType &&
          mDeviceMaps[i]->deviceInst == inDeviceInst) {
         pDeviceMap = mDeviceMaps[i];
         break;
      }
   }
   if (pDeviceMap == NULL) 
   {
      mDeviceMaps.increment();
      mDeviceMaps.last() = new DeviceMap;
      pDeviceMap = mDeviceMaps.last();

      pDeviceMap->deviceInst = inDeviceInst;
      pDeviceMap->deviceType = inDeviceType;
   }

   for (i = 0; i < (U32)pDeviceMap->nodeMap.size(); i++) 
   {
      if (pDeviceMap->nodeMap[i].modifiers == inModifiers &&
          pDeviceMap->nodeMap[i].action    == inAction &&
          ( (object != NULL) ? object == pDeviceMap->nodeMap[i].object : true )) // Check for an object match if the object exists 
      {
         return &pDeviceMap->nodeMap[i];
      }
   }

   // If we're here, the node doesn't exist.  create it.
   pDeviceMap->nodeMap.increment();

   Node* pRetNode = &pDeviceMap->nodeMap.last();
   pRetNode->modifiers = inModifiers;
   pRetNode->action    = inAction;

   pRetNode->flags         = 0;
   pRetNode->deadZoneBegin = 0.0;
   pRetNode->deadZoneEnd   = 0.0;
   pRetNode->scaleFactor   = 1.0;

   pRetNode->consoleFunction = NULL;
   pRetNode->makeConsoleCommand = NULL;
   pRetNode->breakConsoleCommand = NULL;

   //[neob, 5/7/2007 - #2975]
   pRetNode->object = 0;

   return pRetNode;
}

//------------------------------------------------------------------------------
void ActionMap::removeNode(const U32 inDeviceType, const U32 inDeviceInst, const U32 inModifiers, const U32 inAction, SimObject* object /*= NULL*/)
{
   // DMMTODO - Slow INITIAL implementation.  Replace with a faster version...
   //
   DeviceMap* pDeviceMap = NULL;
   U32 i;
   for (i = 0; i < (U32)mDeviceMaps.size(); i++) {
      if (mDeviceMaps[i]->deviceType == inDeviceType &&
          mDeviceMaps[i]->deviceInst == inDeviceInst) {
         pDeviceMap = mDeviceMaps[i];
         break;
      }
   }

   if (pDeviceMap == NULL)
      return;

   U32 realMods = inModifiers;
   if (realMods & SI_SHIFT)
      realMods |= SI_SHIFT;
   if (realMods & SI_CTRL)
      realMods |= SI_CTRL;
   if (realMods & SI_ALT)
      realMods |= SI_ALT;
   if (realMods & SI_MAC_OPT)
      realMods |= SI_MAC_OPT;

   for (i = 0; i < (U32)pDeviceMap->nodeMap.size(); i++) {
      if (pDeviceMap->nodeMap[i].modifiers == realMods &&
          pDeviceMap->nodeMap[i].action    == inAction &&
          ( (object != NULL) ? object == pDeviceMap->nodeMap[i].object : true )) 
      {
          dFree(pDeviceMap->nodeMap[i].makeConsoleCommand);
          dFree(pDeviceMap->nodeMap[i].breakConsoleCommand);
          pDeviceMap->nodeMap.erase(i);
      }
   }
}

//------------------------------------------------------------------------------
const ActionMap::Node* ActionMap::findNode(const U32 inDeviceType, const U32 inDeviceInst,
                    const U32 inModifiers,  const U32 inAction)
{
   // DMMTODO - Slow INITIAL implementation.  Replace with a faster version...
   //
   DeviceMap* pDeviceMap = NULL;
   U32 i;
   for (i = 0; i < (U32)mDeviceMaps.size(); i++)
   {
      if (mDeviceMaps[i]->deviceType == inDeviceType && mDeviceMaps[i]->deviceInst == inDeviceInst)
      {
         pDeviceMap = mDeviceMaps[i];
         break;
      }
   }

   if (pDeviceMap == NULL)
      return NULL;

   U32 realMods = inModifiers;
   if (realMods & SI_SHIFT)
      realMods |= SI_SHIFT;
   if (realMods & SI_CTRL)
      realMods |= SI_CTRL;
   if (realMods & SI_ALT)
      realMods |= SI_ALT;
   if (realMods & SI_MAC_OPT)
      realMods |= SI_MAC_OPT;

   for (i = 0; i < (U32)pDeviceMap->nodeMap.size(); i++)
   {
       if (pDeviceMap->nodeMap[i].action == KEY_ANYKEY && pDeviceMap->nodeMap[i].modifiers == realMods && dIsDecentChar(inAction))
           return &pDeviceMap->nodeMap[i];
       else if (inModifiers == 0 && pDeviceMap->nodeMap[i].modifiers == 0 && pDeviceMap->nodeMap[i].action == inAction)
           return &pDeviceMap->nodeMap[i];
       else if (pDeviceMap->nodeMap[i].modifiers == realMods && pDeviceMap->nodeMap[i].action    == inAction)
           return &pDeviceMap->nodeMap[i];
   }

   return NULL;
}

//------------------------------------------------------------------------------
bool ActionMap::findBoundNode( const char* function, U32 &devMapIndex, U32 &nodeIndex )
{
   devMapIndex = 0;
   nodeIndex = 0;
   return nextBoundNode( function, devMapIndex, nodeIndex );
}

bool ActionMap::nextBoundNode( const char* function, U32 &devMapIndex, U32 &nodeIndex )
{
   // Loop through all of the existing nodes to find the one mapped to the
   // given function:
   for ( U32 i = devMapIndex; i < (U32)mDeviceMaps.size(); i++ )
   {
      const DeviceMap* dvcMap = mDeviceMaps[i];

      for ( U32 j = nodeIndex; j < (U32)dvcMap->nodeMap.size(); j++ )
      {
         const Node* node = &dvcMap->nodeMap[j];
         if ( !( node->flags & Node::BindCmd ) && ( dStricmp( function, node->consoleFunction ) == 0 ) )
         {
            devMapIndex = i;
            nodeIndex = j;
            return( true );
         }
      }

      nodeIndex = 0;
   }

   return( false );
}

//------------------------------------------------------------------------------
bool ActionMap::processUnbind(const char *device, const char *action, SimObject* object /*= NULL*/)
{
   U32 deviceType;
   U32 deviceInst;

   if(!getDeviceTypeAndInstance(device, deviceType, deviceInst))
      return false;
   EventDescriptor eventDescriptor;
   if (!createEventDescriptor(action, &eventDescriptor))
      return false;

   removeNode(deviceType, deviceInst, eventDescriptor.flags,eventDescriptor.eventCode, object);
   return true;
}

//------------------------------------------------------------------------------
// This function is for the use of the control remapper.
// It will only check against the console function (since all remappable commands are
// bound using bind and not bindCmd).
//
const char* ActionMap::getBinding( const char* command )
{
   char* returnString = Con::getReturnBuffer( 1024 );
   returnString[0] = 0;

   char buffer[256];
   char deviceBuffer[32];
   char keyBuffer[64];
 
   U32 devMapIndex = 0, nodeIndex = 0;
   while ( nextBoundNode( command, devMapIndex, nodeIndex ) )
   {
      const DeviceMap* deviceMap = mDeviceMaps[devMapIndex];

      if ( getDeviceName( deviceMap->deviceType, deviceMap->deviceInst, deviceBuffer ) )
      {
         const Node* node = &deviceMap->nodeMap[nodeIndex];
         const char* modifierString = getModifierString( node->modifiers );

         if ( getKeyString( node->action, keyBuffer ) )
         {
            dSprintf( buffer, sizeof( buffer ), "%s\t%s%s", deviceBuffer, modifierString, keyBuffer );
            if ( returnString[0] )
               dStrcat( returnString, "\t" );
            dStrcat( returnString, buffer );
         }
      }

      ++nodeIndex;
   }

   return returnString;
}

//------------------------------------------------------------------------------
// This function is for the use of the control remapper.
// The intent of this function is to determine if the given event descriptor is already
// bound in this action map.  If so, this function returns the command it is bound to.
// If not, it returns NULL.
//
const char* ActionMap::getCommand( const char* device, const char* action )
{
    U32 deviceType;
    U32 deviceInst;
    if ( getDeviceTypeAndInstance( device, deviceType, deviceInst ) )
    {
        EventDescriptor eventDescriptor;
        if ( createEventDescriptor( action, &eventDescriptor ) )
        {
            const ActionMap::Node* mapNode = findNode( deviceType, deviceInst, eventDescriptor.flags, eventDescriptor.eventCode );
            if ( mapNode )
            {
                if ( mapNode->flags & Node::BindCmd )
                {
                    S32 bufferLen = dStrlen( mapNode->makeConsoleCommand ) + dStrlen( mapNode->breakConsoleCommand ) + 2;
                    char* returnString = Con::getReturnBuffer( bufferLen );
                    dSprintf( returnString, bufferLen, "%s\t%s",
                            ( mapNode->makeConsoleCommand ? mapNode->makeConsoleCommand : "" ),
                            ( mapNode->breakConsoleCommand ? mapNode->breakConsoleCommand : "" ) );					
                    return( returnString );
                }					
                else
                    return( mapNode->consoleFunction );					
            }
        }
    }

    return( "" );
}

//------------------------------------------------------------------------------
// This function returns whether or not the mapping specified is inverted.
// Obviously, this should only be used for axes.
bool ActionMap::isInverted( const char* device, const char* action )
{
    U32 deviceType;
    U32 deviceInst;
    if ( getDeviceTypeAndInstance( device, deviceType, deviceInst ) )
    {
        EventDescriptor eventDescriptor;
        if ( createEventDescriptor( action, &eventDescriptor ) )
        {
            const ActionMap::Node* mapNode = findNode( deviceType, deviceInst, eventDescriptor.flags, eventDescriptor.eventCode );
            if ( mapNode )
                return( mapNode->flags & Node::Inverted );
        }
    }

    Con::errorf( "The input event specified by %s %s is not in this action map!", device, action );
    return( false );
}

//------------------------------------------------------------------------------
F32 ActionMap::getScale( const char* device, const char* action )
{
    U32 deviceType;
    U32 deviceInst;
    if ( getDeviceTypeAndInstance( device, deviceType, deviceInst ) )
    {
        EventDescriptor eventDescriptor;
        if ( createEventDescriptor( action, &eventDescriptor ) )
        {
            const ActionMap::Node* mapNode = findNode( deviceType, deviceInst, eventDescriptor.flags, eventDescriptor.eventCode );
            if ( mapNode )
            {
               if ( mapNode->flags & Node::HasScale )
                   return( mapNode->scaleFactor );
            else
               return( 1.0f );
         }
        }
    }

    Con::errorf( "The input event specified by %s %s is not in this action map!", device, action );
    return( 1.0f );
}

//------------------------------------------------------------------------------
const char* ActionMap::getDeadZone( const char* device, const char* action )
{
    U32 deviceType;
    U32 deviceInst;
    if ( getDeviceTypeAndInstance( device, deviceType, deviceInst ) )
    {
        EventDescriptor eventDescriptor;
        if ( createEventDescriptor( action, &eventDescriptor ) )
        {
            const ActionMap::Node* mapNode = findNode( deviceType, deviceInst, eventDescriptor.flags, eventDescriptor.eventCode );
            if ( mapNode )
            {
               if ( mapNode->flags & Node::HasDeadZone )
               {
                   char buf[64];
                   dSprintf( buf, sizeof( buf ), "%g %g", mapNode->deadZoneBegin, mapNode->deadZoneEnd );
                   char* returnString = Con::getReturnBuffer( dStrlen( buf ) + 1 );
                   dStrcpy( returnString, buf );
                   return( returnString );
                }
                else
                   return( "0 0" );				   		
            }
        }
    }

    Con::errorf( "The input event specified by %s %s is not in this action map!", device, action );
    return( "" );
}

//------------------------------------------------------------------------------
const char* ActionMap::buildActionString( const InputEvent* event )
{
    const char* modifierString = getModifierString( event->modifier );

    char objectBuffer[64];
    if ( !getKeyString( event->objInst, objectBuffer ) )
        return( "" );

    U32 returnLen = dStrlen( modifierString ) + dStrlen( objectBuffer ) + 2;	
    char* returnString = Con::getReturnBuffer( returnLen );
    dSprintf( returnString, returnLen - 1, "%s%s", modifierString, objectBuffer );
    return( returnString );
}

//------------------------------------------------------------------------------
bool ActionMap::getDeviceTypeAndInstance(const char *pDeviceName, U32 &deviceType, U32 &deviceInstance)
{
   U32 offset = 0;
    
   if (dStrnicmp(pDeviceName, "keyboard", dStrlen("keyboard")) == 0)
   {
      deviceType = KeyboardDeviceType;
      offset = dStrlen("keyboard");
   } 
   else if (dStrnicmp(pDeviceName, "mouse", dStrlen("mouse")) == 0) 
   {
      deviceType = MouseDeviceType;
      offset = dStrlen("mouse");
   } 
   else if (dStrnicmp(pDeviceName, "joystick", dStrlen("joystick")) == 0) 
   {
      deviceType = JoystickDeviceType;
      offset = dStrlen("joystick");
   } 
   else if (dStrnicmp(pDeviceName, "accelerometer", dStrlen("accelerometer")) == 0) 
   {
      deviceType = AccelerometerDeviceType;
      offset = dStrlen("accelerometer");
   }
   else if (dStrnicmp(pDeviceName, "gyroscope", dStrlen("gyroscope")) == 0)
   {
       deviceType = GyroscopeDeviceType;
       offset = dStrlen("gyroscope");
   }
   else if (dStrnicmp(pDeviceName, "touchdevice", dStrlen("touchdevice")) == 0)
   {
       deviceType = ScreenTouchDeviceType;
       offset = dStrlen("touchdevice");
   }
    else if (dStrnicmp(pDeviceName, "gamepad", dStrlen("gamepad")) == 0)
   {
      deviceType = GamepadDeviceType;
      offset     = dStrlen("gamepad");
   }
   else
      return false;
    
   if (dStrlen(pDeviceName) > offset) 
   {
      const char* pInst = pDeviceName + offset;
      S32 instNum = dAtoi(pInst);
      
      if (instNum < 0)
         deviceInstance = 0;
      else
         deviceInstance = instNum;
   } 
   else 
       deviceInstance = 0;
    
   return true;
}

//------------------------------------------------------------------------------
bool ActionMap::getDeviceName(const U32 deviceType, const U32 deviceInstance, char* buffer)
{
   switch (deviceType)
   {
     case KeyboardDeviceType:
      dStrcpy(buffer, "keyboard");
      break;

     case MouseDeviceType:
      dSprintf(buffer, 16, "mouse%d", deviceInstance);
      break;

     case JoystickDeviceType:
      dSprintf(buffer, 16, "joystick%d", deviceInstance);
      break;
    
     case AccelerometerDeviceType:
     dStrcpy(buffer, "accelerometer");
     break;
    
     case GyroscopeDeviceType:
     dStrcpy(buffer, "gyroscope");
     break;
     
     case ScreenTouchDeviceType:
     dStrcpy(buffer, "touchdevice");
     break;

	  case GamepadDeviceType:
      dSprintf(buffer, 16, "gamepad%d", deviceInstance);
      break;

     default:
      Con::errorf( "ActionMap::getDeviceName: unknown device type specified, %d (inst: %d)", deviceType, deviceInstance);
      return false;
   }

   return true;
}

//------------------------------------------------------------------------------
const char* ActionMap::getModifierString(const U32 modifiers)
{
    U32 realModifiers = modifiers;
    if ( modifiers & SI_LSHIFT || modifiers & SI_RSHIFT )
        realModifiers |= SI_SHIFT;
    if ( modifiers & SI_LCTRL || modifiers & SI_RCTRL )
        realModifiers |= SI_CTRL;
    if ( modifiers & SI_LALT || modifiers & SI_RALT )
        realModifiers |= SI_ALT;
    if ( modifiers & SI_MAC_LOPT || modifiers & SI_MAC_ROPT )
        realModifiers |= SI_MAC_OPT;

   switch (realModifiers & (SI_SHIFT|SI_CTRL|SI_ALT|SI_MAC_OPT)) {
#if defined(TORQUE_OS_OSX)
      // optional code, to output alt as cmd on mac.
      // interpreter sees them as the same...
     case (SI_SHIFT|SI_CTRL|SI_ALT):
      return "cmd-shift-ctrl ";

     case (SI_SHIFT|SI_ALT):
      return "cmd-shift ";

     case (SI_CTRL|SI_ALT):
      return "cmd-ctrl ";

     case (SI_ALT):
      return "cmd ";
#else
     case (SI_SHIFT|SI_CTRL|SI_ALT):
      return "shift-ctrl-alt ";

     case (SI_SHIFT|SI_ALT):
      return "shift-alt ";

     case (SI_CTRL|SI_ALT):
      return "ctrl-alt ";

     case (SI_ALT):
      return "alt ";
#endif
     case (SI_SHIFT|SI_CTRL):
      return "shift-ctrl ";

     case (SI_SHIFT):
      return "shift ";

     case (SI_CTRL):
      return "ctrl ";

// plus new mac cases:
     case (SI_ALT|SI_SHIFT|SI_CTRL|SI_MAC_OPT):
      return "cmd-shift-ctrl-opt ";

     case (SI_ALT|SI_SHIFT|SI_MAC_OPT):
      return "cmd-shift-opt ";

     case (SI_ALT|SI_CTRL|SI_MAC_OPT):
      return "cmd-ctrl-opt ";

     case (SI_ALT|SI_MAC_OPT):
      return "cmd-opt ";

     case (SI_SHIFT|SI_CTRL|SI_MAC_OPT):
      return "shift-ctrl-opt ";

     case (SI_SHIFT|SI_MAC_OPT):
      return "shift-opt ";

     case (SI_CTRL|SI_MAC_OPT):
      return "ctrl-opt ";

     case (SI_MAC_OPT):
      return "opt ";
      
     case 0:
      return "";

     default:
      AssertFatal(false, "Error, should never reach the default case in getModifierString");
      return "";
   }
}

//------------------------------------------------------------------------------
bool ActionMap::getKeyString(const U32 action, char* buffer)
{
   U16 asciiCode = Input::getAscii(action, STATE_LOWER);

//   if (action >= KEY_A && action <= KEY_Z) {
//      buffer[0] = char(action - KEY_A + 'a');
//      buffer[1] = '\0';
//      return true;
//   } else if (action >= KEY_0 && action <= KEY_9) {
//      buffer[0] = char(action - KEY_0 + '0');
//      buffer[1] = '\0';
   if ( (asciiCode != 0) && dIsDecentChar((char)asciiCode))
   {
      for (U32 i = 0; gAsciiMap[i].asciiCode != 0xFFFF; i++) {
         if (gAsciiMap[i].asciiCode == asciiCode)
         {
            dStrcpy(buffer, gAsciiMap[i].pDescription);
            return true;
         }
      }
      // Must not have found a string for that ascii code just record the char
      buffer[0] = char(asciiCode);
      buffer[1] = '\0';
      return true;
   }
   else
   {
      if (action >= KEY_A && action <= KEY_Z)
      {
         buffer[0] = char(action - KEY_A + 'a');
         buffer[1] = '\0';
         return true;
      }
      else if (action >= KEY_0 && action <= KEY_9) {
         buffer[0] = char(action - KEY_0 + '0');
         buffer[1] = '\0';
         return true;
      }
      for (U32 i = 0; gVirtualMap[i].code != 0xFFFFFFFF; i++) {
         if (gVirtualMap[i].code == action) {
            dStrcpy(buffer, gVirtualMap[i].pDescription);
            return true;
         }
      }
   }

   Con::errorf( "ActionMap::getKeyString: no string for action %d", action );
   return false;
}

//--------------------------------------------------------------------------
bool ActionMap::processBindCmd(const char *device, const char *action, const char *makeCmd, const char *breakCmd)
{
   U32 deviceType;
   U32 deviceInst;

   if(!getDeviceTypeAndInstance(device, deviceType, deviceInst))
   {
      Con::printf("processBindCmd: unknown device: %s", device);
      return false;
   }

   // Ok, we now have the deviceType and instance.  Create an event descriptor
   //  for the bind...
   //
   EventDescriptor eventDescriptor;
   if (createEventDescriptor(action, &eventDescriptor) == false) {
      Con::printf("Could not create a description for binding: %s", action);
      return false;
   }

   // SI_POV == SI_MOVE, and the POV works fine with bindCmd, so we have to add these manually.
   if( ( eventDescriptor.eventCode == SI_XAXIS )    ||
       ( eventDescriptor.eventCode == SI_YAXIS )    ||
       ( eventDescriptor.eventCode == SI_ZAXIS )    ||
       ( eventDescriptor.eventCode == SI_RXAXIS )   ||
       ( eventDescriptor.eventCode == SI_RYAXIS )   ||
       ( eventDescriptor.eventCode == SI_RZAXIS )   ||
       ( eventDescriptor.eventCode == SI_SLIDER )   ||
       ( eventDescriptor.eventCode == SI_XPOV )     ||
       ( eventDescriptor.eventCode == SI_YPOV )     ||
       ( eventDescriptor.eventCode == SI_XPOV2 )    ||
       ( eventDescriptor.eventCode == SI_YPOV2 )    ||
       ( eventDescriptor.eventCode == SI_ACCELX )   ||
       ( eventDescriptor.eventCode == SI_ACCELY )   ||
       ( eventDescriptor.eventCode == SI_ACCELZ )   ||
       ( eventDescriptor.eventCode == SI_GRAVX )    ||
       ( eventDescriptor.eventCode == SI_GRAVY )    ||
       ( eventDescriptor.eventCode == SI_GRAVZ )    ||
       ( eventDescriptor.eventCode == SI_GYROX )    ||
       ( eventDescriptor.eventCode == SI_GYROY )    ||
       ( eventDescriptor.eventCode == SI_GYROZ )    ||
       ( eventDescriptor.eventCode == SI_YAW )      ||
       ( eventDescriptor.eventCode == SI_PITCH )    ||
       ( eventDescriptor.eventCode == SI_ROLL ) )	
   {
      Con::warnf( "ActionMap::processBindCmd - Cannot use 'bindCmd' with a move event type. Use 'bind' instead." );
      return false;
   }

   // Create the full bind entry, and place it in the map
   //
   // DMMTODO
   Node* pBindNode = getNode(deviceType, deviceInst,
                             eventDescriptor.flags,
                             eventDescriptor.eventCode);

   pBindNode->flags           = Node::BindCmd;
   pBindNode->deadZoneBegin   = 0;
   pBindNode->deadZoneEnd     = 0;
   pBindNode->scaleFactor     = 1;
   if(makeCmd[0])
      pBindNode->makeConsoleCommand = dStrdup(makeCmd);
   else
      pBindNode->makeConsoleCommand = dStrdup("");

   if(breakCmd[0])
      pBindNode->breakConsoleCommand = dStrdup(breakCmd);
   else
      pBindNode->breakConsoleCommand = dStrdup("");
   return true;
}

//------------------------------------------------------------------------------
bool ActionMap::processBind(const U32 argc, const char** argv, SimObject* object)
{
   // Ok, the bind will come in the following format:
   //  [device] [key or button] <[param spec] [param] ...> [fnName]
   //
   const char* pDeviceName = argv[0];
   const char* pEvent      = argv[1];
   const char* pFnName     = argv[argc - 1];

   // Determine the device
   U32 deviceType;
   U32 deviceInst;

   if(!getDeviceTypeAndInstance(argv[0], deviceType, deviceInst))
   {
      Con::printf("processBind: unknown device: %s", pDeviceName);
      return false;
   }

   // Ok, we now have the deviceType and instance.  Create an event descriptor
   //  for the bind...
   //
   EventDescriptor eventDescriptor;
   if (createEventDescriptor(pEvent, &eventDescriptor) == false) {
      Con::printf("Could not create a description for binding: %s", pEvent);
      return false;
   }

   // Event has now been described, and device determined.  we need now to extract
   //  any modifiers that the action map will apply to incoming events before
   //  calling the bound function...
   //
   // DMMTODO
   U32 assignedFlags = 0;
   F32 deadZoneBegin = 0.0f;
   F32 deadZoneEnd   = 0.0f;
   F32 scaleFactor   = 1.0f;

   if (argc != 3) {
      // We have the following: "[DSIR]" [deadZone] [scale]
      //
      const char* pSpec = argv[2];

      for (U32 i = 0; pSpec[i] != '\0'; i++) {
         switch (pSpec[i]) {
           case 'r': case 'R':
            assignedFlags |= Node::HasScale;
            break;
           case 's': case 'S':
            assignedFlags |= Node::HasScale;
            break;
           case 'd': case 'D':
            assignedFlags |= Node::HasDeadZone;
            break;
           case 'i': case 'I':
            assignedFlags |= Node::Inverted;
            break;
			case 'n': case 'N':
            assignedFlags |= Node::NonLinear;
            break;

           default:
            AssertFatal(false, avar("Misunderstood specifier in bind (spec string: %s)",
                                    pSpec));
         }
      }

      // Ok, we have the flags.  Scan the dead zone and scale, if any.
      //
      U32 curArg = 3;
      if (assignedFlags & Node::HasDeadZone) {
         dSscanf(argv[curArg], "%g %g", &deadZoneBegin, &deadZoneEnd);
         curArg++;
      }
      if (assignedFlags & Node::HasScale) {
         scaleFactor = dAtof(argv[curArg]);
         curArg++;
      }

      if (curArg != (argc - 1)) {
         AssertFatal(curArg == (argc - 1), "error in bind spec somewhere...");
         Con::printf("Improperly specified bind for key: %s", argv[2]);
         return false;
      }
   }

   // Ensure that the console function is properly specified?
   //
   // DMMTODO

   // Create the full bind entry, and place it in the map
   //
   // DMMTODO
   Node* pBindNode = getNode(deviceType, deviceInst,
                             eventDescriptor.flags,
                             eventDescriptor.eventCode, object);

   pBindNode->flags           = assignedFlags;
   pBindNode->deadZoneBegin   = deadZoneBegin;
   pBindNode->deadZoneEnd     = deadZoneEnd;
   pBindNode->scaleFactor     = scaleFactor;
   pBindNode->object          = object;
   pBindNode->consoleFunction = StringTable->insert(pFnName);

   return true;
}

//------------------------------------------------------------------------------

bool ActionMap::processGesture(const InputEvent* pEvent)
{
    static const char *argv[6];
    char buffer[64];

    const Node* pNode = findNode( pEvent->deviceType, pEvent->deviceInst, pEvent->modifier, pEvent->objType );

    if (pNode == NULL) 
    {
        // Check to see if we clear the modifiers, do we find an action?
        if (pEvent->modifier != 0)
            pNode = findNode(pEvent->deviceType, pEvent->deviceInst, 0, pEvent->objInst);
           
        if (pNode == NULL)
            return false;
    }

    // "Do nothing" bind:
    if ( !pNode->consoleFunction[0] )
        return( true );

    // Function
    argv[0] = pNode->consoleFunction;
    
    switch(pEvent->objType)
    {
        case SI_CIRCLE_GESTURE:

            // ID
            argv[1] = Con::getIntArg(pEvent->iValue);

            // Progress
            argv[2] = Con::getFloatArg(pEvent->fValues[0]);

            // Radius
            argv[3] = Con::getFloatArg(pEvent->fValues[1]);

            // Direction (1 clockwise, 0 counter-clockwise)
            argv[4] = Con::getFloatArg(pEvent->fValues[2]);

            // State
            argv[5] = Con::getFloatArg(pEvent->fValues[3]);

            if (pNode->object)
                Con::executef(pNode->object, 6, argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
            else
                Con::execute(6, argv);
            break;

        case SI_SWIPE_GESTURE:

            // ID
            argv[1] = Con::getIntArg(pEvent->iValue);

            // State
            argv[2] = Con::getFloatArg(pEvent->fValues[0]);

            // Direction
            dSprintf(buffer, sizeof(buffer), "%f %f %f", pEvent->fValues[1], pEvent->fValues[2], pEvent->fValues[3]);

            argv[3] = buffer;
            // Speed
            argv[4] = Con::getFloatArg(pEvent->fValues[4]);

            if (pNode->object)
                Con::executef(pNode->object, 5, argv[0], argv[1], argv[2], argv[3], argv[4]);
            else
                Con::execute(5, argv);
            break;

        case SI_KEYTAP_GESTURE:
        case SI_SCREENTAP_GESTURE:

            // ID
            argv[1] = Con::getIntArg(pEvent->iValue);
        
            // Position
            dSprintf(buffer, sizeof(buffer), "%f %f %f", pEvent->fValues[0], pEvent->fValues[1], pEvent->fValues[2]);

            argv[2] = buffer;

            // Direction
            dSprintf(buffer, sizeof(buffer), "%f %f %f", pEvent->fValues[3], pEvent->fValues[4], pEvent->fValues[5]);

            argv[3] = buffer;

            if (pNode->object)
                Con::executef(pNode->object, 4, argv[0], argv[1], argv[2], argv[3]);
            else
                Con::execute(5, argv);

            break;

        case SI_PINCH_GESTURE:
        case SI_SCALE_GESTURE:
        default:
            return true;
    }
       
    return true;
}

//------------------------------------------------------------------------------

bool ActionMap::processTouch(const InputEvent* pEvent)
{
    static const char *argv[4];
    const Node* pNode = findNode(pEvent->deviceType, pEvent->deviceInst, pEvent->modifier, pEvent->objInst);
       
    if (pNode == NULL) 
    {
        // Check to see if we clear the modifiers, do we find an action?
        if (pEvent->modifier != 0)
            pNode = findNode(pEvent->deviceType, pEvent->deviceInst, 0, pEvent->objInst);
           
        if (pNode == NULL)
            return false;
    }
       
    // "Do nothing" bind:
    if ( !pNode->consoleFunction[0] )
        return( true );
       
    // Ok, we're all set up, call the function.
    argv[0] = pNode->consoleFunction;
    argv[1] = pEvent->fingerIDs;
    argv[2] = pEvent->fingersX;
    argv[3] = pEvent->fingersY;
       
    if (pNode->object)
        Con::executef(pNode->object, 4, argv[0], argv[1], argv[2], argv[3]);
    else
        Con::execute(4, argv);
       
    return true;
}

//------------------------------------------------------------------------------

bool ActionMap::processButton(const InputEvent* pEvent)
{
    static const char *argv[2];
    const Node* pNode = findNode(pEvent->deviceType, pEvent->deviceInst, pEvent->modifier, pEvent->objInst);

    if (pNode == NULL) 
    {
        // Check to see if we clear the modifiers, do we find an action?
        if (pEvent->modifier != 0)
            pNode = findNode(pEvent->deviceType, pEvent->deviceInst, 0, pEvent->objInst);

        if (pNode == NULL)
            return false;
    }

    // Whadda ya know, we have this bound.  Set up, and call the console
    //  function associated with it...
    //
    F32 value = pEvent->fValues[0];
       
    if (pNode->flags & Node::Ranged)
    {
        value = (value * 2.0f) - 1.0f;
         
        if (pNode->flags & Node::Inverted)
            value *= -1.0f;
    }
    else
    {
        if (pNode->flags & Node::Inverted)
            value = 1.0f - value;
    }

    if (pNode->flags & Node::HasScale)
        value *= pNode->scaleFactor;

    if (pNode->flags & Node::HasDeadZone)
    {
        if (value >= pNode->deadZoneBegin && value <= pNode->deadZoneEnd)
            value = 0.0f;
        else			
        {
            if( value > 0 )
                value = ( value - pNode->deadZoneBegin ) * ( 1.f / ( 1.f - pNode->deadZoneBegin ) );
            else
                value = ( value + pNode->deadZoneBegin ) * ( 1.f / ( 1.f - pNode->deadZoneBegin ) );
        }
    }

    if( pNode->flags & Node::NonLinear )
        value = ( value < 0.f ? -1.f : 1.f ) * mPow( mFabs( value ), CONST_E );

    // Ok, we're all set up, call the function.
    if(pNode->flags & Node::BindCmd)
    {
        // it's a bind command
        if(pNode->makeConsoleCommand)
            Con::evaluate(pNode->makeConsoleCommand);
    }
    else if ( pNode->consoleFunction[0] )
    {
        argv[0] = pNode->consoleFunction;
        argv[1] = Con::getFloatArg(value);
        
        if (pNode->object)
            Con::executef(pNode->object, 2, argv[0], argv[1]);
        else
            Con::execute(2, argv);
    }

    // [neo, 5/13/2007 - #3109]
    // The execs/evaluate above could have called reentrant script code which made calls to
    // bindCmd() etc, channging the node map underneath us. If enough nodes were added then
    // the node map vector would realloc, with the result that pNode would then be pointing 
    // at garbage and cause a crash when passed to enterBreakEvent() below. So we just look
    // it up again to be safe. This is not needed in the other cases below as we return right
    // after the execs and don't use pNode again.
    pNode = findNode( pEvent->deviceType, pEvent->deviceInst, pEvent->modifier, pEvent->objInst );

    if (pNode == NULL)
    {
        if (pEvent->modifier != 0)
        {
            // Check to see if we clear the modifiers, do we find an action?
            pNode = findNode(pEvent->deviceType, pEvent->deviceInst, 0, pEvent->objInst);

            if (pNode == NULL)
            {
                // We already called any bound methods/functions so our job is done
                return true;
            }
        }
    }

    // And enter the break into the table if this is a make event...
    enterBreakEvent(pEvent, pNode);

    return true;
}

//------------------------------------------------------------------------------

bool ActionMap::processMove(const InputEvent* pEvent)
{
    static const char *argv[4];

    if (pEvent->deviceType == MouseDeviceType)
    {
        const Node* pNode = findNode(pEvent->deviceType, pEvent->deviceInst, pEvent->modifier, pEvent->objType);

        if (pNode == NULL)
        {
            // Check to see if we clear the modifiers, do we find an action?
            if (pEvent->modifier != 0)
                pNode = findNode(pEvent->deviceType, pEvent->deviceInst, 0, pEvent->objType);

            if (pNode == NULL)
                return false;
        }

        // "Do nothing" bind:
        if ( !pNode->consoleFunction[0] )
            return( true );

        // Whadda ya know, we have this bound.  Set up, and call the console
        //  function associated with it.  Mouse events ignore range and dead
        //  zone params.
        //
        F32 value = pEvent->fValues[0];
         
        if (pNode->flags & Node::Inverted)
            value *= -1.0f;
        if (pNode->flags & Node::HasScale)
            value *= pNode->scaleFactor;

        // Ok, we're all set up, call the function.
        argv[0] = pNode->consoleFunction;
        argv[1] = Con::getFloatArg(value);
          
        if (pNode->object)
            Con::executef(pNode->object, 2, argv[0], argv[1]);
        else
            Con::execute(2, argv);

            return true;
    } 
    else if ( (pEvent->objType == XI_POS || pEvent->objType == XI_FLOAT || pEvent->objType == XI_ROT || pEvent->objType == XI_INT) )
    {
        const Node* pNode = findNode(pEvent->deviceType, pEvent->deviceInst, pEvent->modifier,   pEvent->objInst);

        if( pNode == NULL )
            return false;

        // Ok, we're all set up, call the function.
        argv[0] = pNode->consoleFunction;
        S32 argc = 1;

        if (pEvent->objType == XI_INT)
        {
            // Handle the integer as some sort of motion such as a
            // single component to an absolute position
            argv[1] = Con::getIntArg( pEvent->iValue );
            argc += 1;
        }
        else if (pEvent->objType == XI_FLOAT)
        {
            // Handle float as some sort of motion such as a
            // single component to an absolute position
            argv[1] = Con::getFloatArg( pEvent->fValues[0] );
            argc += 1;
        }
        else if (pEvent->objType == XI_POS)
        {
            // Handle Point3F type position
            argv[1] = Con::getFloatArg( pEvent->fValues[0] );
            argv[2] = Con::getFloatArg( pEvent->fValues[1] );
            argv[3] = Con::getFloatArg( pEvent->fValues[2] );

            argc += 3;
        }

        if (pNode->object)
        {
            Con::execute(pNode->object, argc, argv);
        }
        else
        {
            Con::execute(argc, argv);
        }

        return true;
    }
    else if ( pEvent->deviceType == JoystickDeviceType )
    {
        // Joystick events...
        const Node* pNode = findNode( pEvent->deviceType, pEvent->deviceInst, pEvent->modifier,   pEvent->objType );

        if( pNode == NULL )
            return false;

        // "Do nothing" bind:
        if ( !pNode->consoleFunction[0] )
            return( true );

        // Whadda ya know, we have this bound.  Set up, and call the console
        //  function associated with it.  Joystick move events are the same as mouse
        //  move events except that they don't ignore dead zone.
        //
        F32 value = pEvent->fValues[0];
        if ( pNode->flags & Node::Inverted )
            value *= -1.0f;

        if ( pNode->flags & Node::HasScale )
            value *= pNode->scaleFactor;

        if ( pNode->flags & Node::HasDeadZone )
        {
            if ( value >= pNode->deadZoneBegin && value <= pNode->deadZoneEnd )
            {
                value = 0.0f;
            }
            else
            {
                if( value > 0 )
                    value = ( value - pNode->deadZoneBegin ) * ( 1.f / ( 1.f - pNode->deadZoneBegin ) );
                else
                    value = ( value + pNode->deadZoneBegin ) * ( 1.f / ( 1.f - pNode->deadZoneBegin ) );
            }
        }

        if( pNode->flags & Node::NonLinear )
            value = ( value < 0.f ? -1.f : 1.f ) * mPow( mFabs( value ), CONST_E );

        // Ok, we're all set up, call the function.
        argv[0] = pNode->consoleFunction;
        argv[1] = Con::getFloatArg( value );
            
        if (pNode->object)
            Con::executef(pNode->object, 2, argv[0], argv[1]);
        else
            Con::execute(2, argv);

        return true;
    }
    else if ( pEvent->deviceType == GamepadDeviceType )
    {
        // Joystick events...
        const Node* pNode = findNode( pEvent->deviceType, pEvent->deviceInst, pEvent->modifier,   pEvent->objInst );

        if( pNode == NULL )
            return false;

        // "Do nothing" bind:
        if ( !pNode->consoleFunction[0] )
            return( true );

        // Whadda ya know, we have this bound.  Set up, and call the console
        //  function associated with it.  Joystick move events are the same as mouse
        //  move events except that they don't ignore dead zone.
        //
        F32 value = pEvent->fValues[0];
        if ( pNode->flags & Node::Inverted )
            value *= -1.0f;

        if ( pNode->flags & Node::HasScale )
            value *= pNode->scaleFactor;

        if ( pNode->flags & Node::HasDeadZone )
        {
            if ( value >= pNode->deadZoneBegin && value <= pNode->deadZoneEnd )
            {
                value = 0.0f;
            }
            else
            {
                if( value > 0 )
                    value = ( value - pNode->deadZoneBegin ) * ( 1.f / ( 1.f - pNode->deadZoneBegin ) );
                else
                    value = ( value + pNode->deadZoneBegin ) * ( 1.f / ( 1.f - pNode->deadZoneBegin ) );
            }
        }

        if( pNode->flags & Node::NonLinear )
            value = ( value < 0.f ? -1.f : 1.f ) * mPow( mFabs( value ), CONST_E );

        // Ok, we're all set up, call the function.
        argv[0] = pNode->consoleFunction;
        argv[1] = Con::getFloatArg( value );
            
        if (pNode->object)
            Con::executef(pNode->object, 2, argv[0], argv[1]);
        else
            Con::execute(2, argv);

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

bool ActionMap::processMotion(const InputEvent* pEvent)
{
    static const char *argv[2];

    // iOS Accelerometer, Gyroscope and DeviceMotion processing
    // Currently, this is identical to the joystick handling.
       
    // This was copied over into its own section because this will
    // give us a dedicated section to tweak processing based on iOS specific
    // devices. No point in trying to mangle joystick code any further
    const Node* pNode = findNode( pEvent->deviceType, pEvent->deviceInst, pEvent->modifier,   pEvent->objType );
       
    if ( pNode == NULL )
    {
        // Check to see if we clear the modifiers, do we find an action?
        if (pEvent->modifier != 0)
            pNode = findNode( pEvent->deviceType, pEvent->deviceInst, 0, pEvent->objType );
           
        if ( pNode == NULL )
            return false;
    }
       
    // "Do nothing" bind:
    if ( !pNode->consoleFunction[0] )
        return( true );
       
    F32 value = pEvent->fValues[0];
       
    if ( pNode->flags & Node::Inverted )
        value *= -1.0f;
       
    if ( pNode->flags & Node::HasScale )
        value *= pNode->scaleFactor;
       
    if ( pNode->flags & Node::HasDeadZone )
    {
        if ( value >= pNode->deadZoneBegin && value <= pNode->deadZoneEnd )
            value = 0.0f;
    }
       
    // Ok, we're all set up, call the function.
    argv[0] = pNode->consoleFunction;
    argv[1] = Con::getFloatArg( value );
       
    if (pNode->object)
        Con::executef(pNode->object, 2, argv[0], argv[1]);
    else
        Con::execute(2, argv);
       
    return true;
}

//------------------------------------------------------------------------------

bool ActionMap::processXInput(const InputEvent* pEvent)
{
    static const char *argv[2];

    if ((pEvent->objType == XI_FLOAT || pEvent->objType == XI_INT))
    {
        const Node* pNode = findNode( pEvent->deviceType, pEvent->deviceInst, pEvent->modifier, pEvent->objType );

        if (pNode == NULL )
            return false;

        // Ok, we're all set up, call the function.
        argv[0] = pNode->consoleFunction;
        S32 argc = 1;

        if (pEvent->objType == XI_INT)
        {
            // Handle the integer as some sort of motion such as a
            // single component to an absolute position
            argv[1] = Con::getIntArg( pEvent->iValue );
            argc += 1;
        }
        else if (pEvent->objType == XI_FLOAT)
        {
            // Handle float as some sort of motion such as a
            // single component to an absolute position
            argv[1] = Con::getFloatArg( pEvent->fValues[0] );
            argc += 1;
        }

        if (pNode->object)
        {
            Con::execute(pNode->object, argc, argv);
        }
        else
        {
            Con::execute(argc, argv);
        }
    }
    
    return true;
}

//------------------------------------------------------------------------------

bool ActionMap::processAction(const InputEvent* pEvent)
{
    switch(pEvent->action)
    {
    case SI_GESTURE:
        return processGesture(pEvent);
        break;
    case SI_TOUCH:
        return processTouch(pEvent);
        break;
    case SI_MAKE:
        return processButton(pEvent);
        break;
    case SI_MOVE:
        return processMove(pEvent);
        break;
    case SI_MOTION:
        return processMotion(pEvent);
        break;
    case SI_BREAK:
        return checkBreakTable(pEvent);
        break;
    case SI_VALUE:
        return processXInput(pEvent);
    }

    return false;
}

//------------------------------------------------------------------------------
void ActionMap::enterBreakEvent(const InputEvent* pEvent, const Node* pNode)
{
   // There aren't likely to be many breaks outstanding at any one given time,
   //  so a simple linear search is probably sufficient.  Note that the break table
   //  is static to the class, all breaks are directed to the action map that received
   //  the make.
   //
   S32 entry = -1;
   for (U32 i = 0; i < (U32)smBreakTable.size(); i++) {
      if (smBreakTable[i].deviceType == U32(pEvent->deviceType) &&
          smBreakTable[i].deviceInst == U32(pEvent->deviceInst) &&
          smBreakTable[i].objInst    == U32(pEvent->objInst)) {
         // Match.
         entry = i;
         break;
      }
   }
   if (entry == -1) {
      smBreakTable.increment();
      entry = smBreakTable.size() - 1;

      smBreakTable[entry].deviceType = pEvent->deviceType;
      smBreakTable[entry].deviceInst = pEvent->deviceInst;
      smBreakTable[entry].objInst    = pEvent->objInst;
   }

   // Ok, we now have the entry, and know that the device desc. and the objInst match.
   //  Copy out the node information...
   //
   smBreakTable[entry].object = pNode->object;
   // [neo, 5/7/2007 - #2975]
   // object above can be deleted in between a make/break and so object will point
   // to turfed memory and crash. To keep things simple we just store id as well so
   // we can look it up to validate object ref.
   smBreakTable[entry].objectId = pNode->object ? pNode->object->getId() : 0;

   smBreakTable[entry].consoleFunction = pNode->consoleFunction;

   if(pNode->breakConsoleCommand)
      smBreakTable[entry].breakConsoleCommand = dStrdup(pNode->breakConsoleCommand);
   else
      smBreakTable[entry].breakConsoleCommand = NULL;

   smBreakTable[entry].flags         = pNode->flags;
   smBreakTable[entry].deadZoneBegin = pNode->deadZoneBegin;
   smBreakTable[entry].deadZoneEnd   = pNode->deadZoneEnd;
   smBreakTable[entry].scaleFactor   = pNode->scaleFactor;
}

//------------------------------------------------------------------------------
bool ActionMap::checkBreakTable(const InputEvent* pEvent)
{
   for (U32 i = 0; i < (U32)smBreakTable.size(); i++) {
      if (smBreakTable[i].deviceType == U32(pEvent->deviceType) &&
          smBreakTable[i].deviceInst == U32(pEvent->deviceInst) &&
          smBreakTable[i].objInst    == U32(pEvent->objInst)) {
         // Match.  Issue the break event...
         //
         F32 value = pEvent->fValues[0];
         if (smBreakTable[i].flags & Node::Ranged) {
            value = (value * 2.0f) - 1.0f;
            if (smBreakTable[i].flags & Node::Inverted)
               value *= -1.0f;
         } else {
            if (smBreakTable[i].flags & Node::Inverted)
               value = 1.0f - value;
         }

         if (smBreakTable[i].flags & Node::HasScale)
            value *= smBreakTable[i].scaleFactor;

         if (smBreakTable[i].flags & Node::HasDeadZone)
            if (value >= smBreakTable[i].deadZoneBegin &&
                value <= smBreakTable[i].deadZoneEnd)
               value = 0.0f;

         // Ok, we're all set up, call the function.
         if(smBreakTable[i].consoleFunction)
         {
            if ( smBreakTable[i].consoleFunction[0] )
            {
               static const char *argv[2];
               argv[0] = smBreakTable[i].consoleFunction;
               argv[1] = Con::getFloatArg(value);
                              
               if( smBreakTable[i].object )
               {
                  // [neo, 5/7/2007 - #2975]
                  // object above can be deleted in between a make/break and so object will point
                  // to turfed memory and crash. To keep things simple we just store id as well so
                  // we can look it up to validate object ref.
                  if( smBreakTable[i].objectId > 0 && Sim::findObject( smBreakTable[i].objectId ) )
                     Con::executef(smBreakTable[i].object, 2, argv[0], argv[1]);
               }
               else
                  Con::execute(2, argv);
            }
         }
         else if(smBreakTable[i].breakConsoleCommand)
         {
            Con::evaluate(smBreakTable[i].breakConsoleCommand);
            dFree(smBreakTable[i].breakConsoleCommand);
         }
         smBreakTable.erase(i);
         return true;
      }
   }

   return false;
}

//------------------------------------------------------------------------------
bool ActionMap::handleEvent(const InputEvent* pEvent)
{
   // Interate through the ActionMapSet until we get a map that
   //  handles the event or we run out of maps...
   //
   SimSet* pActionMapSet = Sim::getActiveActionMapSet();
   AssertFatal(pActionMapSet && pActionMapSet->size() != 0,
               "error, no ActiveMapSet or no global action map...");

   for (SimSet::iterator itr = pActionMapSet->end() - 1;
        itr > pActionMapSet->begin(); itr--) {
      ActionMap* pMap = static_cast<ActionMap*>(*itr);
      if (pMap->processAction(pEvent) == true)
         return true;
   }

   return false;
}

//------------------------------------------------------------------------------
bool ActionMap::handleEventGlobal(const InputEvent* pEvent)
{
   // Interate through the ActionMapSet until we get a map that
   //  handles the event or we run out of maps...
   //
   SimSet* pActionMapSet = Sim::getActiveActionMapSet();
   AssertFatal(pActionMapSet && pActionMapSet->size() != 0,
               "error, no ActiveMapSet or no global action map...");

   return ((ActionMap*)pActionMapSet->first())->processAction(pEvent);
}

//------------------------------------------------------------------------------
//-------------------------------------- Key code to string mapping
//                                        TODO: Add most obvious aliases...
//
CodeMapping gVirtualMap[] =
{
   //-------------------------------------- KEYBOARD EVENTS
   //
   { "backspace",     SI_KEY,    KEY_BACKSPACE   },
   { "tab",           SI_KEY,    KEY_TAB         },

   { "return",        SI_KEY,    KEY_RETURN      },
   { "enter",         SI_KEY,    KEY_RETURN      },

   { "shift",         SI_KEY,    KEY_SHIFT       },
   { "ctrl",          SI_KEY,    KEY_CONTROL     },
   { "alt",           SI_KEY,    KEY_ALT         },
   { "pause",         SI_KEY,    KEY_PAUSE       },
   { "capslock",      SI_KEY,    KEY_CAPSLOCK    },

   { "escape",        SI_KEY,    KEY_ESCAPE      },

   { "space",         SI_KEY,    KEY_SPACE       },
   { "pagedown",      SI_KEY,    KEY_PAGE_DOWN   },
   { "pageup",        SI_KEY,    KEY_PAGE_UP     },
   { "end",           SI_KEY,    KEY_END         },
   { "home",          SI_KEY,    KEY_HOME        },
   { "left",          SI_KEY,    KEY_LEFT        },
   { "up",            SI_KEY,    KEY_UP          },
   { "right",         SI_KEY,    KEY_RIGHT       },
   { "down",          SI_KEY,    KEY_DOWN        },
   { "print",         SI_KEY,    KEY_PRINT       },
   { "insert",        SI_KEY,    KEY_INSERT      },
   { "delete",        SI_KEY,    KEY_DELETE      },
   { "help",          SI_KEY,    KEY_HELP        },

   { "win_lwindow",   SI_KEY,    KEY_WIN_LWINDOW },
   { "win_rwindow",   SI_KEY,    KEY_WIN_RWINDOW },
   { "win_apps",      SI_KEY,    KEY_WIN_APPS    },

   { "cmd",           SI_KEY,    KEY_ALT         },
   { "opt",           SI_KEY,    KEY_MAC_OPT     },
   { "lopt",          SI_KEY,    KEY_MAC_LOPT    },
   { "ropt",          SI_KEY,    KEY_MAC_ROPT    },

   { "numpad0",       SI_KEY,    KEY_NUMPAD0     },
   { "numpad1",       SI_KEY,    KEY_NUMPAD1     },
   { "numpad2",       SI_KEY,    KEY_NUMPAD2     },
   { "numpad3",       SI_KEY,    KEY_NUMPAD3     },
   { "numpad4",       SI_KEY,    KEY_NUMPAD4     },
   { "numpad5",       SI_KEY,    KEY_NUMPAD5     },
   { "numpad6",       SI_KEY,    KEY_NUMPAD6     },
   { "numpad7",       SI_KEY,    KEY_NUMPAD7     },
   { "numpad8",       SI_KEY,    KEY_NUMPAD8     },
   { "numpad9",       SI_KEY,    KEY_NUMPAD9     },
   { "numpadmult",    SI_KEY,    KEY_MULTIPLY    },
   { "numpadadd",     SI_KEY,    KEY_ADD         },
   { "numpadsep",     SI_KEY,    KEY_SEPARATOR   },
   { "numpadminus",   SI_KEY,    KEY_SUBTRACT    },
   { "numpaddecimal", SI_KEY,    KEY_DECIMAL     },
   { "numpaddivide",  SI_KEY,    KEY_DIVIDE      },
   { "numpadenter",   SI_KEY,    KEY_NUMPADENTER },

   { "f1",            SI_KEY,    KEY_F1          },
   { "f2",            SI_KEY,    KEY_F2          },
   { "f3",            SI_KEY,    KEY_F3          },
   { "f4",            SI_KEY,    KEY_F4          },
   { "f5",            SI_KEY,    KEY_F5          },
   { "f6",            SI_KEY,    KEY_F6          },
   { "f7",            SI_KEY,    KEY_F7          },
   { "f8",            SI_KEY,    KEY_F8          },
   { "f9",            SI_KEY,    KEY_F9          },
   { "f10",           SI_KEY,    KEY_F10         },
   { "f11",           SI_KEY,    KEY_F11         },
   { "f12",           SI_KEY,    KEY_F12         },
   { "f13",           SI_KEY,    KEY_F13         },
   { "f14",           SI_KEY,    KEY_F14         },
   { "f15",           SI_KEY,    KEY_F15         },
   { "f16",           SI_KEY,    KEY_F16         },
   { "f17",           SI_KEY,    KEY_F17         },
   { "f18",           SI_KEY,    KEY_F18         },
   { "f19",           SI_KEY,    KEY_F19         },
   { "f20",           SI_KEY,    KEY_F20         },
   { "f21",           SI_KEY,    KEY_F21         },
   { "f22",           SI_KEY,    KEY_F22         },
   { "f23",           SI_KEY,    KEY_F23         },
   { "f24",           SI_KEY,    KEY_F24         },

   { "numlock",       SI_KEY,    KEY_NUMLOCK     },
   { "scrolllock",    SI_KEY,    KEY_SCROLLLOCK  },

   { "lshift",        SI_KEY,    KEY_LSHIFT      },
   { "rshift",        SI_KEY,    KEY_RSHIFT      },
   { "lcontrol",      SI_KEY,    KEY_LCONTROL    },
   { "rcontrol",      SI_KEY,    KEY_RCONTROL    },
   { "lalt",          SI_KEY,    KEY_LALT        },
   { "ralt",          SI_KEY,    KEY_RALT        },
   { "tilde",         SI_KEY,    KEY_TILDE       },

   { "minus",         SI_KEY,    KEY_MINUS       },
   { "equals",        SI_KEY,    KEY_EQUALS      },
   { "lbracket",      SI_KEY,    KEY_LBRACKET    },
   { "rbracket",      SI_KEY,    KEY_RBRACKET    },
   { "backslash",     SI_KEY,    KEY_BACKSLASH   },
   { "semicolon",     SI_KEY,    KEY_SEMICOLON   },
   { "apostrophe",    SI_KEY,    KEY_APOSTROPHE  },
   { "comma",         SI_KEY,    KEY_COMMA       },
   { "period",        SI_KEY,    KEY_PERIOD      },
   { "slash",         SI_KEY,    KEY_SLASH       },
   { "lessthan",      SI_KEY,    KEY_OEM_102     },

   //-------------------------------------- BUTTON EVENTS
   // Joystick/Mouse buttons
   { "button0",       SI_BUTTON, KEY_BUTTON0    },
   { "button1",       SI_BUTTON, KEY_BUTTON1    },
   { "button2",       SI_BUTTON, KEY_BUTTON2    },
   { "button3",       SI_BUTTON, KEY_BUTTON3    },
   { "button4",       SI_BUTTON, KEY_BUTTON4    },
   { "button5",       SI_BUTTON, KEY_BUTTON5    },
   { "button6",       SI_BUTTON, KEY_BUTTON6    },
   { "button7",       SI_BUTTON, KEY_BUTTON7    },
   { "button8",       SI_BUTTON, KEY_BUTTON8    },
   { "button9",       SI_BUTTON, KEY_BUTTON9    },
   { "button10",      SI_BUTTON, KEY_BUTTON10   },
   { "button11",      SI_BUTTON, KEY_BUTTON11   },
   { "button12",      SI_BUTTON, KEY_BUTTON12   },
   { "button13",      SI_BUTTON, KEY_BUTTON13   },
   { "button14",      SI_BUTTON, KEY_BUTTON14   },
   { "button15",      SI_BUTTON, KEY_BUTTON15   },
   { "button16",      SI_BUTTON, KEY_BUTTON16   },
   { "button17",      SI_BUTTON, KEY_BUTTON17   },
   { "button18",      SI_BUTTON, KEY_BUTTON18   },
   { "button19",      SI_BUTTON, KEY_BUTTON19   },
   { "button20",      SI_BUTTON, KEY_BUTTON20   },
   { "button21",      SI_BUTTON, KEY_BUTTON21   },
   { "button22",      SI_BUTTON, KEY_BUTTON22   },
   { "button23",      SI_BUTTON, KEY_BUTTON23   },
   { "button24",      SI_BUTTON, KEY_BUTTON24   },
   { "button25",      SI_BUTTON, KEY_BUTTON25   },
   { "button26",      SI_BUTTON, KEY_BUTTON26   },
   { "button27",      SI_BUTTON, KEY_BUTTON27   },
   { "button28",      SI_BUTTON, KEY_BUTTON28   },
   { "button29",      SI_BUTTON, KEY_BUTTON29   },
   { "button30",      SI_BUTTON, KEY_BUTTON30   },
   { "button31",      SI_BUTTON, KEY_BUTTON31   },

   //-------------------------------------- MOVE EVENTS
   // Mouse/Joystick axes:
   { "xaxis",         SI_MOVE,   SI_XAXIS       },
   { "yaxis",         SI_MOVE,   SI_YAXIS       },
   { "zaxis",         SI_MOVE,   SI_ZAXIS       },
   { "rxaxis",        SI_MOVE,   SI_RXAXIS      },
   { "ryaxis",        SI_MOVE,   SI_RYAXIS      },
   { "rzaxis",        SI_MOVE,   SI_RZAXIS      },
   { "slider",        SI_MOVE,   SI_SLIDER      },

   //-------------------------------------- POV EVENTS
   // Joystick POV:
   { "xpov",          SI_POV,    SI_XPOV         },
   { "ypov",          SI_POV,    SI_YPOV         },
   { "upov",          SI_POV,    SI_UPOV         },
   { "dpov",          SI_POV,    SI_DPOV         },
   { "lpov",          SI_POV,    SI_LPOV         },
   { "rpov",          SI_POV,    SI_RPOV         },
   { "xpov2",         SI_POV,    SI_XPOV2        },
   { "ypov2",         SI_POV,    SI_YPOV2        },
   { "upov2",         SI_POV,    SI_UPOV2        },
   { "dpov2",         SI_POV,    SI_DPOV2        },
   { "lpov2",         SI_POV,    SI_LPOV2        },
   { "rpov2",         SI_POV,    SI_RPOV2        },

   #if defined( TORQUE_OS_WIN ) || defined( TORQUE_OS_XENON )
   //-------------------------------------- XINPUT EVENTS
   // Controller connect / disconnect:
   { "connect",       XI_BUTTON, XI_CONNECT     },
   
   // L & R Thumbsticks:
   { "thumblx",       XI_AXIS,   XI_THUMBLX     },
   { "thumbly",       XI_AXIS,   XI_THUMBLY     },
   { "thumbrx",       XI_AXIS,   XI_THUMBRX     },
   { "thumbry",       XI_AXIS,   XI_THUMBRY     },

   // L & R Triggers:
   { "triggerl",      XI_AXIS,   XI_LEFT_TRIGGER  },
   { "triggerr",      XI_AXIS,   XI_RIGHT_TRIGGER },

   // DPAD Buttons:
   { "dpadu",         XI_BUTTON, SI_UPOV     },
   { "dpadd",         XI_BUTTON, SI_DPOV   },
   { "dpadl",         XI_BUTTON, SI_LPOV   },
   { "dpadr",         XI_BUTTON, SI_RPOV  },

   // START & BACK Buttons:
   { "btn_start",     XI_BUTTON, XI_START       },
   { "btn_back",      XI_BUTTON, XI_BACK        },

   // L & R Thumbstick Buttons:
   { "btn_lt",        XI_BUTTON, XI_LEFT_THUMB  },
   { "btn_rt",        XI_BUTTON, XI_RIGHT_THUMB },

   // L & R Shoulder Buttons:
   { "btn_l",         XI_BUTTON, XI_LEFT_SHOULDER  },
   { "btn_r",         XI_BUTTON, XI_RIGHT_SHOULDER },

   // Primary buttons:
   { "btn_a",         XI_BUTTON, XI_A           },
   { "btn_b",         XI_BUTTON, XI_B           },
   { "btn_x",         XI_BUTTON, XI_X           },
   { "btn_y",         XI_BUTTON, XI_Y           },
#endif


   //-------------------------------------- MOTION EVENTS
   // Accelerometer/Gyroscope axes:
   { "accelx",        SI_MOTION,    SI_ACCELX    },
   { "accely",        SI_MOTION,    SI_ACCELY    },
   { "accelz",        SI_MOTION,    SI_ACCELZ    },
   { "gravityx",      SI_MOTION,    SI_GRAVX     },
   { "gravityy",      SI_MOTION,    SI_GRAVY     },
   { "gravityz",      SI_MOTION,    SI_GRAVZ     },
   { "gyrox",         SI_MOTION,    SI_GYROX     },
   { "gyroy",         SI_MOTION,    SI_GYROY     },
   { "gyroz",         SI_MOTION,    SI_GYROZ     },
   { "yaw",           SI_MOTION,    SI_YAW       },
   { "pitch",         SI_MOTION,    SI_PITCH     },
   { "roll",          SI_MOTION,    SI_ROLL      },

   //-------------------------------------- TOUCH EVENTS
   // Touch events:
   { "touchdown",     SI_TOUCH,    SI_TOUCHDOWN  },
   { "touchup",       SI_TOUCH,    SI_TOUCHUP    },
   { "touchmove",     SI_TOUCH,    SI_TOUCHMOVE  },

   //-------------------------------------- GESTURE EVENTS
   // Preset gesture events:
   { "circleGesture",      SI_GESTURE,  SI_CIRCLE_GESTURE    },
   { "swipeGesture",       SI_GESTURE,  SI_SWIPE_GESTURE     },
   { "screenTapGesture",   SI_GESTURE,  SI_SCREENTAP_GESTURE },
   { "keyTapGesture",      SI_GESTURE,  SI_KEYTAP_GESTURE    },
   { "pinchGesture",       SI_GESTURE,  SI_PINCH_GESTURE     },
   { "scaleGesture",       SI_GESTURE,  SI_SCALE_GESTURE     },
   
   //-------------------------------------- MISCELLANEOUS EVENTS
   //
   { "anykey",        SI_KEY,      KEY_ANYKEY },
   { "nomatch",       SI_UNKNOWN,  0xFFFFFFFF }
};

AsciiMapping gAsciiMap[] =
{
   //--- KEYBOARD EVENTS
   //
   { "space",           0x0020 },
   //{ "exclamation",     0x0021 },
   { "doublequote",     0x0022 },
   //{ "pound",           0x0023 },
   //{ "ampersand",       0x0026 },
   { "apostrophe",      0x0027 },
   //{ "lparen",          0x0028 },
   //{ "rparen",          0x0029 },
   { "comma",           0x002c },
   { "minus",           0x002d },
   { "period",          0x002e },
   //{ "slash",           0x002f },
   //{ "colon",           0x003a },
   //{ "semicolon",       0x003b },
   //{ "lessthan",        0x003c },
   //{ "equals",          0x003d },
   //{ "morethan",        0x003e },
   //{ "lbracket",        0x005b },
   { "backslash",       0x005c },
   //{ "rbracket",        0x005d },
   //{ "circumflex",      0x005e },
   //{ "underscore",      0x005f },
   { "grave",           0x0060 },
   //{ "tilde",           0x007e },
   //{ "vertbar",         0x007c },
   //{ "exclamdown",      0x00a1 },
   //{ "cent",            0x00a2 },
   //{ "sterling",        0x00a3 },
   //{ "currency",        0x00a4 },
   //{ "brokenbar",       0x00a6 },
   //{ "ring",            0x00b0 },
   //{ "plusminus",       0x00b1 },
   { "super2",          0x00b2 },
   { "super3",          0x00b3 },
   { "acute",           0x00b4 },
   //{ "mu",              0x00b5 },
   //{ "ordmasculine",    0x00ba },
   //{ "questiondown",    0x00bf },
   //{ "gemandbls",       0x00df },
   //{ "agrave",          0x00e0 },
   //{ "aacute",          0x00e1 },
   //{ "acircumflex",     0x00e2 },
   //{ "atilde",          0x00e3 },
   //{ "adieresis",       0x00e4 },
   //{ "aring",           0x00e5 },
   //{ "ae",              0x00e6 },
   //{ "ccedille",        0x00e7 },
   //{ "egrave",          0x00e8 },
   //{ "eacute",          0x00e9 },
   //{ "ecircumflex",     0x00ea },
   //{ "edieresis",       0x00eb },
   //{ "igrave",          0x00ec },
   //{ "iacute",          0x00ed },
   //{ "icircumflex",     0x00ee },
   //{ "idieresis",       0x00ef },
   //{ "ntilde",          0x00f1 },
   //{ "ograve",          0x00f2 },
   //{ "oacute",          0x00f3 },
   //{ "ocircumflex",     0x00f4 },
   //{ "otilde",          0x00f5 },
   //{ "odieresis",       0x00f6 },
   //{ "divide",          0x00f7 },
   //{ "oslash",          0x00f8 },
   //{ "ugrave",          0x00f9 },
   //{ "uacute",          0x00fa },
   //{ "ucircumflex",     0x00fb },
   //{ "udieresis",       0x00fc },
   //{ "ygrave",          0x00fd },
   //{ "thorn",           0x00fe },
   //{ "ydieresis",       0x00ff },
   { "nomatch",         0xFFFF }
};


////Device Event Types
//#define SI_UNKNOWN   0x01
//#define SI_BUTTON    0x02
//#define SI_POV       0x03
//#define SI_XPOV      0x04
//#define SI_YPOV      0x05
//#define SI_UPOV      0x06
//#define SI_DPOV      0x07
//#define SI_LPOV      0x08
//#define SI_RPOV      0x09
//#define SI_KEY       0x0A
//#define SI_XAXIS     0x0B
//#define SI_YAXIS     0x0C
//#define SI_ZAXIS     0x0D
//#define SI_RXAXIS    0x0E
//#define SI_RYAXIS    0x0F
//#define SI_RZAXIS    0x10
//#define SI_SLIDER    0x11
