SECTION Device
	DESCRIPTION Characteristics of the device being simulated
	DESCRIPTION while we do this, of course
    DESCRIPTION that's the truth
PROPERTY workfunction
	TYPE double
	DESCRIPTION Value in eV of the workfunction to use
PROPERTY mesh_basename
	TYPE string
	DESCRIPTION The basename of the path of the mesh, up to the process number

FAILURE
DESCRIPTION You can't activate two different quantum mechanisms
CONDITION QC.activate AND SCH.activate

WARNING
DESCRIPTION The masses you are using are not physically realistic! See this article or that book for more information, you dodo head!
CONDITION QC.massX > 1.0 OR (QC.massY > 1.0 AND QC.massZ > 1.0)
