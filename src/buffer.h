/*
 * FUNCTION BUFFER
 */
#ifndef __BUFFER_H__
#define __BUFFER_H__

typedef struct buffer *BUFFER;

// Initializes function buffer
BUFFER b_init();
// Registers a new variable
void reg_variable(BUFFER b, char* name, char* type);
// Concats string to line
void cat_line(BUFFER b, char* str);
// Adds attributes to line
void add_attribute(BUFFER b, char* str);
// Start building function with given name
void begin_function(BUFFER b, char* name);
// Builds function header
void build_header(BUFFER b);
// Builds a string_append line
void build_strapp_line(BUFFER b, int indent);
// Pushes new line to buffer
void push_line(BUFFER b, const char* str);
void push_sds_line(BUFFER b, sds str);
// Prints function buffer to out
void print_buffer(BUFFER b, FILE* out);
// Resets buffer
void reset(BUFFER b);

#endif
