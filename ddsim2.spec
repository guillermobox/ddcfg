SECTION Device
	DESCRIPTION Characteristics of the device being simulated
	DESCRIPTION while we do this, of course
    DESCRIPTION that's the truth

	PROPERTY workfunction
		TYPE real
		DESCRIPTION Value in eV of the workfunction to use
		DESCRIPTION can be overwritten by another one

	PROPERTY mesh_basename
		DESCRIPTION The basename of the path of the mesh, up to the process number

SECTION RD
	DESCRIPTION Random dopants fluctuations

	PROPERTY activate
		TYPE boolean
		DESCRIPTION Activate the random dopants fluctuations

	PROPERTY damping_factor
		TYPE real
		DEPENDS_ON RD.activate
		DESCRIPTION Damping factor to be used with quantum corrections, 1.0 if none

	PROPERTY damping_steps
		TYPE integer
		DEPENDS_ON RD.activate
		DESCRIPTION Steps to be simulated to get the original doping back after aplying damping

FAILURE
	DESCRIPTION You can't activate two different quantum mechanisms
	CONDITION A.x > 2.0

SECTION A
	DESCRIPTION A section
	PROPERTY x
		DESCRIPTION This is x
		TYPE real

	PROPERTY y
		DESCRIPTION This is y
		TYPE boolean

WARNING
	DESCRIPTION The masses you are using are not physically realistic! See this article or that book for more information, you dodo head!
	CONDITION QC.massX > 1.0 OR (QC.massY > 1.0 AND QC.massZ > 1.0)
