#include <monitor.h>


#define MON_DEFAULT_ATTR_BYTE	((0/* black */ << 4) | (15/* white */ & 0x0F))
#define MON_BLANK_CHAR		(0x20/* space */ | (MON_DEFAULT_ATTR_BYTE << 8))

#define MON_WIDTH	80
#define MON_HEIGHT	25


static void	move_cursor(void); /* Updates the hardware cursor. */
static void	scroll(void); /* Scrolls the text on the screen up by one line. */


// The VGA framebuffer starts at 0xB8000.
static uint16_t *video_memory = (uint16_t *)0xB8000;
// Stores the cursor position.
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;


static void
move_cursor(void)
{
	/* cursor location */
	uint16_t cl = cursor_y * MON_WIDTH + cursor_x;
	outb(0x3D4, 14);      /* Tell the VGA board we are setting the high cursor byte. */
	outb(0x3D5, cl >> 8); /* Send the high cursor byte. */
	outb(0x3D4, 15);      /* Tell the VGA board we are setting the low cursor byte. */
	outb(0x3D5, cl);      /* Send the low cursor byte. */
}


static void
scroll(void)
{
	int i, w = MON_WIDTH, h = MON_HEIGHT;
	/* Get a space character with the default colour attributes. */

	if (cursor_y >= h) { /* we need to scroll up */
		/* move each lines to the previous one */
		for (i = 0; i < w * (h - 1); i++)
			video_memory[i] = video_memory[i + w];
		/* The last line should now be blank. Do this by writing 80
		   spaces to it. */
		for (i = w * (h - 1); i < w * h; i++)
			video_memory[i] = MON_BLANK_CHAR;
	       /* The cursor should now be on the last line. */
		cursor_y = (h - 1);
	}
}


void mon_putchar(char c)
{
	/* The background colour is black (0), the foreground is green (2). */
	uint8_t bc = 0, fc = 2;

	/* The attribute byte is made up of two nibbles - the lower being the
	   foreground colour, and the upper the background colour. */
	uint8_t attr_byte = (bc << 4) | (fc & 0x0F);
	/* The attribute byte is the top 8 bits of the word we have to send to
	   the VGA board. */
	uint16_t attr = attr_byte << 8;
	uint16_t *location;

	if (c == 0x08 && cursor_x > 0) {
		/* Handle a backspace, by moving the cursor back one space */
		cursor_x--;
	} else if (c == 0x09) {
		/* Handle a tab by increasing the cursor's X, but only to a point where
		   it is divisible by 8. */
		cursor_x = (cursor_x + 8) & ~(8 - 1);
	} else if (c == '\r') {
		/* Handle carriage return */
		cursor_x = 0;
	} else if (c == '\n') {
		/* Handle newline return */
		cursor_x = 0;
		cursor_y++;
	} else if (c >= ' ') {
		/* Handle any other printable char */
		location = video_memory + (cursor_x++ + MON_WIDTH * cursor_y);
		*location = c | attr;
	}

	if (cursor_x >= MON_WIDTH) {
		cursor_x = 0;
		cursor_y++;
	}
	scroll();
	move_cursor();
}


void
mon_clear(void)
{
	int i;

	for (i = 0; i < MON_WIDTH * MON_HEIGHT; i++)
		video_memory[i] = MON_BLANK_CHAR;
	cursor_x = cursor_y = 0;
	move_cursor();
}
