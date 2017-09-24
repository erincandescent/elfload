/* Copyright © 2014, Owen Shepherd
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted without restriction.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "elfload.h"

FILE *f;
void *buf;

typedef void (*entrypoint_t)(int (*putsp)(const char*));

static bool fpread(el_ctx *ctx, void *dest, size_t nb, size_t offset)
{
    (void) ctx;

    if (fseek(f, offset, SEEK_SET))
        return false;

    if (fread(dest, nb, 1, f) != 1)
        return false;

    return true;
}

static void *alloccb(
    el_ctx *ctx,
    Elf_Addr phys,
    Elf_Addr virt,
    Elf_Addr size)
{
    (void) ctx;
    (void) phys;
    (void) size;
    return (void*) virt;
}

static void check(el_status stat, const char* expln)
{
    if (stat) {
        fprintf(stderr, "%s: error %d\n", expln, stat);
        exit(1);
    }
}

static void go(entrypoint_t ep)
{
    ep(puts);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s [elf-to-load]\n", argv[0]);
        return 1;
    }

    f = fopen(argv[1], "rb");
    if (!f) {
        perror("opening file");
        return 1;
    }

    el_ctx ctx;
    ctx.pread = fpread;

    check(el_init(&ctx), "initialising");

    if (posix_memalign(&buf, ctx.align, ctx.memsz)) {
        perror("memalign");
        return 1;
    }

    if (mprotect(buf, ctx.memsz, PROT_READ | PROT_WRITE | PROT_EXEC)) {
        perror("mprotect");
        return 1;
    }

    ctx.base_load_vaddr = ctx.base_load_paddr = (uintptr_t) buf;

    check(el_load(&ctx, alloccb), "loading");
    check(el_relocate(&ctx), "relocating");

    uintptr_t epaddr = ctx.ehdr.e_entry + (uintptr_t) buf;

    entrypoint_t ep = (entrypoint_t) epaddr;

    printf("Binary entrypoint is %x; invoking %p\n", ctx.ehdr.e_entry, ep);

    go(ep);

    fclose(f);

    free(buf);

    return 0;
}
