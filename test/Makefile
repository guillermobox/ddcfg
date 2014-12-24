
all: test

.PHONY: test

test: main
	@echo
	@echo "This should fail"
	@echo
	./main configuration_fail.cfg test.inispec
	@echo
	@echo "This should pass"
	@echo
	./main configuration_pass.cfg test.inispec
	@echo

main: ../ddcfg.a main.c
	$(CC) main.c ../ddcfg.a -o main
