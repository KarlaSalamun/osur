/*! Printing on stdout/stderr */
#pragma once

int printf ( char *format, ... );
int puts ( char *input );
void warn ( char *format, ... );

int stdio_init ();
