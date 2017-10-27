all: tests

.PHONY: tests testdriver all clean

clean:
	$(RM) -f testdriver

tests: testdriver test_select
	@echo
	@echo Running tests
	@echo
	@python2 ybt.py testsuite.yaml
	@echo

testdriver: testdriver.c ../ddcfg.a
	$(CC) $^ -o $@

test_select: test_select.c ../ddcfg.a
	$(CC) $^ -o $@

test_configuration: test_configuration.c ../ddcfg.a
	$(CC) $^ -o $@

test_parsing: test_parsing.c ../ddcfg.a
	$(CC) $^ -o $@
