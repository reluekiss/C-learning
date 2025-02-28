#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <png.h>

#ifndef null
#define null ((void *) 0)
#endif

#ifndef false
#define false (0)
#endif

#ifndef true
#define true (1)
#endif

enum {
	log_success,                    log_warning,                    log_failure,                    log_comment,
	log_count
};

enum {
	file_type_text,                 file_type_common_assembly,      file_type_flat_assembly,        file_type_gnu_assembly,
	file_type_netwide_assembly,     file_type_yet_another_assembly, file_type_c_source,             file_type_c_header,
	file_type_ada_body,             file_type_ada_specification,    file_type_cpp_source,           file_type_cpp_header,
	file_type_fortran_90,           file_type_shell,                file_type_cube_script,          file_type_xolatile_script,
	file_type_count
};

enum {
	effect_normal,                  effect_bold,                    effect_italic,                  effect_undefined_code,
	effect_underline,               effect_blink,                   effect_reverse,                 effect_invisible_text,
	effect_count
};

enum {
	colour_grey,                    colour_red,                     colour_green,                   colour_yellow,
	colour_blue,                    colour_pink,                    colour_cyan,                    colour_white,
	colour_count
};

enum {
	character_null,                 character_start_header,         character_start_text,           character_end_text,
	character_end_transmission,     character_enquiry,              character_acknowledge,          character_bell,
	character_backspace,            character_tab_horizontal,       character_line_feed,            character_tab_vertical,
	character_form_feed,            character_carriage_return,      character_shift_out,            character_shift_in,
	character_data_link_escape,     character_device_control_1,     character_device_control_2,     character_device_control_3,
	character_device_control_4,     character_not_acknowledge,      character_synchronous_idle,     character_end_transmission_block,
	character_cancel,               character_end_medium,           character_substitute,           character_escape,
	character_file_separator,       character_group_separator,      character_record_separator,     character_unit_separator
};

enum {
	cursor_none,                    cursor_left,                    cursor_middle,                  cursor_right,
	cursor_wheel_up,                cursor_wheel_down,
	cursor_count
};

enum {
	signal_none,
	signal_a,                       signal_b,                       signal_c,                       signal_d,
	signal_e,                       signal_f,                       signal_g,                       signal_h,
	signal_i,                       signal_j,                       signal_k,                       signal_l,
	signal_m,                       signal_n,                       signal_o,                       signal_p,
	signal_q,                       signal_r,                       signal_s,                       signal_t,
	signal_u,                       signal_v,                       signal_w,                       signal_x,
	signal_y,                       signal_z,                       signal_0,                       signal_1,
	signal_2,                       signal_3,                       signal_4,                       signal_5,
	signal_6,                       signal_7,                       signal_8,                       signal_9,
	signal_escape,                  signal_tabulator,               signal_return,                  signal_new_line,
	signal_slash,                   signal_backslash,               signal_semicolon,               signal_backquote,
	signal_space,                   signal_backspace,               signal_dot,                     signal_comma,
	signal_cite,                    signal_caps_lock,               signal_minus,                   signal_equal,
	signal_left_bracket,            signal_right_bracket,           signal_left_control,            signal_right_control,
	signal_left_shift,              signal_right_shift,             signal_left_alt,                signal_right_alt,
	signal_f1,                      signal_f2,                      signal_f3,                      signal_f4,
	signal_f5,                      signal_f6,                      signal_f7,                      signal_f8,
	signal_f9,                      signal_f10,                     signal_f11,                     signal_f12,
	signal_arrow_up,                signal_arrow_down,              signal_arrow_left,              signal_arrow_right,
	signal_num_lock,                signal_pause_break,             signal_insert,                  signal_home,
	signal_page_up,                 signal_delete,                  signal_end,                     signal_page_down,
	signal_key_add,                 signal_key_subtract,            signal_key_multiply,            signal_key_divide,
	signal_key_enter,               signal_key_dot,                 signal_key_0,                   signal_key_1,
	signal_key_2,                   signal_key_3,                   signal_key_4,                   signal_key_5,
	signal_key_6,                   signal_key_7,                   signal_key_8,                   signal_key_9,
	signal_count
};

static void input (const void * data, int size) {
	read (STDIN_FILENO, data, (unsigned long int) size);
}

static void output (const void * data, int size) {
	write (STDOUT_FILENO, data, (unsigned long int) size);
}

static void clean_up (void (* procedure) (void)) {
	atexit (procedure);
}

static int string_length (const char * string) {
	int length;

	if (string == null) {
		return (0);
	}

	for (length = 0; string [length] != '\0'; ++length);

	return (length);
}

static void echo (const char * data) {
	if (data == null) {
		return;
	}

	output (data, string_length (data));
}

static void echo_byte (int byte) {
	output ("0123456789ABCDEF" + (byte % 256) / 16, 1);
	output ("0123456789ABCDEF" + (byte % 256) % 16, 1);
	output (" ",                                    1);
}

static void fatal_failure (int condition, const char * message) {
	if (condition != 0) {
		echo ("[\033[1;31m FATAL \033[0m] ");
		echo (message);
		echo ("\n");
		exit (log_failure);
	}
}

static int randomize (int minimum, int maximum) {
	return (rand () % (maximum - minimum + 1) + minimum);
}

static int tick_tock (void) {
	return ((int) clock ());
}

static void limit (int * value, int minimum, int maximum) {
	if (value == null) {
		return;
	}

	if (* value <= minimum) {
		* value = minimum;
	}

	if (* value >= maximum) {
		* value = maximum;
	}
}

static void * allocate (int size) {
	char * data = null;

	if (size <= 0) {
		return (null);
	}

	data = calloc ((unsigned long int) size, sizeof (* data));

	fatal_failure (data == null, "allocate : Failed to allocate memory, internal function 'calloc' returned null pointer.");

	return ((void *) data);
}

static void * reallocate (void * data, int size) {
	if (size <= 0) {
		return (data);
	}

	data = realloc (data, (unsigned long int) size);

	fatal_failure (data == null, "reallocate: Failed to reallocate memory, internal function 'realloc' returned null pointer.");

	return (data);
}

static void * deallocate (void * data) {
	if (data != null) {
		free (data);
	}

	return (null);
}

static void * record (void) {
	char * buffer = null;
	int    offset = 0;
	int    loling = 1024;

	buffer = reallocate (buffer, loling);

	do {
		if ((offset + 1) % loling == 0) {
			buffer = reallocate (buffer, ((offset + 1) / loling + 1) * loling);
		}

		buffer [offset] = '\0';

		input (& buffer [offset], (int) sizeof (* buffer));

		++offset;
	} while (buffer [offset - 1] != '\0');

	buffer [offset - 1] = '\0';

	return (buffer);
}

static int character_compare_array (char character, const char * character_array) {
	int i = 0;

	do {
		if (character == character_array [i]) {
			return (true);
		}
	} while (++i != string_length (character_array));

	return (false);
}

static int character_count (const char * string, char this, int from, int to, char stop) {
	int count;

	for (count = 0; (from != to) && (string [from] != stop); from += ((to < from) ? -1 : 1)) {
		count += (int) ((string [from] == this) || (this == '\0'));
	}

	return (count);
}

static int character_is_uppercase (char character) {
	return ((int) ((character >= 'A') && (character <= 'Z')));
}

static int character_is_lowercase (char character) {
	return ((int) ((character >= 'a') && (character <= 'z')));
}

static int character_is_digit (char character) {
	return ((int) ((character >= '0') && (character <= '9')));
}

static int character_is_blank (char character) {
	return ((int) ((character == ' ') || (character == '\t') || (character == '\r') || (character == '\n')));
}

static int character_is_alpha (char character) {
	return ((character_is_uppercase (character) != 0) || (character_is_lowercase (character) != 0));
}

static int character_is_symbol (char character) {
	return (character_compare_array (character, "~!@#$%^&*()+{}|:\"<>?`-=[]\\;',./"));
}

static int character_is_visible (char character) {
	return ((int) ((character >= ' ') && (character <= '~')));
}

static int character_is_invisible (char character) {
	return (character_is_visible (character) == 0);
}

static int character_is_escape (char character) {
	return ((int) (character == '\033'));
}

static int character_is_underscore (char character) {
	return ((int) (character == '_'));
}

static int character_is_hexadecimal (char character) {
	return (character_compare_array (character, "0123456789ABCDEF"));
}

static char * string_reverse_limit (char * string, int limit) {
	int i;

	fatal_failure (string == null, "string_reverse: String is null pointer.");

	for (i = 0; i < limit / 2; ++i) {
		char temporary         = string [i];
		string [i]             = string [limit - 1 - i];
		string [limit - 1 - i] = temporary;
	}

	return (string);
}

static char * string_reverse (char * string) {
	return (string_reverse_limit (string, string_length (string)));
}

static char * string_delete (char * string, int length) {
	int i;

	if ((string == null) || (length <= 0)) {
		return (string);
	}

	for (i = 0; i != length; ++i) {
		string [i] = '\0';
	}

	return (string);
}

static int string_compare (const char * string_0, const char * string_1) {
	int i = 0;

	fatal_failure (string_0 == null, "string_compare: Destination string is null pointer.");
	fatal_failure (string_1 == null, "string_compare: Source string is null pointer.");

	for (i = 0; (string_0 [i] != '\0') && (string_1 [i] != '\0'); ++i) {
		if (string_0 [i] != string_1 [i]) {
			return (false);
		}
	}

	if (string_0 [i] != string_1 [i]) {
		return (false);
	}

	return (true);
}

static char * string_copy (char * string_0, const char * string_1) {
	int i = 0;

	fatal_failure (string_0 == null, "string_copy: Destination string is null pointer.");
	fatal_failure (string_1 == null, "string_copy: Source string is null pointer.");

	for (i = 0; i != string_length (string_1) + 1; ++i) {
		string_0 [i] = string_1 [i];
	}

	return (string_0);
}

static char * string_concatenate (char * string_0, const char * string_1) {
	string_0 += string_length (string_0);

	while (* string_1 != '\0') {
		* string_0++ = * string_1++;
	}

	* string_0 = '\0';

	return (string_0);
}

static int string_compare_limit (const char * string_0, const char * string_1, int limit) {
	int i = 0;

	fatal_failure (string_0 == null, "string_compare: Destination string is null pointer.");
	fatal_failure (string_1 == null, "string_compare: Source string is null pointer.");

	for (i = 0; (i != limit) && (string_0 [i] != '\0') && (string_1 [i] != '\0'); ++i) {
		if (string_0 [i] != string_1 [i]) {
			return (false);
		}
	}

	return (true);
}

static char * string_copy_limit (char * string_0, const char * string_1, int limit) {
	int i = 0;

	if (limit <= 0) {
		return (string_0);
	}

	for (i = 0; i != limit; ++i) {
		string_0 [i] = string_1 [i];
	}

	return (string_0);
}

static char * string_concatenate_limit (char * string_0, const char * string_1, int limit) {
	int i        = 0;
	int length_0 = 0;
	int length_1 = 0;

	if (limit <= 0) {
		return (string_0);
	}

	length_0 = string_length (string_0);
	length_1 = string_length (string_1);

	for (i = 0; (i != length_1) && (i != limit); ++i) {
		string_0 [length_0 + i] = string_1 [i];
	}

	return (string_0);
}

static char * string_realign (char * string, int amount, char character) {
	int offset, length;

	length = string_length (string);

	for (offset = 0; offset != length; ++offset) {
		string [amount - offset - 1] = string [length - offset - 1];
	}

	for (offset = 0; offset != amount - length; ++offset) {
		string [offset] = character;
	}

	string [amount] = '\0';

	return (string);
}

static void memory_delete (void * memory, int length) {
	int i = 0;

	char * cast = (char *) memory;

	fatal_failure (memory == null, "memory_delete: Memory is null pointer.");

	if (length <= 0) {
		return;
	}

	for (i = 0; i != length; ++i) {
		cast [i] = '\0';
	}
}

static int memory_compare (void * memory, const void * source, int length) {
	int i = 0;

	char * cast_0 = (char *) memory;
	char * cast_1 = (char *) source;

	fatal_failure (memory == null, "memory_compare: Memory is null pointer.");
	fatal_failure (source == null, "memory_compare: Source is null pointer.");

	if (length <= 0) {
		return (-1);
	}

	for (i = 0; (cast_0 [i] != '\0') && (cast_1 [i] != '\0'); ++i) {
		if (cast_0 [i] != cast_1 [i]) {
			return (false);
		}
	}

	return (true);
}

static void memory_copy (void * memory, const void * source, int length) {
	int i = 0;

	char * cast_0 = (char *) memory;
	char * cast_1 = (char *) source;

	fatal_failure (memory == null, "memory_copy: Memory is null pointer.");
	fatal_failure (source == null, "memory_copy: Source is null pointer.");

	if (length <= 0) {
		return;
	}

	for (i = 0; i != length; ++i) {
		cast_0 [i] = cast_1 [i];
	}
}

static void terminal_clear (void) {
	echo ("\033[2J\033[H");
}

static void terminal_colour (int colour, int effect) {
	char format [8] = "\033[ ;3 m";

	format [2] = (char) (effect % effect_count) + '0';
	format [5] = (char) (colour % colour_count) + '0';

	echo (format);
}

static void terminal_cancel (void) {
	echo ("\033[0m");
}

static void terminal_show_cursor (int show) {
	if (show != 0) {
		echo ("\033[?25h");
	} else {
		echo ("\033[?25l");
	}
}

static int encode_byte (const char * byte) {
	int encode = 0;

	fatal_failure (character_is_hexadecimal (byte [0]) == 0, "encode_byte: Upper byte character is not hexadecimal digit.");
	fatal_failure (character_is_hexadecimal (byte [1]) == 0, "encode_byte: Lower byte character is not hexadecimal digit.");

	encode = ((byte [0] - (character_is_digit (byte [0]) ? ('0') : ('A' - 10))) << 4) | (byte [1] - (character_is_digit (byte [1]) ? ('0') : ('A' - 10)));

	return (encode);
}

static char * decode_byte (int byte) {
	char * decode = "  ";

	fatal_failure ((byte <= -1) || (byte >= 256), "decode_byte: Byte is out of range.");

	decode [0] = byte / 16;
	decode [1] = byte % 16;

	return (decode);
}

static int file_open (const char * name, int mode) {
	int descriptor = -1;

	fatal_failure (name == null, "file_open: Failed to open file, name is null pointer.");

	descriptor = open (name, mode, 0666);

	fatal_failure (descriptor == -1, "file_open: Failed to open file, function open returned invalid descriptor.");

	return (descriptor);
}

static int file_close (int file) {
	fatal_failure (file         == -1, "file_close: Failed to close file, invalid file descriptor.");
	fatal_failure (close (file) == -1, "file_close: Failed to close file, function close returned invalid code.");

	return (-1);
}

static void file_read (int file, const void * data, int size) {
	fatal_failure (file == -1,   "file_read: Failed to read from file, invalid descriptor.");
	fatal_failure (data == null, "file_read: Failed to read from file, data is null pointer.");
	fatal_failure (size == 0,    "file_read: Failed to read from file, size is zero.");

	read (file, data, (unsigned long int) size);
}

static void file_write (int file, const void * data, int size) {
	fatal_failure (file == -1,   "file_write: Failed to write to file, invalid descriptor.");
	fatal_failure (data == null, "file_write: Failed to write to file, data is null pointer.");
	fatal_failure (size == 0,    "file_write: Failed to write to file, size is zero.");

	write (file, data, (unsigned long int) size);
}

static void file_echo (int file, const char * data) {
	file_write (file, data, string_length (data));
}

static int file_seek (int file, int whence) {
	fatal_failure (file == -1, "file_seek: Failed to seek in file, invalid descriptor.");

	return ((int) lseek (file, 0, whence));
}

static int file_size (const char * name) {
	int size = -1;
	int file = -1;

	file = file_open (name, O_RDONLY);

	size = lseek (file, 0, SEEK_END);

	fatal_failure (size == -1, "file_size: Failed to get size of file, invalid file size.");

	file = file_close (file);

	return (size);
}

static int file_type (const char * name) {
	char * extensions [file_type_count] = {
		".txt",         ".asm",         ".fasm",        ".gasm",        ".nasm",        ".yasm",        ".c",           ".h",
		".adb",         ".ads",         ".cpp",         ".hpp",         ".f90",         ".sh",          ".cfg",         ".x"
	};

	int type = 0;

	for (; * name != '.'; ++name);

	for (type = 0; type != file_type_count; ++type) {
		if (string_compare_limit (name, extensions [type], string_length (extensions [type]))) {
			return (type);
		}
	}

	return (-1);
}

static void * file_record (const char * name) {
	int    file = -1;
	int    size = -1;
	char * data = null;

	fatal_failure (name == null, "file_record: Failed to import file, name is null pointer.");

	file = file_open (name, O_RDONLY);
	size = file_size (name);
	data = allocate  (size + 1);

	file_read (file, data, size);

	file = file_close (file);

	return (data);
}

static char * file_import (const char * name) {
	int    file = -1;
	int    size = -1;
	char * data = null;

	fatal_failure (name == null, "file_import: Failed to import file, name is null pointer.");

	file = file_open (name, O_RDONLY);
	size = file_size (name) + 1;
	data = allocate  (size);

	file_read (file, data, size - 1);

	data [size - 1] = '\0';

	file = file_close (file);

	return (data);
}

static void file_export (const char * name, const void * data) {
	(void) name;
	(void) data;
}

static char * number_to_string (int number) { /* refactor me please... */
	int i, sign;

	static char string [32];

	string_delete (string, 32);

	if (number == 0) {
		string [0] = '0';
		string [1] = '\0';
		return (string);
	}

	if (number < 0) {
		number *= -1;
		sign = 1;
	} else {
		sign = 0;
	}

	for (i = (string [0] == '-'); number != 0; ++i) {
		string [i] = (char) (number % 10) + '0';
		number /= 10;
	}

	if (sign != 0) {
		string [i] = '-';
		++i;
	}

	string [i] = '\0';

	string_reverse (string);

	return (string);
}

static char * format_to_string (int number, int sign, int base, int amount, char character) { /* refactor me please... */
	static char string [32];

	int i;

	string_delete (string, 32);

	if (number == 0) {
		string [0] = '0';
		string [1] = '\0';

		string_realign (string, amount, character);

		return (string);
	}

	if (number < 0) {
		number *= -1;
	}

	for (i = (string [0] == '-'); number != 0; ++i) {
		string [i] = "0123456789ABCDEF" [number % base];
		number /= base;
	}

	if (sign != 0) {
		string [i] = '-';
		++i;
	}

	string [i] = '\0';

	string_reverse (string);

	string_realign (string, amount, character);

	return (string);
}

static void print (const char * format, ...) { /* refactor me please... */
	va_list list;

	va_start (list, format);

	for (; * format != character_null; ++format) {
		switch (* format) {
			case '%': {
				++format;
				switch (* format) {
					case '%': {
						output ("%", 1);
					} break;
					case 'i': {
						char * string = number_to_string (va_arg (list, int));
						output (string, string_length (string));
					} break;
					case 't': {
						int toggle = (va_arg (list, int));
						terminal_colour ((toggle == true) ? colour_green : colour_red, effect_normal);
						output ((toggle == true) ? "+" : "-", 1);
						terminal_cancel ();
					} break;
					case 's': {
						char * string = va_arg (list, char *);
						output (string, string_length (string));
					} break;
					default: {
						output ("?", 1);
					} break;
				}
			} break;
			case '/': {
				++format;
				switch (* format) {
					case '/': output ("/", 1);                                break;
					case '0': terminal_colour (colour_grey,   effect_normal); break;
					case '1': terminal_colour (colour_red,    effect_normal); break;
					case '2': terminal_colour (colour_green,  effect_normal); break;
					case '3': terminal_colour (colour_yellow, effect_normal); break;
					case '4': terminal_colour (colour_blue,   effect_normal); break;
					case '5': terminal_colour (colour_pink,   effect_normal); break;
					case '6': terminal_colour (colour_cyan,   effect_normal); break;
					case '7': terminal_colour (colour_white,  effect_normal); break;
					case '-': terminal_cancel ();                             break;
					default:  output ("?", 1);                                break;
				}
			} break;
			default: {
				output (format, 1);
			} break;
		}
	}

	va_end (list);
}

static void error (const char * format, ...) { /* refactor me please... */
	va_list list;

	va_start (list, format);

	for (; * format != character_null; ++format) {
		switch (* format) {
			case '%': {
				++format;
				switch (* format) {
					case '%': {
						output ("%", 1);
					} break;
					case 'i': {
						char * string = number_to_string (va_arg (list, int));
						output (string, string_length (string));
					} break;
					case 't': {
						int toggle = (va_arg (list, int));
						terminal_colour ((toggle == true) ? colour_green : colour_red, effect_normal);
						output ((toggle == true) ? "+" : "-", 1);
						terminal_cancel ();
					} break;
					case 's': {
						char * string = va_arg (list, char *);
						output (string, string_length (string));
					} break;
					default: {
						output ("?", 1);
					} break;
				}
			} break;
			case '/': {
				++format;
				switch (* format) {
					case '/': output ("/", 1);                                break;
					case '0': terminal_colour (colour_grey,   effect_normal); break;
					case '1': terminal_colour (colour_red,    effect_normal); break;
					case '2': terminal_colour (colour_green,  effect_normal); break;
					case '3': terminal_colour (colour_yellow, effect_normal); break;
					case '4': terminal_colour (colour_blue,   effect_normal); break;
					case '5': terminal_colour (colour_pink,   effect_normal); break;
					case '6': terminal_colour (colour_cyan,   effect_normal); break;
					case '7': terminal_colour (colour_white,  effect_normal); break;
					case '-': terminal_cancel ();                             break;
					default:  output ("?", 1);                                break;
				}
			} break;
			default: {
				output (format, 1);
			} break;
		}
	}

	va_end (list);

	exit (log_failure);
}

static char * format (const char * base, ...) { /* refactor me please... */
	static char output [256];

	va_list list;

	string_delete (output, 256);

	va_start (list, base);

	for (; * base != character_null; ++base) {
		switch (* base) {
			case '%': {
				++base;
				switch (* base) {
					case '%': string_concatenate (output, "%");                                     break;
					case 'i': string_concatenate (output, number_to_string (va_arg (list, int)));   break;
					case 's': string_concatenate (output,                   va_arg (list, char *)); break;
					default:  string_concatenate (output, "?");                                     break;
				}
			} break;
			case '/': {
				++base;
				switch (* base) {
					case '/': string_concatenate (output, "/");          break;
					case '0': string_concatenate (output, "\033[1;30m"); break;
					case '1': string_concatenate (output, "\033[1;31m"); break;
					case '2': string_concatenate (output, "\033[1;32m"); break;
					case '3': string_concatenate (output, "\033[1;33m"); break;
					case '4': string_concatenate (output, "\033[1;34m"); break;
					case '5': string_concatenate (output, "\033[1;35m"); break;
					case '6': string_concatenate (output, "\033[1;36m"); break;
					case '7': string_concatenate (output, "\033[1;37m"); break;
					case '-': string_concatenate (output, "\033[0m");    break;
					default:  string_concatenate (output, "?");          break;
				}
			} break;
			default: {
				string_concatenate_limit (output, base, 1);
			} break;
		}
	}

	va_end (list);

	return (output);
}

static void swap_element (int * array, int swap, int with) {
	int temporary = array [swap];

	array [swap] = array [with];
	array [with] = temporary;
}

static int execute (const char * command) {
	return (system (command));
}

static void * import_png_image (const char * path, int * width, int * height) {
	FILE * file;
	int  * data;
	int    index;

	png_byte    colour_type  = 0;
	png_byte    bit_depth    = 0;
	png_bytep * row_pointers = null;

	png_structp structure   = null;
	png_infop   information = null;

	file = fopen (path, "rb");

	fatal_failure (file == null, path);

	structure   = png_create_read_struct (PNG_LIBPNG_VER_STRING, null, null, null);
	information = png_create_info_struct (structure);

	png_init_io   (structure, file);
	png_read_info (structure, information);

	* width       = (int) png_get_image_width  (structure, information);
	* height      = (int) png_get_image_height (structure, information);
	  colour_type =       png_get_color_type   (structure, information);
	  bit_depth   =       png_get_bit_depth    (structure, information);

	if (bit_depth == 16) {
		png_set_strip_16 (structure);
	}

	if (colour_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb (structure);
	}

	if ((colour_type == PNG_COLOR_TYPE_GRAY) && (bit_depth < 8)) {
		png_set_expand_gray_1_2_4_to_8 (structure);
	}

	if (png_get_valid (structure, information, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha (structure);
	}

	if ((colour_type == PNG_COLOR_TYPE_RGB) || (colour_type == PNG_COLOR_TYPE_GRAY) || (colour_type == PNG_COLOR_TYPE_PALETTE)) {
		png_set_filler (structure, 0xff, PNG_FILLER_AFTER);
	}

	if ((colour_type == PNG_COLOR_TYPE_GRAY) || (colour_type == PNG_COLOR_TYPE_GRAY_ALPHA)) {
		png_set_gray_to_rgb (structure);
	}

	png_read_update_info (structure, information);

	row_pointers = allocate ((* height) * (int) sizeof (* row_pointers));

	for (index = 0; index < (* height); ++index) {
		row_pointers [index] = allocate ((int) png_get_rowbytes (structure, information));
	}

	png_read_image (structure, row_pointers);

	fclose (file);

	data = allocate ((* width) * (* height) * (int) sizeof (* data));

	for (index = 0; index < (* height); ++index) {
		memory_copy (& data [index * (* width)], row_pointers [index], (* width) * (int) sizeof (* data));

		row_pointers [index] = deallocate (row_pointers [index]);
	}

	row_pointers = deallocate (row_pointers);

	png_destroy_read_struct (& structure, & information, null);

	return (data);
}

static void export_png_image (const char * path, int * data, int width, int height) {
	png_image image = { 0 };

	image.version = PNG_IMAGE_VERSION;
	image.format  = PNG_FORMAT_RGBA;
	image.width   = (unsigned int) width;
	image.height  = (unsigned int) height;

	png_image_write_to_file (& image, path, 0, data, 0, null);

	png_image_free (& image);
}

