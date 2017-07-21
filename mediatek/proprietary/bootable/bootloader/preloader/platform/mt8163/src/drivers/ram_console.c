#include <string.h>
#include "platform.h"
#include "ram_console.h"

#define RAM_CONSOLE_SIG (0x43474244) /* DBGC */
#define MOD "[RAM_CONSOLE]"

struct ram_console_buffer {
	u32 sig;
	/* for size comptible */
	u32 off_pl;
	u32 off_lpl; /* last preloader */
	u32 sz_pl;
	u32 off_lk;
	u32 off_llk; /* last lk */
	u32 sz_lk;
	u32 padding[9]; /* size = 4 * 16 = 64 byte */
};

struct reboot_reason_pl {
	u32 wdt_status;
	u32 last_func[RAM_CONSOLE_PL_SIZE]
};

static struct ram_console_buffer *ram_console = NULL;
static int ram_console_size;
extern unsigned int g_rgu_status;

#define ALIGN(x, size) ((x + size - 1) & ~(size - 1))

void ram_console_init(void)
{
    int i;
    int j;
    unsigned int * ptr_int;
    char * ptr_char;
    struct reboot_reason_pl *rr_pl;
    ram_console = (struct ram_console_buffer *)RAM_CONSOLE_SRAM_ADDR;

    if (((struct ram_console_buffer *)RAM_CONSOLE_SRAM_ADDR)->sig == RAM_CONSOLE_SIG) {
	ram_console = (struct ram_console_buffer *)RAM_CONSOLE_SRAM_ADDR;
	ram_console_size = RAM_CONSOLE_SRAM_SIZE;
    } else {
        print("%s sram(0x%x) sig 0x%x mismatch\n", MOD, ram_console, ram_console->sig);
	ram_console = (struct ram_console_buffer *)RAM_CONSOLE_DRAM_ADDR;
	ram_console_size = RAM_CONSOLE_DRAM_SIZE;
    }

    ptr_int = ram_console;
    for (i=0; i<0x10; i+=4) {
        print("%s %X %X %X %X", MOD, *(ptr_int + 0), *(ptr_int + 1), *(ptr_int + 2), *(ptr_int + 3));
        print(" | ");
        ptr_char = (char *)ptr_int;
        for (j=0; j<0x10; j+=4) {
            print("%c%c%c%c ", *(ptr_char + 0), *(ptr_char + 1), *(ptr_char + 2), *(ptr_char + 3));
            ptr_char += 4;
        }
        ptr_int += 4;
        print("\n");
	if (i != 0 && i%0x10 == 0x0c)
	    print("%s ------------------------------------------\n", MOD);
    }
    print("%s start:   0x%x, size: 0x%x\n", MOD, ram_console, ram_console_size);
    if (g_rgu_status != 0 &&
	ram_console->sig == RAM_CONSOLE_SIG &&
	ram_console->sz_pl == sizeof(struct reboot_reason_pl) &&
	ram_console->off_pl == sizeof(struct ram_console_buffer) &&
	ram_console->off_lpl == (ram_console->off_pl + ALIGN(ram_console->sz_pl, 64))) {
	print("%s preloader last status: ", MOD);
	rr_pl = (void*)ram_console + ram_console->off_pl;
	for (i = 0; i < RAM_CONSOLE_PL_SIZE; i++) {
	    print("0x%X ", rr_pl->last_func[i]);
	}
	print("\n");
	memcpy((void *)ram_console + ram_console->off_lpl, (void *)ram_console + ram_console->off_pl, ram_console->sz_pl);
    } else {
	memset(ram_console, 0, ram_console_size);
	ram_console->sig = RAM_CONSOLE_SIG;
	ram_console->off_pl = sizeof(struct ram_console_buffer);
	ram_console->sz_pl = sizeof(struct reboot_reason_pl);
	ram_console->off_lpl = ram_console->off_pl + ALIGN(ram_console->sz_pl, 64);
    }
}

void ram_console_reboot_reason_save(u32 rgu_status)
{
    struct reboot_reason_pl *rr_pl;
    if (ram_console) {
	rr_pl = (void*)ram_console + ram_console->off_pl;
	rr_pl->wdt_status = rgu_status;
	print("%s wdt status (0x%x)=0x%x\n", MOD,
	      rr_pl->wdt_status, rgu_status);
    }
}

void ram_console_pl_save(unsigned int val, int index)
{
    struct reboot_reason_pl *rr_pl;
    if (ram_console) {
	rr_pl = (void*)ram_console + ram_console->off_pl;
	if (index < RAM_CONSOLE_PL_SIZE)
	    rr_pl->last_func[index] = val;
    }
}

