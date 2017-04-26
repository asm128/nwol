#include "nwol_array.h"
#include "label.h"

struct SSpecies {
	uint32_t							ParentMale;
	uint32_t							ParentFemale;
	::nwol::glabel						Name;
};

struct SDiseases {
	uint32_t							ParentMale;
	uint32_t							ParentFemale;
	::nwol::glabel						Name;
};

struct SPlant {
	uint32_t							Species;
	int64_t								DateBirth;
	int64_t								DateTransplant;
	int64_t								DateFloracion;
	int64_t								TemperatureBirth;
	int64_t								TemperatureTransplant;
	int64_t								TemperatureFloracion;
	int64_t								HumidityBirth;
	int64_t								HumidityTransplant;
	int64_t								HumidityFloracion;
	::nwol::glabel						Name;
};

struct SVivero {
	::nwol::array_obj<SSpecies>			Species;
	::nwol::array_obj<SPlant>			Plants;
	::nwol::array_obj<::nwol::glabel>	Diseases;
};
