Building with CMake

Clone the repository.

 git clone https://csil-git1.cs.sfu.ca/373-23-3-lembas/social-gaming.git

Create a new directory for building.

 mkdir networkbuild

Change into the new directory.

 cd networkbuild

Run CMake with the path to the source.

 cmake ../web-socket-networking/

Run make inside the build directory:

 make

This produces chat server and client tools called bin/chatserver and bin/client-ncurses respectively. The library for single threaded clients and servers is built in lib/.

Running the Server 

 bin/chatserver 8000
