Generating and publishing the Torque2D reference documentation
==============================================================

There are two separate doc sets, both generated from engine/source by Doxygen:

  Torque2DDocs     - the C++ engine reference. Built from the engine source with
                     the *_ScriptBinding.* files excluded.
  TorqueScriptDocs - the TorqueScript reference. Built from ONLY the
                     *_ScriptBinding.* files, plus the two hand-written pages
                     engine/source/console/documenting_ScriptPage.md and
                     syntax_ScriptPage.md.

Both are published to the gh-pages branch and served at
https://torquegameengines.github.io/Torque2D/

You need Doxygen on your PATH. https://www.doxygen.nl/download.html


1. Generate
-----------

RUN THESE FROM tools/doxygen/config, NOT FROM tools/doxygen.

Doxygen resolves the relative paths inside a config file against your current
working directory, not against the config file's own location. Every path in
these configs -- INPUT = ../../../engine/source, OUTPUT_DIRECTORY = ../output/...,
WARN_LOGFILE, HTML_EXTRA_STYLESHEET -- assumes you are standing in config/. Run
them from anywhere else and Doxygen looks for the engine source outside the
repository and writes its output somewhere you did not intend.

    cd tools/doxygen/config
    doxygen torque2DReference.cfg
    doxygen torqueScriptReference.cfg

That writes:

    tools/doxygen/output/torque2DDocs/html
    tools/doxygen/output/torqueScriptDocs/html
    tools/doxygen/output/warn-t2d.log
    tools/doxygen/output/warn-ts.log

output/ is gitignored, so the build products never land on development or
master. Regenerate BOTH sets, not one -- publishing a half-updated pair is how
the script reference silently fell an entire release behind the engine.

Check the two warning logs. warn-ts.log in particular catches real defects in
the /*! */ comments in the _ScriptBinding.h files: @param names that do not
match the macro's argument string, documented symbols that were never declared,
and unknown Doxygen commands.


2. Publish
----------

gh-pages is an orphan branch with no source tree in it. Its .gitignore ignores
/engine/*, /tools/*, and the built executable, which is what lets you check it
out in this same working directory: your source tree and the doc output sit
there as ignored files while you commit.

    git checkout gh-pages
    (copy tools/doxygen/output/torque2DDocs/html     over Torque2DDocs/html)
    (copy tools/doxygen/output/torqueScriptDocs/html over TorqueScriptDocs/html)
    git add Torque2DDocs TorqueScriptDocs
    git commit -m "Updated documentation to <version>"
    git push origin gh-pages
    git checkout development

GitHub Pages serves the result within a minute or two.


How the TorqueScript reference works
------------------------------------

Worth understanding before anyone edits the binding macros.

There is no preprocessor step. Doxygen does not natively understand

    ConsoleMethodWithDocs(SceneObject, setPosition, ConsoleVoid, 3, 4, (x, y))

so torqueScriptReference.cfg turns it into something Doxygen does understand,
using ENABLE_PREPROCESSING + MACRO_EXPANSION + EXPAND_ONLY_PREDEF and a
PREDEFINED block that rewrites each macro into synthetic C++:

    ConsoleMethodGroupBeginWithDocs(class, super) -> class class : public super { public:
    ConsoleMethodWithDocs(class, name, ret, ...)  -> ret class::name argString
    ConsoleMethodGroupEndWithDocs(class)          -> };

A _ScriptBinding.h file therefore reads to Doxygen as a class declaration whose
members are the script methods, and the /*! */ comment above each macro becomes
that member's documentation.

Two consequences:

  - THE CLASS HIERARCHY IN THE PUBLISHED DOCS COMES FROM THE SECOND ARGUMENT OF
    ConsoleMethodGroupBeginWithDocs, not from C++ inheritance. Get that argument
    wrong and the docs show the wrong parent, however correct the C++ is.
  - The ConsoleString / ConsoleInt / ConsoleVoid spellings are load-bearing.
    PREDEFINED maps them to readable return types (String, Integer, void).

torque2DReference.cfg has the mirror-image block: it expands every binding macro
to nothing, so leftover bindings cannot confuse the C++ pass.


Historical, not part of the process
-----------------------------------

  comment.rb                          A one-time 2013 migration that converted
                                      the old ConsoleMethod(..., "(args) desc")
                                      form to the /*! */ + WithDocs form. There
                                      is nothing left for it to convert.

  generateDocs.bat                    DEAD. References a config that does not
                                      exist, and publishes via svn checkout
                                      against GarageGames infrastructure through
                                      a GitHub SVN bridge retired in 2024.

  generateDocs.command                Referenced by the old ReadMe; never
                                      committed, or long since deleted.

  torqueScriptReference-from-dump.cfg  A Doxyfile 1.6.1 for an alternative
  main.generateDocs.cs                pipeline that dumped the console API from
                                      a running engine and fed that to Doxygen.
                                      Broken at both ends: it writes to a
                                      documentation/ directory that no longer
                                      exists, and expects macros the engine no
                                      longer defines. Revivable, but redundant
                                      with the config-based path above.

  torqueScriptReference-header.html   Orphaned. HTML_HEADER, HTML_FOOTER and
  torqueScriptReference-footer.html   LAYOUT_FILE are all empty in both current
  torqueScriptReference-layout.xml    configs. Last touched 2014, and the header
                                      hardcodes a GarageGames URL.
