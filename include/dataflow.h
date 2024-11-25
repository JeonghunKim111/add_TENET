#pragma once
#include "stt.h"
#include "pe_array.h"
#include "statement.h"
#include "mapping.h"

namespace TENET{

class Dataflow
{
public:
	Dataflow(Statement &&st, PEArray &&pe, Mapping &&mp);

	isl_union_map *GetSpaceMap();
	isl_union_map *GetTimeMap();
	isl_union_map *GetSpaceTimeMap();
	isl_union_set *GetDomain();
	double GetDomainSize();
	isl_union_map *GetAccess(std::string tensor_name,
		AccessType type);
	isl_union_set *GetSpaceDomain();
	isl_union_set *GetTimeDomain();
	isl_union_set *GetSpaceTimeDomain();
	isl_union_map *MapTimeToPrev(unsigned distance, bool is_range);
	isl_union_map *MapSpaceToNeighbor(unsigned distance, bool is_range);
	isl_union_map * MapSpaceTimeToNeighbor(unsigned space_distance=1, 
		bool space_is_range=true, unsigned time_distance = 1, 
		bool time_is_range = true, bool include_self = false);
	isl_union_map * MapSpaceTimeToAccess(std::string tensor_name, AccessType type);
	double GetPENum();
	double GetTotalTime();
	// following are functions that perform dataflow analysis
	double GetUniqueVolume(std::string tensor_name, AccessType type,
		isl_union_map* space_time_to_neighbor);
	double GetTotalVolume(std::string tensor_name, AccessType type);
	double GetReuseFactor(std::string tensor_name, AccessType type,
		isl_union_map* space_time_to_neighbor);
	double GetTemporalReuseVolume(std::string tensor_name, AccessType type);
	double GetSpatialReuseVolume(std::string tensor_name, AccessType type, isl_union_map* stt_neighbor, bool is_total = true, int distance = 0);
	double GetMacNum(int mac_per_instance = 1);
	double GetMacNumPerPE(int mac_per_instance = 1);
	double GetActivePENum();
	double GetAverageActivePENum();
	double GetIngressDelay(isl_union_map* space_time_to_neighbor , std::string tensor_name = "");
	double GetEgressDelay(isl_union_map* space_time_to_neighbor, std::string tensor_name = "");
	double GetComputationDelay();
	double GetDelay(isl_union_map* space_time_to_neighbor);
	double GetL1Read(std::string tensor_name, AccessType type);
	double GetL1Write(std::string tensor_name, AccessType type);
	double GetL2Read(std::string tensor_name, AccessType type,
		isl_union_map* space_time_to_neighbor);
	double GetL2Write(std::string tensor_name, AccessType type,
		isl_union_map* space_time_to_neighbor);
	double GetEnergy(isl_union_map* space_time_to_neighbor);

	Dataflow copy() const;

private:
	Statement _st;
	PEArray _pe;
	Mapping _mp;

	double convert_upwqp_to_int(isl_union_pw_qpolynomial *upwqp);
};

} // namespace TENET
