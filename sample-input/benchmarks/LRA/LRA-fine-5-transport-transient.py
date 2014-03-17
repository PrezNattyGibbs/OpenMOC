from openmoc import *
import openmoc.log as log
import openmoc.plotter as plotter
import openmoc.materialize as materialize
import time

###############################################################################
#######################   Main Simulation Parameters   ########################
###############################################################################

num_threads = options.num_omp_threads
track_spacing = options.track_spacing
num_azim = options.num_azim
tolerance = options.tolerance
max_iters = options.max_iters
relax_factor = options.relax_factor
acceleration = options.acceleration
mesh_level = options.mesh_level
dt_cmfd = 1.e-4
dt_moc  = 1.e-2
log.setLogLevel('NORMAL')

###############################################################################
###########################   Creating Materials   ############################
###############################################################################

log.py_printf('NORMAL', 'Importing materials data from py...')

materials = materialize.materialize('LRA-materials-transient.py')

region1 = materials['region_1'].getId()
region2 = materials['region_2'].getId()
region3 = materials['region_3'].getId()
region4 = materials['region_4'].getId()
region5 = materials['region_5'].getId()
region6 = materials['region_6'].getId()


###############################################################################
###########################   Creating Surfaces   #############################
###############################################################################

log.py_printf('NORMAL', 'Creating surfaces...')

planes = []
planes.append(XPlane(x=-82.5))
planes.append(XPlane(x=82.5))
planes.append(YPlane(y=-82.5))
planes.append(YPlane(y=82.5))
planes[0].setBoundaryType(REFLECTIVE)
planes[1].setBoundaryType(VACUUM)
planes[2].setBoundaryType(REFLECTIVE)
planes[3].setBoundaryType(VACUUM)


###############################################################################
#############################   Creating Cells   ##############################
###############################################################################

log.py_printf('NORMAL', 'Creating cells...')

cells = []
cells.append(CellBasic(universe=1, material=region1))
cells.append(CellBasic(universe=2, material=region2))
cells.append(CellBasic(universe=3, material=region3))
cells.append(CellBasic(universe=4, material=region4))
cells.append(CellBasic(universe=5, material=region5))
cells.append(CellBasic(universe=6, material=region6))
cells.append(CellFill(universe=21, universe_fill=31))
cells.append(CellFill(universe=22, universe_fill=32))
cells.append(CellFill(universe=23, universe_fill=33))
cells.append(CellFill(universe=24, universe_fill=34))
cells.append(CellFill(universe=25, universe_fill=35))
cells.append(CellFill(universe=26, universe_fill=36))
cells.append(CellFill(universe=0, universe_fill=7))

cells[12].addSurface(halfspace=+1, surface=planes[0])
cells[12].addSurface(halfspace=-1, surface=planes[1])
cells[12].addSurface(halfspace=+1, surface=planes[2])
cells[12].addSurface(halfspace=-1, surface=planes[3])


###############################################################################
###########################   Creating Lattices   #############################
###############################################################################

log.py_printf('NORMAL', 'Creating LRA lattice...')

assembly1 = Lattice(id=31, width_x=3.0, width_y=3.0)
assembly1.setLatticeCells([[1, 1, 1, 1, 1],
                           [1, 1, 1, 1, 1],
                           [1, 1, 1, 1, 1],
                           [1, 1, 1, 1, 1],
                           [1, 1, 1, 1, 1]])

assembly2 = Lattice(id=32, width_x=3.0, width_y=3.0)
assembly2.setLatticeCells([[2, 2, 2, 2, 2],
                           [2, 2, 2, 2, 2],
                           [2, 2, 2, 2, 2],
                           [2, 2, 2, 2, 2],
                           [2, 2, 2, 2, 2]])


assembly3 = Lattice(id=33, width_x=3.0, width_y=3.0)
assembly3.setLatticeCells([[3, 3, 3, 3, 3],
                           [3, 3, 3, 3, 3],
                           [3, 3, 3, 3, 3],
                           [3, 3, 3, 3, 3],
                           [3, 3, 3, 3, 3]])


assembly4 = Lattice(id=34, width_x=3.0, width_y=3.0)
assembly4.setLatticeCells([[4, 4, 4, 4, 4],
                           [4, 4, 4, 4, 4],
                           [4, 4, 4, 4, 4],
                           [4, 4, 4, 4, 4],
                           [4, 4, 4, 4, 4]])


assembly5 = Lattice(id=35, width_x=3.0, width_y=3.0)
assembly5.setLatticeCells([[5, 5, 5, 5, 5],
                           [5, 5, 5, 5, 5],
                           [5, 5, 5, 5, 5],
                           [5, 5, 5, 5, 5],
                           [5, 5, 5, 5, 5]])


assembly6 = Lattice(id=36, width_x=3.0, width_y=3.0)
assembly6.setLatticeCells([[6, 6, 6, 6, 6],
                           [6, 6, 6, 6, 6],
                           [6, 6, 6, 6, 6],
                           [6, 6, 6, 6, 6],
                           [6, 6, 6, 6, 6]])


core = Lattice(id=7, width_x=15.0, width_y=15.0)
core.setLatticeCells([[26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26],
                         [26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26],
                         [23, 23, 23, 23, 23, 23, 23, 26, 26, 26, 26],
                         [23, 23, 23, 23, 23, 23, 23, 24, 26, 26, 26],
                         [22, 21, 21, 21, 21, 22, 22, 25, 25, 26, 26],
                         [22, 21, 21, 21, 21, 22, 22, 25, 25, 26, 26],
                         [21, 21, 21, 21, 21, 21, 21, 23, 23, 26, 26],
                         [21, 21, 21, 21, 21, 21, 21, 23, 23, 26, 26],
                         [21, 21, 21, 21, 21, 21, 21, 23, 23, 26, 26],
                         [21, 21, 21, 21, 21, 21, 21, 23, 23, 26, 26],
                         [22, 21, 21, 21, 21, 22, 22, 23, 23, 26, 26]])


###############################################################################
###########################   Creating Cmfd Mesh   #############################
###############################################################################

log.py_printf('NORMAL', 'Creating cmfd mesh...')

mesh = Mesh(MOC, acceleration, relax_factor, mesh_level)


###############################################################################
##########################   Creating the Geometry   ##########################
###############################################################################

log.py_printf('NORMAL', 'Creating geometry...')

geometry = Geometry(mesh)
for material in materials.values(): geometry.addMaterial(material)
for cell in cells: geometry.addCell(cell)
geometry.addLattice(assembly1)
geometry.addLattice(assembly2)
geometry.addLattice(assembly3)
geometry.addLattice(assembly4)
geometry.addLattice(assembly5)
geometry.addLattice(assembly6)
geometry.addLattice(core)

geometry.initializeFlatSourceRegions()

###############################################################################
########################   Creating the TrackGenerator   ######################
###############################################################################

log.py_printf('NORMAL', 'Initializing the track generator...')

track_generator = TrackGenerator(geometry, num_azim, track_spacing)
track_generator.generateTracks()

###############################################################################
########################   Creating the Cmfd module   #########################
###############################################################################

log.py_printf('NORMAL', 'Creating cmfd...')

cmfd = Cmfd(geometry, 1e-9)
cmfd.setOmega(1.5)

###############################################################################
###########################   Running a Simulation   ##########################
###############################################################################

log.py_printf('NORMAL', 'Creating transient solver...')

solver = ThreadPrivateSolverTransient(geometry, track_generator, cmfd)
solver.setNumThreads(num_threads)
solver.setSourceConvergenceThreshold(tolerance)
solver.initialize()

tcmfd = Tcmfd(geometry, 1e-9)
tcmfd.setOmega(1.5)
tcmfd.setLambda([0.0654, 1.35])
tcmfd.setBeta([0.0054, 0.001087])
tcmfd.setVelocity([3e7, 3e5])

transientSolver = TransientSolver(geometry, tcmfd, cmfd, solver)
transientSolver.setKappa(3.204e-11)
transientSolver.setAlpha(3.83e-11)
transientSolver.setNu(2.43)
transientSolver.setDtMOC(dt_moc)
transientSolver.setDtCMFD(dt_cmfd)
transientSolver.setStartTime(0.0)
transientSolver.setEndTime(3.0)
transientSolver.setNumDelayGroups(2)
transientSolver.setTransientMethod('MAF')
transientSolver.setPowerInit(1.e-6)

#plotter.plotFlatSourceRegions(geometry, gridsize=500)
#plotter.plotTemperature(geometry, gridsize=500)
transientSolver.solveInitialState()

#plotter.plotSigmaA(geometry, 0)
#plotter.plotSigmaA(geometry, 1)

for t in range(int(3.0/dt_moc)):
   transientSolver.solveOuterStep()

   #if (abs(t*dt_moc - 1.72) < 1.e-6):
   #   plotter.plotTemperature(geometry, gridsize=500)
   #elif (abs(t*dt_moc - 0.1) < 1.e-6):
   #   plotter.plotTemperature(geometry, gridsize=500)
   #elif (abs(t*dt_moc - 1.0) < 1.e-6):
   #   plotter.plotTemperature(geometry, gridsize=500)

    #  plotter.plotSigmaA(geometry, 0)
     # plotter.plotSigmaA(geometry, 1)

###############################################################################
############################   Generating Plots   #############################
###############################################################################

log.py_printf('NORMAL', 'Plotting data...')

#plotter.plotMaterials(geometry, gridsize=500)
#plotter.plotCells(geometry, gridsize=500)

#plotter.plotFluxes(geometry, solver, energy_groups=[1,2])
#plotter.plotMeshFluxes(mesh, energy_groups=[1,2])

log.py_printf('TITLE', 'Finished')

