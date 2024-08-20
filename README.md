<h1>SAGE project</h1>
<h3>Simple Amiga Game Engine V24.8 August 2024</h3>
<h4>1. In this repository</h4>
<p>You will find the libraries and the documentation in the lib folder in three versions, "sage_debug.lib" has debug log and safe activated, use it for your dev.
"sage_safe.lib" as safe mode activated (check for null pointer) and "sage.lib" is the full speed version with no debug and no check.</p>
<p>"include" folder contains all SAGE includes, copy them in your include folder.</p>
<p>"demo" folder contains some quick demos off SAGE features</p>
<p>"tutorial" folder contains 10 tutorials with source code to briefly explore the main features of SAGE.</p>
<h4>2. How to compile tutorials and demos</h4>
<p>To compile the tutorials or the demos you should have a C compiler (for sure), i'm using SAS/C V6.59 but any version above 6.5 should work, you also need an assign to the external libraries folder, you can find all the includes in the LibInclude repository, just clone it in the same directory where you clone this repo, then add an assign libinclude: to the folder. You should also have vasm in your path, all asm files are compiled with vasm.</p>
<p>The next step is to run the compilation, go into the tutorial directory and type smake, everything should compile.</p>
<p>In general to compile a program for SAGE you can do something like "sc LINK myfile.c CPU=68060 IDIR=libinclude: MATH=68882 NOICONS DATA=far pathtolib/sage.lib"</p>
