SECTION Device
	DESCRIPTION Characteristics of the device being simulated
	DESCRIPTION while we do this, of course
DESCRIPTION that's the truth
PROPERTY workfunction
	TYPE real
	DESCRIPTION Value in eV of the workfunction to use
PROPERTY mesh_basename
	TYPE string
	DESCRIPTION The basename of the path of the mesh, up to the process number

SECTION Scale
	DESCRIPTION Scale the device after reading the mesh files
PROPERTY activate
	TYPE boolean
	DEFAULT no
	DESCRIPTION Activate the scaling
PROPERTY x
	TYPE real
	DEPENDS_ON Scale.activate
	DESCRIPTION Multiply the first dimension of all nodes by this number
PROPERTY y
	TYPE real
	DEPENDS_ON Scale.activate
	DESCRIPTION Multiply the second dimension of all nodes by this number
PROPERTY z
	TYPE real
	DEPENDS_ON Scale.activate
	DESCRIPTION Multiply the third dimension of all nodes by this number
PROPERTY eps
	TYPE real
	DEPENDS_ON Scale.activate
	DESCRIPTION Value of eps to apply to the device oxide after scaling

SECTION Solver
	DESCRIPTION Control the simulation general behaviour
PROPERTY loop_simulation
	TYPE boolean
	DEFAULT no
	DESCRIPTION Simulate the device. If false, only initialization is done
PROPERTY equi_simulation
	TYPE boolean
	DEFAULT no
	DESCRIPTION Simulate the device only in equilibrium.
PROPERTY Vth_fluct
	TYPE boolean
	DEFAULT no
	DESCRIPTION Search for the threshold voltage after simulating the input voltages
PROPERTY tolerance
	DEPENDS_ON Solver.Vth_fluct
	TYPE real
	DESCRIPTION Tolerance when searching for Vth fluctuations
PROPERTY threshold_current
	DEPENDS_ON Solver.Vth_fluct
	TYPE real
	DESCRIPTION Current in (A) that defines the threshold
PROPERTY step
	DEPENDS_ON Solver.Vth_fluct
	TYPE real
	DESCRIPTION Value in (V) to advance the gate voltage
PROPERTY max_step
	DEPENDS_ON Solver.Vth_fluct
	TYPE real
	DESCRIPTION Maximum step value after aplying Newton

SECTION Tecplot
	DESCRIPTION Control the mesh output in tecplot files
PROPERTY activate
	TYPE boolean
	DESCRIPTION Save the mesh each time fich_FEM is called
	DEFAULT no
PROPERTY cummulative
	TYPE boolean
	DESCRIPTION Save each time in a different file (WARNING, lots of space used)
	DEFAULT no
PROPERTY gzip
	TYPE boolean
	DESCRIPTION Compress the tecplot output in gzip (recommended with cummulative)
	DEFAULT no
	
	
SECTION SaveState
	DESCRIPTION Save/Recover the simulation state	
PROPERTY WRITE_EQUIL
	TYPE boolean
	DESCRIPTION Writes files saving the simulation state of the equilibrium
	DEFAULT no
PROPERTY WRITE_VOLTAGE
	TYPE boolean
	DESCRIPTION Writes files saving the simulation state of the equilibrium
	DESCRIPTION and all the potentials indicated to be simulated in voltages.dat
	DEFAULT no
PROPERTY READ_EQUIL
	TYPE boolean
	DESCRIPTION Reads files that contain the simulation state of the equilibrium
	DEFAULT no
PROPERTY READ_VOLTAGE
	TYPE boolean
	DESCRIPTION Reads files that contain the simulation state of the jumped code
	DESCRIPTION to the polarisation indicated by JUMP_TO_VOLTAGE_No
	DEFAULT no
PROPERTY JUMP_TO_VOLTAGE_No
	TYPE integer
	DEFAULT 1
	DESCRIPTION Value of the potential we want to jump to (eg. 3: indicates 3rd potential)
	DESCRIPTION IMPORT:the jump is to the point where the CURRENT for the indicated
	DESCRIPTION potential has just been calculated
	
	

SECTION FD
	DESCRIPTION Control the use of Boltzmann or Fermi-Dirac
PROPERTY activate
	TYPE boolean
	DEFAULT no
	DESCRIPTION value 0 for Boltzmann, value 1 for Fermi-Dirac	
	
	
SECTION HOLES
	DESCRIPTION  Simulations solving hole equations
PROPERTY activate
	TYPE boolean
	DEFAULT no
	DESCRIPTION Activates the solution of Continuity eq for holes 
	DESCRIPTION and DG equations for holes (Guess and Gummel functions)	
	
	
SECTION QC
	DESCRIPTION Control the quantum corrections simulation
PROPERTY activate
	TYPE boolean
	DEFAULT no
	DESCRIPTION Simulate the system using quantum corrections
PROPERTY R_bn
	TYPE real
	DEFAULT 3.0
	DESCRIPTION Parameter R_bn for bns_masa_cte and box_masa
PROPERTY massX
	TYPE real
	DEPENDS_ON QC.activate
	DESCRIPTION Quantum mass on Silicon in X direction
PROPERTY massY
	TYPE real
	DEPENDS_ON QC.activate
	DESCRIPTION Quantum mass on Silicon in Y direction
PROPERTY massZ
	TYPE real
	DEPENDS_ON QC.activate
	DESCRIPTION Quantum mass on Silicon in Z direction
PROPERTY massX_p
	TYPE real
	DEPENDS_ON QC.activate
	DEFAULT 0.0
	DESCRIPTION Hole Quantum mass on Silicon in X direction
PROPERTY massY_p
	TYPE real
	DEPENDS_ON QC.activate
	DEFAULT 0.0
	DESCRIPTION Hole Quantum mass on Silicon in Y direction
PROPERTY massZ_p
	TYPE real
	DEPENDS_ON QC.activate
	DEFAULT 0.0
	DESCRIPTION Hole Quantum mass on Silicon in Z direction
PROPERTY massOX
	TYPE real
	DEPENDS_ON QC.activate
	DESCRIPTION Quantum mass on Oxide
PROPERTY masa_ox_xp
	TYPE real
	DEFAULT 0.4
	DESCRIPTION Parameter electron effective mass for xp, Silicon 0.4
PROPERTY potencial_barrier
	TYPE real
	DEFAULT 1.5
	DESCRIPTION Parameter eV, potencial barrier of the oxide
	DESCRIPTION (afinidad del metal)-(afinidad semiconductor) para
	DESCRIPTION silicio 3.10 eV para oxido de hafnio 1.5 eV


SECTION SCH
	DESCRIPTION Control the 2D Schrodinger quantum correction simulation
PROPERTY activate
	TYPE boolean
	DEFAULT no
	DESCRIPTION Simulate the system using quantum correction
PROPERTY Tecplot
	TYPE boolean
	DEPENDS_ON SCH.activate
	DEFAULT no
	DESCRIPTION Print the nodes/elements of the extracted slices
PROPERTY file
	DEPENDS_ON SCH.activate
	DESCRIPTION Input file with the positions of the 2D slices
PROPERTY anisotropic
	TYPE string
	DEPENDS_ON SCH.activate
	DEFAULT anisotropic
	VALUES anisotropic, isotropic
	DESCRIPTION Select if the Effective Mass Tensor (EMT) is anisotropic or isotropic, for the Schrodinger equation
PROPERTY orientation
	TYPE integer
	DEPENDS_ON SCH.activate
	DEFAULT 110
	VALUES 110,100
	DESCRIPTION Crystal orientation of the channel [100 or 110]
PROPERTY Temperature
	TYPE integer
	DEFAULT 300
	DESCRIPTION Transverse temperature [K]
PROPERTY numEig
	TYPE integer
	DEPENDS_ON SCH.activate
	DEFAULT 20
	DESCRIPTION The number of eigenvalues to be calculated
PROPERTY nblock
	TYPE integer
	DEPENDS_ON SCH.activate
	DEFAULT 10
	DESCRIPTION (Solver parameters) choose NBLOCK and NSTEPS so that
	DESCRIPTION NSTEPS*NBLOCK lies in the range of 3*NUMEIG to 10*NUMEIG
PROPERTY nsteps
	TYPE integer
	DEPENDS_ON SCH.activate
	DEFAULT 8
	DESCRIPTION (Solver parameters) choose NBLOCK and NSTEPS so that 
	DESCRIPTION NSTEPS*NBLOCK lies in the range of 3*NUMEIG to 10*NUMEIG
PROPERTY specTol
	TYPE real
	DEPENDS_ON SCH.activate
	DEFAULT 1e-4
	DESCRIPTION (Solver parameters) Defines the convergence tolerance
PROPERTY mat_ml_3
	TYPE real
	DEPENDS_ON SCH.activate
	DEFAULT 0.916d0
	DESCRIPTION Defines the relative mass in the 3rd longitudional valley
PROPERTY mat_mt_3
	TYPE real
	DEPENDS_ON SCH.activate
	DEFAULT 0.19d0
	DESCRIPTION Defines the relative mass in the 3rd transversal valley
PROPERTY mat_ml_4
	TYPE real
	DEPENDS_ON SCH.activate
	DEFAULT 0.916d0
	DESCRIPTION Defines the relative mass in the 4rd longitudional valley
PROPERTY mat_mt_4
	TYPE real
	DEPENDS_ON SCH.activate
	DEFAULT 0.19d0
	DESCRIPTION Defines the relative mass in the 4rd transversal valley
PROPERTY mat_ml_5
	TYPE real
	DEPENDS_ON SCH.activate
	DEFAULT 0.916d0
	DESCRIPTION Defines the relative mass in the 5rd longitudional valley
PROPERTY mat_mt_5
	TYPE real
	DEPENDS_ON SCH.activate
	DEFAULT 0.19d0
	DESCRIPTION Defines the relative mass in the 5rd transversal valley


SECTION Mobility
	DESCRIPTION Control the mobility of electrons
PROPERTY activate
	TYPE boolean
	DEFAULT yes
	DESCRIPTION Uses the mobility model set for SI MOSFET devices (NOT FOR InGaAs)
PROPERTY perpendicular
	TYPE boolean
	DEFAULT no
	DESCRIPTION Activate the mobility correction by the perpendicular electric field
PROPERTY ECN
	TYPE real
	DEPENDS_ON Mobility.perpendicular
	DEFAULT 0.0
	DESCRIPTION Normalized electric normal field
PROPERTY lateral
	TYPE boolean
	DEFAULT no
	DESCRIPTION Activate the mobility correction by the lateral electric field
PROPERTY Vsat
	TYPE real
	DEPENDS_ON Mobility.lateral
	DEFAULT 0.0
	DESCRIPTION Saturation velocity
PROPERTY beta
	TYPE real
	DEPENDS_ON Mobility.lateral
	DEFAULT 0.0
	DESCRIPTION Beta value for the mobility


SECTION Mobility_Conc_Temp
	DESCRIPTION Concentration Temperature Analytical Model for electron mobility
PROPERTY mun_min
	TYPE real
	DEFAULT 55.24
	DESCRIPTION Parameter for Analytical Concentration-temperature Model
PROPERTY mun_max
	TYPE real
	DEFAULT 1429.23
	DESCRIPTION Parameter for Analytical Concentration-temperature Model
PROPERTY nrefn
	TYPE real
	DEFAULT 1.072e17
	DESCRIPTION Parameter for Analytical Concentration-temperature Model
PROPERTY nun
	TYPE real
	DEFAULT -2.3
	DESCRIPTION Parameter for Analytical Concentration-temperature Model
PROPERTY xin
	TYPE real
	DEFAULT -3.8
	DESCRIPTION Parameter for Analytical Concentration-temperature Model
PROPERTY alphan
	TYPE real
	DEFAULT 0.73
	DESCRIPTION Parameter for Analytical Concentration-temperature Model
PROPERTY Temperature
	TYPE real
	DEFAULT 300.0
	DESCRIPTION Parameter for Analytical Concentration-temperature Model



SECTION OTV
	DESCRIPTION Oxide Thickness Variability with roughness profile
PROPERTY activate
	TYPE boolean
	DEFAULT no
	DESCRIPTION Activate the OTV study, needs a roughness profile
PROPERTY file
	DEPENDS_ON OTV.activate
	DESCRIPTION Input file with the roughness profile to apply to the oxide
PROPERTY method
	DEPENDS_ON OTV.activate
	VALUES profile,homogeneous,permitivity
	DESCRIPTION Profile applies the deformation as in the file. Homogeneous
	DESCRIPTION deformates the device to be homogeneous and equivalent to
	DESCRIPTION the profile, in average. The permitivity solution tries to
	DESCRIPTION modify the permitivity of the oxide to have the same
	DESCRIPTION effect on the device.

SECTION WF
	DESCRIPTION Work function variability with Voronoi patches
PROPERTY activate
	TYPE boolean
	DEFAULT no
	DESCRIPTION Activate the work function variability, ignore previous Device.WF value
PROPERTY file
	DEPENDS_ON WF.activate
	DESCRIPTION Input file with the actual patches

SECTION LER
	DESCRIPTION Line Edge Roughness variability
PROPERTY activate
	TYPE boolean
	DEFAULT no
	DESCRIPTION Activate the line edge roughness variability
PROPERTY axis_length
	DEPENDS_ON LER.activate
	TYPE integer
	VALUES 0,1,2
	DESCRIPTION Direction of advante of the LER (x,y,z) = (0,1,2)
PROPERTY axis_width
	DEPENDS_ON LER.activate
	TYPE integer
	VALUES 0,1,2
	DESCRIPTION Direction of deformation of the LER (x,y,z) = (0,1,2)
PROPERTY file
	DEPENDS_ON LER.activate
	DESCRIPTION Input file with the actual deformation profile
PROPERTY material_tag
	TYPE integer
	DEPENDS_ON LER.activate
	DEFAULT -6
	DESCRIPTION Value of i_frontera that will be deformed


SECTION GER
	DESCRIPTION Gate Edge Roughness variability
PROPERTY activate
	TYPE boolean
	DEFAULT no
	DESCRIPTION Activate the Gate edge roughness variability
PROPERTY file
	DEPENDS_ON GER.activate
	DESCRIPTION Input file with the actual deformation profile


SECTION RD
	DESCRIPTION Random dopants fluctuations
PROPERTY activate
	TYPE boolean
	DEFAULT no
	DESCRIPTION Activate the random dopants fluctuations
PROPERTY damping_factor
	DEPENDS_ON RD.activate
	TYPE real
	DESCRIPTION Damping factor to be used with quantum corrections, 1.0 if none
PROPERTY damping_steps
	DEPENDS_ON RD.activate
	TYPE integer
	DESCRIPTION Steps to be simulated to get the original doping back after aplying damping
#PROPERTY backdoping
#	DEPENDS_ON RD.activate
#	TYPE real
#	DESCRIPTION Doping to apply on all nodes

PROPERTY mapping
	DEPENDS_ON RD.activate
	TYPE string
	VALUES cic, ngp
	DESCRIPTION Mapping used to share the charge between adjacent nodes, cloud-in-cell or nearest-grid-point.
PROPERTY sections
	DEPENDS_ON RD.activate
	TYPE subsection
	POINTS_TO Dopingprofile
	DESCRIPTION Each section that define a given doping profile is listed here

SUBSECTION Dopingprofile
	DESCRIPTION Section that defines a doping profile
PROPERTY material_tag
	TYPE integer
	DESCRIPTION Value of i_frontera in which we want to apply the doping
PROPERTY doping_type
	VALUES N,P
	DESCRIPTION Type of doping to be applied
PROPERTY source
	VALUES file,seed
	DESCRIPTION Source of the random dopants: from a file or generated randomly
PROPERTY seed
	TYPE integer
	DESCRIPTION Seed to feed the random number generator
PROPERTY constante_red
	TYPE real
	DESCRIPTION Lattice constant, set this as appropiate for the material being simulated
PROPERTY maxdoping
	TYPE real
	DESCRIPTION Maximum value of doping for a node, will be truncated there
PROPERTY mindoping
	TYPE real
	DESCRIPTION Minimum value of doping for a node, will be truncated there
PROPERTY contacts_cut
	TYPE real
	DESCRIPTION Nanometers where there is no RD applied from the contacts

