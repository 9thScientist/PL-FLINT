V=flint
$V: $V.fl sds/sds.c aux.c buffer.c
	flex $V.fl
	cc -g `pkg-config --cflags glib-2.0` lex.yy.c sds/sds.c aux.c buffer.c -o $V `pkg-config --libs glib-2.0`
