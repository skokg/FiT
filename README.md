# The FiT Object Analysis and Tracking Package 

--- Description:

The FiT (Forward-in-Time) object analysis package can be used to identify and track objects in a sequence of 3D (or 2D) fields. The package is designed in a very general way - it does not care which variable is used for tracking (precipitation, vertical velocity, ...). The output from the package can be used to determine object statistics and other object-related properties such as location, size, lifespan, and trajectories. The package is written in C++ and includes MPI (Message Passing Interface) support. Its behavior is controlled by a namelist. 

For a more detailed description of how the software works and how to compile and use it, please refer to the README.dat file. The folder "sample_files" contains a sample dataset with a sample namelist, so once the software is successfully compiled, one can test if it works as expected. 

Do not hesitate to contact me if you have trouble compiling/using it or have any questions (my email is below).

--- Author:

Gregor Skok, Faculty of Mathematics and Physics, University of Ljubljana, Slovenia

Email: Gregor.Skok@fmf.uni-lj.si

--- References:

Skok, G., Bacmeister, J. T., Tribbia, J., 2013: Analysis of tropical cyclone precipitation using an object-based algorithm. Journal of climate, vol. 26, iss. 8, 2563-2579.

Skok, G., Tribbia, J., Rakovec, J., 2010: Object-based analysis and verification of WRF model precipitation in the low- and Midlatitude Pacific Ocean. Monthly weather review,  vol. 138, no. 12, 4561-4575

Skok, G., Tribbia, J., Rakovec, J., Brown, B., 2009: Object-based analysis of satellite-derived precipitation systems over the low-and midlatitude Pacific Ocean. Monthly weather review, vol. 137,3196-3218.
