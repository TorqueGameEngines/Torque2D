# Documenting TorqueScript {#DocumentingTorqueScript}

## Introduction

This references describes how the TorqueScript manual is created.  It covers the tool used, the location of the documentation
text, and the process of adding or editing all the documentation types.  The end result is a single manual that can be referenced on-line or downloaded, and that contains information about classes, functions, operators, sub-systems,
general information, etc.

As a bit of background, TorqueScript is a scripting language written specifically for Torque.  The compiling and running
of the script code is a part of the engine itself.  Our goal here is to extract just the internal documentation that refers
to TorqueScript.

The manual is created using [Doxygen](doxygen.org), so its helpful to get some experience with that tool and
 also to refer to its manual for documenting features.  This reference only covers Doxygen lightly but probably
enough to get started.  It also covers our use of its high-level features to get what we want.  For now, we'll just
say that Doxygen is a feature-rich tool to extract documents out of a code base.  A C++ comment can be laced with 
special commands which Doxygen recognizes.  These sections are explicity about return types from functions, code examples, and
other features.  The end result is an organized "manual" of pages, with the code explained and with cross-referenced links
between the concepts.  The docs can be output in many formats including HTML, pdf, and chm.

## Documentation As an Ongoing Project

First, thanks in advance for any contribution to our collective knowledge to the TorqueScript documention!
There are also fertile bits of information over at the
[Torque Developer Network](http://tdn.garagegames.com/wiki/TDN_Home) (free login required) that could be yanked
over.  For instance, there are TDN sections for each [class](http://tdn.garagegames.com/wiki/TGB/Reference), and
each [global function](http://docs.garagegames.com/tge/official/content/documentation/Reference/Console%20Functions/TorqueScript_Console_Functions_21.html), at least.

While many of the basics are done, there is always more to cover.  Whether you know of tricks for creating better
game loops, gotchas to watch for, a bit of architecture that has gone undocumented, or anything else really, we could
use your help.  The goal of this project is to collect great information about Torque2D and get it into the source itself,
where it will live with the code.

## How to "Compile" the Torque2D Manual

If we want to see our docs as we go, the first thing we'll need is the ability to see the output from Doxygen as
we make edits.  Once we know how, this can be done in less than 5 seconds.  Here are the steps:

1. Go to the directory `Torque2D/tools/documentation`
     You should see, among files and directories
     + `config` - a directory containing a few files that are used to tell Doxygen how to proceed.
     + `output` - a directory where the results will go.  If it doesn't exist for you, simple make a new `output` directory.
	 (If you run `doxygen` without this directory, it will fail.)
     + `doxygen.exe` - currently Torque2D comes with a Windows copy of Doxygen.  But you can download a newer version or a
	 version for almost any platform, which you may need to do now.  It's free!
2. Start a command line tool in the `documentation` directory.
    I use Windows PowerShell (which I open by clicking the Window's Start menu and typing "powershell" into the search box).
3. Run Doxgyen from a command line feeding it the `config\torqueScriptReference.cfg` file.  I also like to pipe the output
to a file, as you see below.  Doxygen is very verbose!
	@code
	> .\doxygen.exe config\torqueScriptReference.cfg > output\info-ts.log
	@endcode
4. Afterwards, the directory `output/torqueScriptDocs` should have been produced, and inside of it, you should
have both `html` and `xml` output.  To see the results, click `html/index.html` to open it in a browser.
Click around and get accustomed to the manual if you haven't already.  As you make changes, "recompile" the manual
and simply refresh in your browser to see the results.

## About ScriptBinding and ScriptPage Files

We create two manuals with one Torque2D code base -- one is an engine reference and the other is this TorqueScript language reference.
This is unusual for Doxygen, and in order to have it create both, some cleverness was required.
However, we were able to achieve a pretty strong manual just by using a few techniques.

First, know that a file should be suffixed with `ScriptBinding.*` or `ScriptPage.*` to be included.  We tell Doxygen
to only read these files while creating the scripting manual (and vice-versa when creating the engine manual).
As you may expect then, these specially-named files contain the TorqueScript-related functions and not the engine code.
When Doxygen sees a file of code, it will add them to the doc, so it is important
to not have Doxygen run on any file that you don't want documented at the time.

As an example, most TorqueScript methods and functions are in files suffixed with `ScriptBinding.h`.
Comments in `SimObject_ScriptBinding.h` are gleened by Doxygen, but not comments in `SimObject.h` or `SimObject.cpp`.

> You may notice a few binding functions (ConsoleMethod(), ConsoleFunction(), etc.) not in the ScriptBinding files.  You may also
> notice that these don't produce docs.  These are older bindings, which work but have not been updated to the new format.
> (For what it's worth, migrating them to ScriptBinding files isn't just for documentation purposes.)

## Documenting Classes, Methods, and Global Functions

Documenting code "symbols" is by far the bulk of the good documentation. Each class, method, or function can have a 
description as complex as you want.  For methods and functions, 
you can (and hopefully will) explicitly state the parameters and return type.  You can put example code with each symbol if it helps.  
Finally, we should probably keep a "see also" section for related methods and functions.

Here is an edited version of comments for SimObject::getId() as an example of how to accomplish several of these practices.
Almost every type of symbol doc is included here:

@verbatim
/*! get the unique numeric ID -- or "handle" -- of this object.

	@param param1 a pretend param for this call since there wasn't one :)
	@return Returns the numeric ID.

	The id is provided for you by the simulator upon object creation.  You can not change it
	and it likely will not be reused by any other object after this object is deleted.

	@par Example
	@code
	new SimObject(example);
	echo(example.getId());
	> 1752
	@endcode

	@par Caveat
	You can not access the id directly.  That is, you can not access `%%object.id`.
	If you do set `%%object.id` you will only succeed in creating a dynamic field named
	`id` -- an unrelated field to the actual object's id.

	@par Example
	@code
	%example = SimObject();
	echo(%example.getId());
	> 1753

	... 

	I'm pretending I need one more paragraph here to show off lists.  I can use
	+ a list symbol such as "+" here on each line of a list
	+ a second element in my list

	@see getName, setName
*/
ConsoleMethodWithDocs(SimObject, getId, ConsoleInt, 2, 2, ())
{
   return object->getId();
}
@endverbatim

Knowing a bit about Doxygen does help, but you can get pretty far just trying the ideas above and experimenting.  The following
are several notes about the above example:

+ The entire doxygen comment section is surrounded by `/*!` and `*/` comment enclosings.  There are
other options, but this author prefers to set these at beginning and end and then not have to decorate every line.
+ This type of description is placed directly above the symbol it is documenting.
+ We have a `@param`, `@return`, and `@see` (for "see also" sections). Those are easy.
+ Any other text is just part of the detailed description.
+ We can create "sections" just like the `param` and `return` sections by using `@par <section>` like we did with "@par Caveat".  Just remember to start the paragraph immediately after that line.
+ We also wanted to create a couple of examples so we start each one with "@par Example" and follow that with a block of code between "@code" and "@endcode".
+ We can also use backticks around a phrase to make it represent in-line code.
+ In Doxygen, percent `%` is a command character.  TorqueScript is also full of `%` however.
In code sections between "@code" and "@endcode" you won'th have a problem, but in regular text
or even in in-line code sections, we have to use two percent signs to create one.
+ Note that I'm using `+` to start a list.  You can use `*` or `-` just like in markdown.  However, because
some people like to have a `*` at the beginning of every line of comments, Doxygen likes to eat those up, so
it's best to just avoid them.
+ Obvious function names and class names will be automatically cross-referenced.  They are "obvious"
when they have a camel case letter in them somewhere.  If your function does not, try putting "::" in front of it
or "()" after it.  To reference something in another class, you have to use className::methodName.  Doxygen explains
all this in its docs.  One last point though: let's suppose you want to avoid a link.  Use "%" in front of the camel
cased word that happens to be another class or function.

## Documenting Global Function Groups

All of the global functions in TorqueScript are documented individually as described above.
However, it is best to keep related functions clustered together.  Doxygen provides the idea of a "group"
(which is sometimes called a "module") to cluster things, so we use it to group global function categories.
See the [modules](modules.html) page for what we mean.

We also take advantage of Doxygen's ability to have groups within groups to keep better organized.
All of the categories groups are a subgroup of the `TorqueScriptFunctions` main group, as you might
have noticed if you clicked the modules link above.

To document a group of functions, let's suppose they are in the same file together.  If they are not, we can
modify the following formulas.  Go to that file, say the console output functions in `output_ScriptBinding.h`.
Near the top of the file, before any functions, tell Doxygen this will be a group as follows:

@verbatim
/*! @defgroup ConsoleOutputFunctions Console Output
	@ingroup TorqueScriptFunctions
	@{
*/
@endverbatim

What this does is start a new group `ConsoleOutputFunctions` and also gives it the more human-readable name "Console Output".
Then it puts this group within the `TorqueScriptFunctions` main group.  And finally it starts the group with "@{".

Now go to the bottom of the file, somewhere after all the functions and put the following:

@verbatim
/*! @} */ // group ConsoleOutputFunctions
@endverbatim

This line ends the group with "@}".  The comment after that is just to be friendly.

This should effectively put all the documented functions between "@{" and "@}" into a group together.

## Documenting Sub-Systems

A Sub-system is a collection of components, such as classes and functions, that work together to form a bigger whole.
An example is the %Taml module or the Module Manager.  This is another great use of the "group" concept (see above for
another example).  This time, however, we want to write a description of the sub-system itself.  Doxygen supports this
by letting you document the group, as if it were a code symbol itself.  These docs then become the "detailed description"
portion of the group page.

The question is where should all this text be placed?  It isn't related directly to one class.
The solution is to make a special file for the sub-system documentaion and place it near the other code to which it
relates.  Typically, the files (classes) that make up a sub-system are placed in a directory together, so this is often
a great place for the sub-system "group" file.

The file should end with `.dox` which is a special Doxygen file extension.  Doxygen will read these files as if they were
code, but the C++ compiler will not.  (And remember, our files should also end in `ScriptPage.*` to be seen while building
the scripting manual.  Therefore, your file should be something like `mySubSystem_ScriptPage.dox`.

While there are no examples of TorqueScript sub-systems documented yet, we can see an example for the Torque2D engine
manual at `Torque2D\engine\source\persistence\taml\taml.dox`.  (Note the lack of a ScriptPage suffix because this is not
scripting docs!)  In the case of the taml file, it is mostly a markdown formatted file.  Section headers
begin with #, and URLs and other markdown data are also marked appropriately.

However, the entire file markdown file is placed in a Doxygen comment block with `/*!` at the top and `*/` at the bottom.
The entire file effectively becomes a giant comment.  We then add to the comment Doxygen commands to turn the entire
file into a "group" description like so at the top:

@code
/*! @defgroup tamlGroup TAML Reference
  ... the entire contents in markdown
*/
@endcode

For a little cross-referencing sweetness, classes related to this sub-system were marked with the following
comment.  This causes them to be included in the sub-system "group" and have a link each to the group description.

@code
/// @ingroup tamlGroup
/// @see tamlGroup
@endcode

Again, this was done for an engine subsytem, but the same *could* be done for TorqueScript systems or modules.

## Documenting General Information

Sometimes your docs are not really related to any one class, or even one sub-system.  This page on creating documents is
one example.  The TorqueScript Syntax page is another.

Doxygen provides one last concept we will talk about, called a "page."  The way we keep pages is the most markdown-like
and the least like Doxygen comments yet.  To create these, simply create a markdown file (extension .md).  The top header
of the file (the section header that starts with a single hash #), will be the name of the page in the final document.

As usual, this markdown file should be suffixed with `_ScriptPage.*`, for instance "syntax_ScriptPage.md"

We try to keep the pages close to the code it relates to, but rememer "pages" tend to be unrelated to particular classes,
modules, etc.  Therefore the pages have been kept in the `Torque2D\engine\source\console` directory since it most
represents the entire TorqueScript interpreter in general.  For example this document is `documenting_ScriptPage.md`
within the `console` directory.

The content of the file requires no doxygen commands such as @@defgroup or @@param (although @@code has come in useful as will be
described in the appendix).  It is read as pure markdown (with a few
exceptions).  Doxygen will successfully create links for any symbols is recongizes however.  (To force the link off, put a
percent % in front of the text.

## Documenting Operations, Types, etc.

*This topic is more advanced.  You can possibly skip it.*

Another type of module documentation is for non-functions.  This includes TorqueScript types (String, Boolean, etc.) and
operators (SPC, new, etc.).

To do this, we are playing a bit of a trick.  Remember that Doxygen was not created to document your language that you created
(such as TorqueScript).  What we will do is make a psuedo-code file full of pretend C++ constructs, called
`Torque2D\engine\source\console\core_ScriptBinding.dox`  The C++ compiler won't be interested, but when Doxygen sees
the file it will parse it as real code.

The solution we have employed is to have one special file located in the same directory as the TorqueScript interpreter code.
C++ compilers will not bother to compile a .dox file but doxygen will read it.  For all practical purposes, though,
this is a C++ file in form.

We will not go into the details here, but read that file and add to it to create lists of operators or
types in groups.

## Converting "Old" Documents to "New"

*If you read the appendix on how documenting TorqueScript is done with Doxygen, this may make more sense to you.*

You may find, from time to time, snippets of an older form of documentation for TorqueScript in the code base.  One of the
Appendices explains in more detail, but basically, documents used to be a "usage" string as part of a macro call like this:

@code
ConsoleFunction(myFunction, void, 4, 4, "(id) very long string of documents..."
						"@param including parameters and other doxygen commands!")
@endcode

These macro calls appeared with the source code in the same file, for instance in `SimObject.cc`.  For readability reasons,
these functions were mostly pulled to their own files, such as `SimObject_ScriptBinding.h`, but not all of them have made it.
(We could use help in moving the stragglers as well!)

The new documentation process takes advantage of these binding files being separate.  Additionally it needs a new macro that
does not take the usage string, called ConsoleFunctionWithDocs, etc.  For instance this is the new form:

@code
/*! very long string of documents
    @param which is a good thing now
*/
ConsoleFunctionWithDocs(myFunction, void, 3, 3, (id))
@endcode

Ultimately, when all the functions and methods are complete, the macro name may change back to not include the "WithDocs" ending.
For now, this is what you are seeing!

## Appendix A: Doxygen Command and Markup Caveats

It will probably be necessary to limit yourself some when trying to grab a markdown page and move it to the source base for
Doxygen's use.  While Doxygen claims to handle markdown, including GitHub flavored markdown, in practice we have run into a
few edge cases.  Below are a few of them.

+ lists starting with `*` may not work.  This is because it seems like the line is part of a comment surrounded with *.
If you use `+` or `-` it will work.

+ doxygen understands "fencepost" code blocks (~~~) or (~~~{.cpp}) but we have seen some issues.  Specifically, doxygen
will eat double-slash `//` comments within them!  This is probably a bug.  Luckily Doxygen understands its own commands
embedded in markdown, so you can use @@code and @@endcode around code sections.  This will preserver comments inside of those sections.

+ starting a word with % such as %%myVar is very common in TorqueScript, but Doxygen reserves % to mean "do not create a link here".
For instance, if you don't want SimObject to become a link, use `%%SimObject`.  For TorqueScript variables use two percents before your
variables to see one.  This is not necessary in code sections!  But it is necessary in in-line code and regular text.  :)

+ GitHub flavored markdown provides for easier numbered lists.  Simply start each item with `1.` and they will be re-numbered.
However this does not work in Doxygen.  Number each item correctly.

+ **Blockquotes** markdown lets you put a `>` at the beginning of a paragraph. All the following lines will be part of one
section of blockquotes.  However, in Doxygen, you appears to need a `>` after every new line.

## Appendix B.  How Documenting TorqueScript Works

For several years, Torque2D produced a TorqueScript manual in a different way than described in this reference.

As a bit of back story, don't forget that we want to be able to produce a manual for Torque2D's C++ engine as well
as produce a manual for TorqueScript.  So running Doxygen on the code produced only the Torque2D manual.  This is "as it should be."

Back then, to create a TorqueScript manual one would compile Torque2D, and run it.  From within the program one would call
dumpConsoleFunctions() and other functions to produce a "psuedo-code" output file.  The file was suspiciously like a program itself,
complete with documentation of the "functions" and "classes".  This file would then be run through Doxygen and viola!, a
second reference manual was produced.

The problem with using the above method had two specific drawbacks worth removing if possible.  First, it required several steps -- 
compiling the code, running it against some script calls, running Doxygen on the output of the dump -- and so it was impractical to 
add and modify new docs as easily as Doxygen is designed to do.

Second, the script documents had to be hidden from Doxygen on the normal 
pass lest it include confusing bits of information in the engine docs!  To do so, the docs appeared in a (large) string for each TorqueScript
call or class.  This was harder to work with than simple comments.  A less important issue, but still a real one, was the possibility that
all these strings, compiled into the engine, could be expensive in space.  (Though the strings could be skipped at compile time if
TORQUE_SHIPPING was defined.)

The new solution employed a different clever solution.  Now, the TorqueScript function bindings are documented just like any other code
-- in comment sections with the code.  And "compiling" the documents is no harder than running Doxygen on the code base.
The way we get Doxygen to pick between documenting the engine or  
TorqueScript is that TorqueScript functions are in special files ending with `_ScriptBinding.*` or `_ScriptPage.*`.  These files are skipped when 
creating docs for the engine.  Likewise, they are the only files used when creating the script reference.


> Normally, Doxygen is used to create a cross-referenced document for a C++ code base.  Note that our desire here is
> to document a class and function binding library, along with the syntax, operators, types, etc. of TorqueScript.
> But Doxygen is so useful and capable that it was worth working out a way to use it for our purposes.  As a matter of fact,
> we can also produce a Torque2D engine document from the same code base!

## Appendix C.  Some Unusual Doxygen Facts

Doxygen tries to be a lot of things in a lot of formats, and this has sometimes caused odd edge-case bugs.  This is a
quick list of a few quirks I wanted to save somewhere:

Of course, normally Doxygen data is saved in comments and the `@` commands are how you tell it about constructs from
parameter lists to what group to put things in.   But you can use markdown for many formatting commands like bold,
lists, blockquotes, etc.  (I don't think Doxygen has any other kind of formatting, except that you can use direct
html apparently).

Now a `.dox` file is read by Doxygen just like a source code file.   The commands to Doxygen must be in comments, and
you can even have source code in it (and we do) but the compiler won't see it.  If you wanted to use an entire `.dox` file
as one big description (and we do), you still need to encase the entire thing in a comment block.

Finally `.md` files are an exception.  Doxygen reads and uses the text in these without the comment blocks.  This makes a natural "page" construct.  I'm not sure any other construct can be created this way.  However, if
you sneak in an `@` command or two, those will work!  (And doxygen isn't great at all markdown syntax, sadly.)  In this way,
you *could* put class info or group info inside of a `.md` perhaps.

