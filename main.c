/*
 * GeekOS C code entry point
 * Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, Iulian Neamtiu <neamtiu@cs.umd.edu>
 * $Revision: 1.51 $
 *
 * Copyright (c) 2004 - 2011, Timothy Henry <thenry@cs.uri.edu>
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/bootinfo.h>	// for memory size
#include <geekos/memory.h>		// memory manager functions
#include <geekos/screen.h>		// screen constants and functions
#include <geekos/tss.h>			// TSS structure definiton
#include <geekos/interrupts.h>	// interrupt functions & Interrupt_State struct definition
#include <geekos/kthread.h>		// process management functions
#include <geekos/traps.h>		// trap / system call functionality
#include <geekos/timer.h>		// timer initialization
#include <geekos/keyboard.h>	// keyboard constants and functions

#include <stddef.h>	// for null

#define ASCII_RET 0x0d	// return key ASCII code

/*
 * echo keypress function
 */
void Echo_Keypress()
{
	// begin
	Print("Hello from Trey!\n");

	// setup variables
	int row, col;
	Keycode current;
	bool continue_flag = true;

	// loop until exit
	do {
		current = Wait_For_Key(); // wait for keypress
		if((Wait_For_Key()&KEY_RELEASE_FLAG) != 0)	// wait for KEY_RELEASE_FLAG
		{
			switch(current)	// process current Keycode
			{
				// if backspace
				case ASCII_BS:
					Get_Cursor(&row, &col);
					// if all the way left, do nothing
					if(col == 0)
						break;
					// else backspace
					Put_Cursor(row, col - 1);
					Print(" ");
					Put_Cursor(row, col - 1);
					break;
				// if return key
				case ASCII_RET:
					Get_Cursor(&row, &col);
					// go to next line
					Put_Cursor(row + 1, 0);
					break;
				// if ctrl-d
				case (KEY_CTRL_FLAG | 'd'):
					// end loop
					continue_flag = false;
					break;
				// print key if nothing else
				default:
					Print("%c", current);
			}
		}
	} while(continue_flag);	// will terminate when ctrl-d is pressed
							// dropping continue_flag

	// end
	Print("\nGoodbye!\n");
}


/*
 * Kernel C code entry point.
 * Initializes kernel subsystems, mounts filesystems,
 * and spawns init process.
 */
void Main(struct Boot_Info* bootInfo)
{
    Init_BSS();			// clear kernel BSS (memory.c)
    Init_Screen();		// clear display (screen.c)
    Init_Mem(bootInfo);	// initialize memory manager
    Init_TSS();			// initialize kernel TSS (tss.c)
    Init_Interrupts();	// initialize interrupt handlers (interrupts.c)
    Init_Scheduler();	// initalize and spawn scheduler thread (kthread.c)
    Init_Traps();		// initialize system call traps (traps.c)
    Init_Timer();		// initialize system timer (timer.c)
    Init_Keyboard();	// initialize keyboard (keyboard.c)


    Set_Current_Attr(ATTRIB(BLACK, GREEN|BRIGHT));
    Print("Welcome to GeekOS!\n");
    Set_Current_Attr(ATTRIB(BLACK, GRAY));

    //TODO("Start a kernel thread to echo pressed keys!");

	// Thread for Echo_Keypress()
    struct Kernel_Thread* thread = Start_Kernel_Thread(Echo_Keypress, NULL, PRIORITY_NORMAL, false);

    /* Now this thread is done. */
    Exit(0);
}
