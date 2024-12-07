PRJ = easy-stun

OBJS = main.o es_node.o es_node_local.o es_node_remote.o es_params.o

PRJ_LDFLAGS += -lssl -lcrypto

all: $(PRJ) $(LIB)

%.o: %.c
	$(CC) $(PROG_CFLAGS) -c -o $@ $<

$(PRJ): $(OBJS)
	$(CC) $(PROG_CFLAGS) -o $@ $^ $(PRJ_LDFLAGS)

clean:
	rm -f *.o $(PRJ)

.PHONY: all
