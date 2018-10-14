PHONY_TARGS = build-debug build-s1 build-s2 build-s3 build-s4 build-s5

.PHONY: all $(PHONY_TARGS)

all: $(PHONY_TARGS)

build-debug:
	cd debug && $(MAKE) gitrev.h && $(MAKE)

build-s1:
	cd s1 && $(MAKE) gitrev.h && $(MAKE)

build-s2:
	cd s2 && $(MAKE) gitrev.h && $(MAKE)

build-s3:
	cd s3 && $(MAKE) gitrev.h && $(MAKE)

build-s4:
	cd s4 && $(MAKE) gitrev.h && $(MAKE)

build-s5:
	cd s5 && $(MAKE) gitrev.h && $(MAKE)
