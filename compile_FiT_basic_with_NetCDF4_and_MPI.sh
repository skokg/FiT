#!/bin/sh

mpicxx -O2 -Wall -Wno-unused-result -Wno-unknown-pragmas -o FiT_Object_analysis_basic_with_NetCDF4_and_MPI.exex CC_Object_analysis_FiT_basic_with_NetCDF4_and_MPI.cc -I/usr/include/hdf5/serial/ -lboost_serialization -lgd -lnetcdf 


