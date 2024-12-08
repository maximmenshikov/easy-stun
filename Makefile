PRJ = easy-stun

OBJS = src/main.o src/es_node.o src/es_node_local.o src/es_node_remote.o src/es_params.o

PRJ_LDFLAGS += -lssl -lcrypto

all: $(PRJ) $(LIB)

%.o: %.c
	$(CC) $(PROG_CFLAGS) -g -c -o $@ $<

$(PRJ): $(OBJS)
	$(CC) $(PROG_CFLAGS) -o $@ $^ $(PRJ_LDFLAGS)

clean:
	rm -f *.o $(PRJ)

.PHONY: all
