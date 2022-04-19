TARGET_1 := servidor
TARGET_2 := cliente
TARGET_3 := servidor_multi
TARGET_4 := cliente_multi

TARGETS := $(TARGET_1) $(TARGET_2) $(TARGET_3) $(TARGET_4) $(TARGET_5) $(TARGET_6)

SHARED_LIB := utils
LD := LD_LIBRARY_PATH=/home/utnso/template/utils/Debug/

lib:
	-cd $(SHARED_LIB)/Debug && make all

$(TARGET_1): lib
	-cd $@/Debug && make all
	-cd $@/Debug && $(LD) ./$@

$(TARGET_2): lib
	-cd $@/Debug && make all
	-cd $@/Debug && $(LD) ./$@

$(TARGET_3): lib
	-cd $@/Debug && make all
	-cd $@/Debug && $(LD) ./$@

$(TARGET_4): lib
	-cd $@/Debug && make all
	-cd $@/Debug && $(LD) ./$@

clean:
	-cd $(SHARED_LIB)/Debug && make clean
	-cd $(TARGET_1)/Debug && make clean
	-cd $(TARGET_2)/Debug && make clean
	-cd $(TARGET_3)/Debug && make clean
	-cd $(TARGET_4)/Debug && make clean
	-rm -f *.log

build: lib
	-cd $(ARG)/Debug && make all


run: $(TARGETS)

all: clean run

memcheck: 
	$(MAKE) build ARG=$(PROCESO)
	$(LD) valgrind --leak-check=full --show-leak-kinds=all --log-file="$@-$(PROCESO).log" $(PROCESO)/Debug/$(PROCESO)


helgrind:
	$(MAKE) build ARG=$(PROCESO)
	$(LD) valgrind --tool=helgrind --log-file="$@-$(PROCESO).log" $(PROCESO)/Debug/$(PROCESO)

help:
	@echo "############## Rules #############"
	@echo " lib: compile the shared library (Set SHARED_LIB var)"
	@echo " TARGET_X: compile and run a target (Set TARGET_X vars)"
	@echo " clean: clean all project"
	@echo " run: run all targets (See available targets)"
	@echo " all: clean project and run targets"
	@echo " memcheck: run an specific target with valgrind. Example: make memcheck PROCESO=servidor"
	@echo " helgrind: same as rule memcheck"
	@echo ""


.PHONY: all run clean lib build memcheck helgrind help $(TARGETS)




