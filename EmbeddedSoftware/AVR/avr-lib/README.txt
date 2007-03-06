#================#
| AVR-lib readme |
#================#

Requirements
~~~~~~~~~~~~

Recent versions of avr-gcc, avr-libc and binutils-avr. A POSIX compliant shell
(/bin/sh), GNU make and GNU awk. Linux platforms are the safe bet, MacOS X and
MS Windows are untested and probably need tweaking.

Instructions
~~~~~~~~~~~~

These instructions will help you to set up AVR-bios on a new node and create 
an application template. The template demonstrates how to use AVR-bios 
functions to communicate on the CAN bus, and how to use AVR-lib code in your 
own projects. It is recommended to use the template as a starting point when 
creating a new project. The template is also neccessary even if you are going 
to use a finished project from somewhere else, since it contains node 
specific information on how to call AVR-bios functions. The application 
specific files in the template can be replaced with the corresponding files 
from the external project.

1. Checkout or update the avr-lib tree from svn if it's not already done.

2. Edit system.inc to fit your method of uploading via ISP. This only needs to
   be done once. The purpose of system.inc is to keep system specific 
   parameters out of Makefile so it can be properly versioned.

3. Edit config.inc to fit the node hardware you are setting up. Make sure you 
   pick an unused NODE_ID, from 1 to 254. At some point in the future, this 
   will probably be allocated automagically from a node database. But for now 
   set it manually. This needs to be done for each new node you are setting 
   up.

4. Build AVR-bios. The simple way is to run `make', which will create 
   everything you need. Other useful make targets are:
   
   `make lst'               Builds bios.elf, bios.map and bios.lst but skips 
                            libbios and the other template files. Good during 
                            bios development.
   `make clean'             Cleans everything that can be rebuilt. Do this 
                            before committing any changes in AVR-bios.
   `make cleanintermediate' Removes all files produced during build which are 
                            normally useless once the build is done. This is 
                            done in the default rule.
   `make template'          Prepares the template directory. Please note that 
                            the template files created are meaningless unless 
                            used together with a bios built at the same time.

5. Connect your ISP to the node and run `make install' to upload the bios to 
   the empty node. If the node already has a working AVR-bios it's possible to
   update it to a new version without ISP. Unfortunately the PC tools to 
   automate this hasn't been written yet. Expect a future `make reinstall'.

6. Copy the template directory to a directory of your choice. Place it in the 
   same parent directory as avr-lib if you don't want to fiddle with paths in 
   the Makefile. `cp -r template ../myapp' will do the trick, if your current 
   directory is avr-lib.

7. Change to the application directory you created. This is now a stand-alone 
   application for the exact node you just prepared. By stand-alone i mean it 
   has all information it needs to cooperate with AVR-bios on the node. It 
   probaly still depends on the AVR-lib source tree to exist, but that tree 
   can be updated without breaking anything in the app-bios interface. Please 
   note that this directory cannot be reused for another node, not even one 
   with the same configuration, since the NODE_ID must differ. Currently it's 
   one directory - one node. This may change in the future when node info is 
   stored in a database.
   
   The files in the application dir are
   
   bios.h     - generated file containing AVR-bios interface declarations and 
                CAN data structures
   bios.inc   - generated file containing node specific configuration
   config.inc - application specific configuration
   libbios.a  - library archive containing the AVR-bios interface, i.e. the 
                ROM-addresses of the bios functions in the node. Does not 
                contain any actual code, since that is already in the node's 
                flash.
   main.c     - main application code
   Makefile   - application makefile, currently needs to be edited to select 
                which AVR-lib drivers to compile

8. Edit config.inc to suit your application. This file is used together with 
   bios.inc to generate config.h as part of the build process. config.h is in 
   turn included by all AVR-lib drivers to set parameters.

9. Build the application with `make'. You'd better run a `make clean' first to
   delete old object files that may lie around from other builds. Object files
   from AVR-lib code should be built in the application dir instead of in the
   AVR-lib source tree, but currently they're not.

10. Upload the application over CAN. Currently the only existing PC tool for 
    this can not be scripted so it has to be run manually. Expect a 
    `make install' to work when the canManager tool is ready.
