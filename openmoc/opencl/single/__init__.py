import openmoc
import _openmoc_opencl_single
from openmoc_opencl_single import *
import signal

# Tell Python to recognize CTRL+C and stop the C++ extension module
# when this is passed in from the keyboard
signal.signal(signal.SIGINT, signal.SIG_DFL)

Timer = openmoc.Timer
