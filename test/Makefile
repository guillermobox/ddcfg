all: tests

.PHONY: tests testdriver all clean

clean:
	$(RM) testdriver test_select test_configuration test_parsing

tests: testdriver test_select test_configuration test_parsing
	@echo
	@echo Running tests
	@echo
	@python2 ybt.py testsuite.yaml
	@echo

testdriver: testdriver.c ../ddcfg.a
	$(CC) -g $^ -o $@

test_select: test_select.c ../ddcfg.a
	$(CC) -g $^ -o $@

test_configuration: test_configuration.c ../ddcfg.a
	$(CC) -g $^ -o $@

test_parsing: test_parsing.c ../ddcfg.a
	$(CC) -g $^ -o $@
