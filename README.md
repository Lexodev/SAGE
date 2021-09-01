<h1>SAGE project</h1>
<h3>Amiga game engine V1.0</h3>
<h4>1. How to compile</h4>
<p>To compile the lib you should have a C compiler (for sure), i'm using SAS/C V6.58 but any version above 6.5 should work, you also need an assign on the external libraries, you can find all the includes in the LibInclude repository, just clone it in the same directory where you clone this repo, then add an assign libinclude: to this repo. You should also have vasm in your path, all asm files are compiled with vasm.</p>
<p>The next step is to run the compilation, go into the src directory and type smake, everything should compile. The .lib will be put in the lib directory. You can now compile all other examples like tests, tutorial or demo.</p>
