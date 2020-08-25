CC=gcc
OBJECTS=qlist.o udp.o qinfo.o main.o
TARGET=quakemg
CFLAGS=
LFLAGS=


$(TARGET): $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o $(TARGET)

