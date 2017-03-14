all: tests

.PHONY: tests testdriver all clean

clean:
	$(RM) -f testdriver

tests: testdriver
	@echo
	@echo Running tests
	@echo
	@python2 ybt.py testsuite.yaml
	@echo

testdriver: testdriver.c ../ddcfg.a
	$(CC) $^ -o $@
