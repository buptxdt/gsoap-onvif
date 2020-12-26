include Makefile.inc
 
PROGRAM = PTZ
 
SOURCES += myptz.c
 
OBJECTS := $(patsubst %.c,$(TEMPDIR)%.o,$(filter %.c, $(SOURCES)))
 
all: $(OBJECTS_ONVIF) $(OBJECTS_COMM) $(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS_ONVIF) $(OBJECTS_COMM) $(OBJECTS) $(LDLIBS)
 
clean:
	rm -f $(OBJECTS_ONVIF)
	rm -f $(OBJECTS_COMM)
	rm -f $(OBJECTS)
	rm -f $(PROGRAM)


