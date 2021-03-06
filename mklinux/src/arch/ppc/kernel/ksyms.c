#include <linux/module.h>
#include <linux/smp.h>

extern put_user_byte();

static struct symbol_table arch_symbol_table = {
#include <linux/symtab_begin.h>
	/* platform dependent support */
  X(put_user_byte),
#include <linux/symtab_end.h>
};

void arch_syms_export(void)
{
	register_symtab(&arch_symbol_table);
}

