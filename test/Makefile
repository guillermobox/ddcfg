
all: test

.PHONY: test

test: main
	@echo
	@echo "This should fail"
	@echo
	./main configuration_fail.cfg --check test.inispec
	@echo
	@echo "This should pass"
	@echo
	./main configuration_pass.cfg --check test.inispec
	@echo

main:
	$(CC) main.c ../ddcfg.a -o main
