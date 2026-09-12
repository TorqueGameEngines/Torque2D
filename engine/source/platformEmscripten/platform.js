//-----------------------------------------------------------------------------
// Copyright (c) 2014 James S Urquhart
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

mergeInto(LibraryManager.library, {

// NOTE: the message/title args arrive as C (UTF8) POINTERS into the wasm heap, not
// JS strings, so they must be decoded with UTF8ToString() — the old code passed the
// raw pointer straight to alert(), which showed the integer ADDRESS (e.g. "363333").
//
// AlertOK is informational; route it to console.error instead of a blocking native
// alert() — a web game must not freeze the browser tab, and an engine that calls
// AlertOK every frame (e.g. a recurring assert) would otherwise wedge the page with
// an un-dismissable dialog storm. The decision dialogs keep a real (blocking) prompt
// because the engine needs their boolean answer.
js_AlertOK: function(title, message) {
	console.error('[Torque] ' + UTF8ToString(message));
	return 1;
},

js_AlertOKCancel: function(title, message) {
	return confirm(UTF8ToString(message)) ? 1 : 0;
},

js_AlertRetry: function(title, message) {
	return confirm(UTF8ToString(message)) ? 1 : 0;
},

js_AlertYesNo: function(title, message) {
	return confirm(UTF8ToString(message)) ? 1 : 0;
},

js_AlertOk: function(title, message) {
	console.error('[Torque] ' + UTF8ToString(message));
	return 1;
},

js_ConsoleEnabled: function(prompt) {
	// TODO
},

js_ConsoleDisabled: function() {
	// TODO
},

js_ConsoleLine: function(message) {
	if (message) console.log(UTF8ToString(message));
},

step_warn: function() {
	console.log('step warn');
}

// (The desktop's size is SDL 2's now -- the screen the page is on, from
// Video::getDesktopResolution in EmscriptenWindow.cpp. These functions used to
// make up a 1024x768 one.)

});
