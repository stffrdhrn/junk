/*
 * Copyright (C) 2020, Stafford Horne <shorne@gmail.com>
 *
 * License: GNU GPL, version 2 or later.
 *   See the COPYING file in the top-level directory.
 */
#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <glib.h>

#include <qemu-plugin.h>

QEMU_PLUGIN_EXPORT int qemu_plugin_version = QEMU_PLUGIN_VERSION;

struct watchpoint {
  uint64_t addr;      /* The address to watch for, 0 for any */
  uint64_t addr_mask; /* A mask for the address pattern, i.e 0x00ff */
  bool     load;
  bool     store;
};

/* Just one watchpoint supported */
static struct watchpoint wp;
static FILE*  tracefile;

static enum qemu_plugin_mem_rw rw = QEMU_PLUGIN_MEM_RW;

static void plugin_exit(qemu_plugin_id_t id, void *p)
{
    if (tracefile != stdout) {
        fclose(tracefile);
    }
}

static void vcpu_mem(unsigned int cpu_index, qemu_plugin_meminfo_t meminfo,
                     uint64_t vaddr, void *udata)
{
    bool is_store = qemu_plugin_mem_is_store(meminfo);
    bool do_trace = false;
    uint64_t insn_addr = (uint64_t) udata;

    if ((wp.addr_mask & vaddr) == wp.addr) {
      if (is_store && wp.store) {
        do_trace = true;
      }
      if (!is_store && wp.load) {
        do_trace = true;
      }
    }

    if (do_trace) {
        fprintf(tracefile, "%llx: %s %llx\n", insn_addr, is_store ? "ST": "LD", vaddr);
    }
}

static void vcpu_tb_trans(qemu_plugin_id_t id, struct qemu_plugin_tb *tb)
{
    size_t n = qemu_plugin_tb_n_insns(tb);
    size_t i;

    for (i = 0; i < n; i++) {
        struct qemu_plugin_insn *insn = qemu_plugin_tb_get_insn(tb, i);

        qemu_plugin_register_vcpu_mem_cb(insn, vcpu_mem,
                                         QEMU_PLUGIN_CB_NO_REGS,
                                         rw, (void *) qemu_plugin_insn_vaddr(insn));
    }
}

/*
 * Parse Args:
 * arg=addr,arg=0x00000000,arg=addr_mask,arg=0x0000000,
 * arg=load,arg=store
 * arg=tracepath,arg=/some/path/tracefile.txt
 */
QEMU_PLUGIN_EXPORT int qemu_plugin_install(qemu_plugin_id_t id,
                                           const qemu_info_t *info,
                                           int argc, char **argv)
{

    char * tracepath = "trace.txt";
    int c;

    for (c = 0; c < argc; c++) {
        if (!strcmp(argv[c], "addr")) {
            c++;
            wp.addr = strtol(argv[c], NULL, 16);
        } else if (!strcmp(argv[c], "addr_mask")) {
            c++;
            wp.addr_mask = strtol(argv[c], NULL, 16);
        } else if (!strcmp(argv[c], "tracepath")) {
            c++;
            tracepath = argv[c];
        } else if (!strcmp(argv[c], "load")) {
            wp.load = true;
        } else if (!strcmp(argv[c], "store")) {
            wp.store = true;
        } else {
            fprintf(stderr, "Unknown argument: %d %s\n", c, argv[c]);
        }
    }

    /* Mask off any addr bits we need to mask */
    wp.addr = wp.addr_mask & wp.addr;

    if (tracepath[0] == '-') {
        tracefile = stdout;
    } else {
        tracefile = fopen(tracepath, "w");
    }

    qemu_plugin_register_vcpu_tb_trans_cb(id, vcpu_tb_trans);
    qemu_plugin_register_atexit_cb(id, plugin_exit, NULL);
    return 0;
}
