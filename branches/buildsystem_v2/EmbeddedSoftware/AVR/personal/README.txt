This is the directory where the user should put it's own node directories.
If they are placed here the paths will be correct, otherwise the template
makefile must be edited.

Node directories should be based on ./template for the build system to work 
properly. The ./template directory is probably under subversion control but 
your node directory should not be, so a simple copy is not the correct way.
Instead use the subversion export feature to extract a clean copy of a 
directory stripped from svn files.

Example:
cd AVR/personal
svn export template node_0x04_IrReceiver2

To build and install or upgrade a bios on the node, you have to first setup
a system.inc based on system.inc.template.

After that, simply:
cd node_0x04_IrReceiver2
edit bios.inc to suit the node hardware, in particular set NODE_ID correctly.
make bios
make installbios (for ISP uploading to a fresh AVR) or
make upgradebios (for uploading over CAN to a node with a working bios)
make cleanbios (optional to reduce clutter in bios dir)

The recommended layout of the node directory is the following:

node_<NODE_ID>_<DESCRIPTION>/
|
|--bios/ (build tree for make bios)
|  |
|  |--bios.h (header file for the bios functions)
|  |
|  |--bios.hex (load file for installing/upgrading bios)
|  |
|  |--bios.map (memory layout of bios for use when linking application)
|  |
|  |--libbios.a (library for linking application with bios functions)
|  |
|  |--... (other build files if make cleanbios hasn't been run)
|
|--build/ (build tree for application)
|  |
|  |--...
|
|--src/
|  |
|  |--config.inc.template (template for settings used by the application)
|  |
|  |--main.c (application main source file)
|  |
|  |--Makefile (for building application, invoked by top level Makefile)
|  |
|  |--... (other source or header files used by the application)
|
|--bios.inc (settings file for bios)
|
|--config.inc (settings file for application)
|
|--Makefile (the make file used for all building, maintenance and flashing)

This layout is automatically set up if you are using an application in 
../../application. If this is the case, it's preferred to make the src/ 
directory a link to the application directory so that all nodes with that
function are using the same source. It also simplifies subversion commits 
which should happen in application/ not in personal/. For example:

ln -s ../../application/IrReceive src
make
make install
make start
make reset

If you're creating a node specific application which doesn't fit in
../../application, the src/ directory should still be based on 
../../application/template, which could be copied and stripped with svn export
as above.
