# elfload - an ELF loader for embedded systems

libelfload implements a compact ELF loader for embedded systems. It is optimized
for minimal memory use, not performance; it caches very little data about the
binary being loaded, instead re-reading it from the file as necessary. This will
probably not satisfy you if you're trying to load, say, OpenOffice, but you
probably aren't doing that on an embedded system.

If elfload knows about your architecture, then it should "just work"; otherwise,
you will need to teach it. This mostly involves teaching it about the relocation
types applicable to your chosen platform, and adding a bit of architecture
information to elfarch.h.

## Usage
Everything centers around an el_ctx object; this contains all of the loader's
internal state. You should probably embed it inside some form of state structure
of your own, so that your callbacks have somewhere to gather their information
from.

You initialise the context by calling el_init. Before doing that, you must set
the "pread" member to a function which will read from the appropriate file
offset.

Once you have initialised the context, you may modify the base_load_[pv]addr
variables to configure where in memory the binary will be loaded.

elfload always accesses the binary using the specified physical addresses, but
performs relocations according to the specified virtual addresses. You can
exploit this difference if you are loading a binary before paging is enabled,
but then intending to run it paged (this feature is for Gandr's kernel loader).

Relocatable elf files (check for ctx->ehdr.e_type == ELF_DYN) are normally
linked to load at address 0; so you will probably want to adjust the load
address.

Your next step will probably be to actually load the binary. Do this using
el_load; this will read the program headers and load the binary as specified.
You pass an allocation callback; this will be called to allocate the memory.
The passed addresses are after the load offset has been applied. You should
return the address of the memory allocated (which must be the same as the
physical address, if you are to use el_relocate)

After the binary has been loaded, you can call el_relocate to perform
relocations on it.

If this is successful, you will probably want to jump into the binary. You can
grab the entrypoint from ctx->ehdr.e_entry; remember that you will need to
add the appropriate address offset.

## Other Features
You probably have some extra information you wish to extract from the
executable; you can do that using el_findphdr (if the information is contained
in a segment) or el_finddyn (if the information is contained in a dynamic table
entry). These functions are usable at any time after el_init has succeeded.

## Freeing memory
You are responsible for freeing any memory you allocate (including that
allocated through el_load's allocation callback). elfload does not allocate any
memory itself.

## Testing elfload
The supplied Makefile will build elfload for your host platform, and a test
application called "elfloader". This can be used to demonstrate and test
elfload. A sample source file, "sample.c", is supplied which is designed to
be loaded by this program.

Linux, *BSD and Illumos users can probably compile this using
    $ gcc -o sample.elf sample.c -nostdlib -pie -fPIC

Otherwise, you will probably need to use an ELF cross compiler, e.g.
    $ YourArch-elf-gcc -o sample.elf sample.c -nostdlib -pie -fPIC

(If you are testing this on AMD64 Windows, add -mabi=ms to that compile line)

You should then be able to run sample.elf, and get a "Helllo, World!" response,
using
    $ ./elfloader sample.elf

(if you are not using your system compiler, then you may experience issues at
this step due to ABI mismatches. YMMV.)

## Symbol resolution and dynamic linking
elfload does not presently support these features (even when it does, they are
likely to be optional features you can compile out)
