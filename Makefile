
DIRS=reglookuplib osinfo

all:
	set -e; for d in $(DIRS); do $(MAKE) -C $$d ; done
clean:
	set -e; for d in $(DIRS); do $(MAKE) -C $$d clean ; done
