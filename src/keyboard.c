#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <bios.h>
#include <stdlib.h>
#include "io.h"
#include "protocol.h"
#include "plato_key.h"
#include "key.h"
#include "keyboard.h"
#include "prefs.h"
#include "touch.h"
#include "screen.h"

void keyboard_out(unsigned char platoKey)
{
    if (platoKey==0xff)
    return;
  
  if (platoKey>0x7F)
    {
      Key(ACCESS);
      Key(ACCESS_KEYS[platoKey-0x80]);
      return;
    }
  Key(platoKey);
  return;
  
}

void keyboard_main(void)
{
  unsigned char ch;
  unsigned short state;
  bool shift_pressed;
  bool ctrl_pressed;
  bool alt_pressed;
  
  if (kbhit())
    {
      ch=getch();
      state = _bios_keybrd(_KEYBRD_SHIFTSTATUS);

      if ((state&0x03)!=0) // Detect shift keys.
	shift_pressed=true;
      else
	shift_pressed=false;
      
      if ((state&0x04)!=0) // Detect ctrl key.
	ctrl_pressed=true;
      else
	ctrl_pressed=false;

      if ((state&0x08)!=0) // Detect alt key.
	alt_pressed=true;
      else
	alt_pressed=false;

      if (ch==0x00) // Extended key.
	{
	  ch=getch();
	  printf("0x%02x\n",ch);
	  if (ch==0x2D)
	    {
	      prefs_display("Exit PLATOTERM (Y/N)? ");
	      ch=prefs_get_key_matching("ynYN");
	      switch(ch)
		{
		case 'y':
		  touch_done();
		  screen_done();
		  exit(0);
		  break;
		case 'n':
		  prefs_clear();
		}
	    }
	  if (ch==0x3B)
	    prefs_run();
	  else if (shift_pressed==true)
	    {
	      keyboard_out(extended_shift_key_to_pkey[ch]);
	    }
	  else
	    {
	      keyboard_out(extended_key_to_pkey[ch]);
	    }
	}
      else if ((shift_pressed==true) && (ctrl_pressed==true))
	{
	  keyboard_out(ctrl_shift_key_to_pkey[ch]);
	}
      else
	{
	  keyboard_out(key_to_pkey[ch]);
	}
      
    }
}

void keyboard_clear(void)
{
}

void keyboard_out_tty(int ch)
{
  io_send_byte(ch);
  if (ch==0x0D)
    io_send_byte(0x0A); // Send a linefeed along with CR in TTY mode.
}
