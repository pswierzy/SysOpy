CFLAGS = -Wall -std=c17 -g -fPIC
LDFLAGS = -L. -lcollatz -Wl,-rpath=.

LIB_OBJECTS = collatz.o

STATIC_LIB = libcollatz.a
SHARED_LIB = libcollatz.so

MAIN_OBJECT = main.o
DYNAMIC_OBJECT = collatz_dynamic.o

OUTPUT = program_static program_shared

all: static shared program_static program_shared program_dynamic

static: $(LIB_OBJECTS)
	ar rcs $(STATIC_LIB) $(LIB_OBJECTS)

shared: $(LIB_OBJECTS)
	gcc -shared -o $(SHARED_LIB) $(LIB_OBJECTS)

program_static: $(MAIN_OBJECT) static
	gcc $(MAIN_OBJECT) -o program_static $(LDFLAGS) -static

program_shared: $(MAIN_OBJECT) shared
	gcc $(MAIN_OBJECT) -o program_shared $(LDFLAGS)

program_dynamic: $(DYNAMIC_OBJECT) shared
	gcc $(DYNAMIC_OBJECT) -o program_dynamic $(LDFLAGS)

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(LIB_OBJECTS) $(MAIN_OBJECT) $(STATIC_LIB) $(SHARED_LIB) $(OUTPUT)
