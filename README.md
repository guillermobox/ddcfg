Configuration parser for DDSIM
==============================

This is a simple configurator parser for the DDSIM simulator.

Try with make and then `./main.elf`.

Also try doing:

    ./main.elf --config QC.massX 12.34

And see how it changes, or with a bool:

    ./main.elf --config QC.activate TRUE
    ./main.elf --config QC.activate 1
    ./main.elf --config QC.activate trUE

Also, the python script `switch.py` can take a configuration file
as input, and generate a set of different configuration files
but changing the value of some options, according to the wanted
transformation, try this:

    python switch.py input_test.in QC.massX 1.0:0.1:2.0
    python switch.py input_test.in QC.activate true,false
    python switch.py input_test.in QC.massX 1-5,8-10
