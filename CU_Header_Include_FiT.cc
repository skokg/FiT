#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <sys/stat.h>
#include <sys/resource.h>
#include <random>

#include <gd.h>
#include <gdfontl.h>

#ifndef DISABLE_NETCDF4
	#include <netcdf.h>
	#include <hdf5.h>
#endif


#ifndef DISABLE_BOOST_SERIALIZATION
	#include <boost/serialization/list.hpp>
	#include <boost/serialization/string.hpp>
	#include <boost/serialization/vector.hpp>
	#include <boost/archive/binary_oarchive.hpp>
	#include <boost/archive/binary_iarchive.hpp>
	#include <boost/archive/text_oarchive.hpp>
	#include <boost/archive/text_iarchive.hpp>
	using namespace boost;
#endif

using namespace std;

// da prav dela error(..) - da prav displaya line number
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)
#define FU __PRETTY_FUNCTION__
//#define TT "\t"
#define tabt "\t"
#define ERRORIF(x) if (x) error(AT,FU, #x)

#define BAD_DATA_FLOAT -9999


const rlim_t kStackSize = 1000 * 1024 * 1024;   // min stack size = 16 MB

long random_number_seed=-1;


//string ramdisk="/mnt/ramdisk";

#include "CU_utils_subset.cc"
#include "CU_C3DSupport_code.cc"
#include "CU_C3DObject_classes.cc"
#include "CU_C3DNamelist_read.cc"
#include "CU_C3DField_class.cc"

#include "CU_Parameters_global.cc"
