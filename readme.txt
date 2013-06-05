How to build HLAE
=================

For info on how to build HLAE see how_to_build.txt.


Directory Contents
==================

AfxCppCli/
  CLR Class Library in C++/CLI meant for handling interop
  and encapsulating unmanaged code.
  
AfxGui/
  C#, the HLAE GUI and more.
  
AfxHook/
  Boot image that is injected in the target process that shall
  be hooked, handles loading of the desired AfxHook*.dll and
  other things.
  
AfxHookGoldSrc/
  Hook for GoldSrc engine.
  
AfxHookSource/
  Hook for Source engine.
  
doc/
  Notes.

HlaeRemote/
  HLAE IPC-remoting example in C#.
  
build/
  Directory where C++ and C++/CLI projects place their intermediate
  files, binaries and PDBs.
  
misc/
  miscellaneous
  
resources/
  files that should be packaged with an release, use copy_resources_release.bat.
  
shared/
  files and source code used in more than one project go here
  
tools/
  small tools not part of the hlae core download
  
copy_resources_release.bat
  Helper for copying required files from the resource folder to
  build\Release\bin
  
how_to_build.txt
  Info on how to build HLAE.  
  
mdt.sln
  Solution file for Microsoft Visual Studio 2008 .NET (VC++) which organizes C++
  and C++/CLI  projects.
  
readme.txt
  This file.
