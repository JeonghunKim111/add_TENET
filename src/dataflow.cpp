#include "dataflow.h"

using namespace std;
using namespace TENET;

Dataflow::Dataflow(Statement &&st, PEArray &&pe, Mapping &&mp):
	_st(move(st)),
	_pe(move(pe)),
	_mp(move(mp))
{}

isl_union_map*
Dataflow::GetSpaceMap()
{
	isl_union_map *space_map = _mp.GetSpaceMap();
	space_map = isl_union_map_intersect_domain(space_map, _st.GetDomain());
	return space_map;
}

isl_union_map*
Dataflow::GetTimeMap()
{
	isl_union_map *time_map = _mp.GetTimeMap();
	time_map = isl_union_map_intersect_domain(time_map, _st.GetDomain());
	return time_map;
}

isl_union_map*
Dataflow::GetSpaceTimeMap()
{
	isl_union_map *space_time_map = _mp.GetSpaceTimeMap();
	space_time_map = isl_union_map_intersect_domain(space_time_map, _st.GetDomain());
	return space_time_map;
}

isl_union_set*
Dataflow::GetDomain()
{
	return _st.GetDomain();
}

isl_union_map*
Dataflow::GetAccess(
	string tensor_name,
	AccessType type)
{
	return _st.GetAccess(tensor_name, type);
}

isl_union_set*
Dataflow::GetSpaceDomain()
{
	return isl_union_set_apply(this->GetDomain(), this->GetSpaceMap());
}

isl_union_set*
Dataflow::GetTimeDomain()
{
	return isl_union_set_apply(this->GetDomain(), this->GetTimeMap());
}

isl_union_set*
Dataflow::GetSpaceTimeDomain()
{
	return isl_union_set_apply(this->GetDomain(), this->GetSpaceTimeMap());
}

isl_union_map*
Dataflow::MapTimeToPrev(unsigned distance, bool is_range)
{
	if (is_range == false) // querying the exact distance
	{
		if (distance == 0)
			return isl_union_set_identity(this->GetTimeDomain());
		else
		{
			isl_union_map *outer = this->MapTimeToPrev(distance, true);
			isl_union_map *inner = this->MapTimeToPrev(distance - 1, true);
			return isl_union_map_subtract(outer, inner);
		}
	}
	else  // querying time within a given distance
	{
		isl_union_map * ret = isl_union_set_identity(GetTimeDomain());
		isl_union_map * neighbor = isl_union_set_lex_gt_union_set(GetTimeDomain(),
			GetTimeDomain());
		neighbor = isl_union_map_lexmax(neighbor);
		neighbor = isl_union_map_union(neighbor, isl_union_map_copy(ret));
		for (unsigned i = 0; i < distance; i++)
			ret = isl_union_map_apply_range(ret, isl_union_map_copy(neighbor));
		isl_union_map_free(neighbor);
		return ret;
	}
}

isl_union_map*
Dataflow::MapSpaceToNeighbor(unsigned distance, bool is_range)
{
	if (is_range == false) // querying the exact distance
	{
		if (distance == 0)
			return isl_union_set_identity(GetSpaceDomain());
		else
		{
			isl_union_map *outer = this->MapSpaceToNeighbor(distance, true);
			isl_union_map *inner = this->MapSpaceToNeighbor(distance - 1, true);
			return isl_union_map_subtract(outer, inner);
		}
	}
	else  // querying space within a given distance
	{
		isl_union_map *ret = isl_union_set_identity(GetSpaceDomain());
		isl_union_map *neighbor = _pe.GetInterconnect();
		neighbor = isl_union_map_union(neighbor, isl_union_set_identity(_pe.GetDomain()));
		for (unsigned i = 0; i < distance; i++)
			ret = isl_union_map_apply_range(ret, isl_union_map_copy(neighbor));
		isl_union_map_free(neighbor);
		return ret;
	}
}

/*
	This function is used by many dataflow analysis methods to map a point representing
	space-time to its neighbor. Space_distance and time_distance represent how many steps
	should be counted.space_is_range and time_is_range represent whether we want neighbors
	within a certain distance or neighbors with exact some distance. include_self represent
	whether the point itself should be counted into neighbors. A usual setting is let
	space_distance=time_distance=1, space_is_range=time_is_range=true, include_self=false.
	Which count all points within one cycle and within 1 distance in PE array, not
	including the point itself.
 */
isl_union_map*
Dataflow::MapSpaceTimeToNeighbor(
	unsigned space_distance,
	bool space_is_range,
	unsigned time_distance,
	bool time_is_range,
	bool include_self)
{
	isl_union_map *space_to_neighbor = MapSpaceToNeighbor(space_distance, space_is_range);
	isl_union_map *time_to_neighbor = MapTimeToPrev(time_distance, time_is_range);
	isl_union_map *space_time_to_neighbor = isl_union_map_product(space_to_neighbor, time_to_neighbor);
	if (include_self == false)
	{
		isl_union_map *space_time_identity = isl_union_set_identity(GetSpaceTimeDomain());
		space_time_to_neighbor = isl_union_map_subtract(space_time_to_neighbor, space_time_identity);
	}
	return space_time_to_neighbor;
}

isl_union_map *
Dataflow::MapSpaceTimeToAccess(string tensor_name, AccessType type)
{
	isl_union_map *space_time_to_domain = isl_union_map_reverse(GetSpaceTimeMap());
	isl_union_map *access = _st.GetAccess(tensor_name, type);
	return isl_union_map_apply_range(space_time_to_domain, access);
}
/*
* GetUniqueVolume: the size of data required that cannot be find from
* neighbor in space-time domain
*/
double
Dataflow::GetUniqueVolume(
	string tensor_name,
	AccessType type,
	isl_union_map *space_time_to_neighbor)
{
	isl_union_map *access = MapSpaceTimeToAccess(tensor_name, type);
	isl_union_map *neighbor_access = isl_union_map_apply_range(space_time_to_neighbor,
		isl_union_map_copy(access));
	isl_union_map *unique_access = isl_union_map_subtract(access, neighbor_access);
	isl_union_pw_qpolynomial *unique_access_num = isl_union_map_card(unique_access);
#ifdef DEBUG
	fprintf(stdout,"Unique Access Num for %s:\n",tensor_name.c_str());
	p = isl_printer_print_union_pw_qpolynomial(p, unique_access_num);
	p = isl_printer_end_line(p);
#endif
	unique_access_num = isl_union_pw_qpolynomial_sum(unique_access_num); // sum on time
	unique_access_num = isl_union_pw_qpolynomial_sum(unique_access_num); // sum on space

	return convert_upwqp_to_int(unique_access_num);
}
/*
* GetTotalVolume: the size of data required in total when no data reuse
* is considered
*/
double
Dataflow::GetTotalVolume(string tensor_name, AccessType type)
{
	isl_union_map *access = GetAccess(tensor_name, type);
	isl_union_pw_qpolynomial *access_num = isl_union_map_card(access);
	access_num = isl_union_pw_qpolynomial_sum(access_num);
	return convert_upwqp_to_int(access_num);
}

double
Dataflow::GetTemporalReuseVolume(string tensor_name, AccessType type)
{
	isl_union_map *stt_prev = MapSpaceTimeToNeighbor(0, false, 1, false, false);
	double total_volume = GetTotalVolume(tensor_name, type);
	double unique_volume = GetUniqueVolume(tensor_name, type, stt_prev);
	double dsize = GetDomainSize();
	return (double)(total_volume - unique_volume) / dsize;
}

double
Dataflow::GetSpatialReuseVolume(string tensor_name, AccessType type, isl_union_map *stt_neighbor)
{
	if (stt_neighbor == NULL)
		stt_neighbor = MapSpaceTimeToNeighbor(1, false, 1, true, false);
	isl_union_map *stt_access = MapSpaceTimeToAccess(tensor_name, type);
	isl_union_map *neighbor_access = isl_union_map_apply_range(isl_union_map_copy(stt_neighbor), isl_union_map_copy(stt_access));
	isl_union_map *spatial_reuse = isl_union_map_intersect(stt_access, neighbor_access);
	isl_union_pw_qpolynomial *spatial_reuse_num = isl_union_map_card(spatial_reuse);

	spatial_reuse_num = isl_union_pw_qpolynomial_sum(spatial_reuse_num); // sum on time
	spatial_reuse_num = isl_union_pw_qpolynomial_sum(spatial_reuse_num); // sum on space

	double number = convert_upwqp_to_int(spatial_reuse_num);
	double dsize = GetDomainSize();
	double res = number / dsize;
	return res;
}
/*
* GetReuseFactor: calculate reuse factor by TotalVolume/UniqueVolume
*/
double
Dataflow::GetReuseFactor(string tensor_name, AccessType type,
	isl_union_map* space_time_to_neighbor)
{
	double unique_volume = GetUniqueVolume(tensor_name, type, space_time_to_neighbor);
	double total_volume = GetTotalVolume(tensor_name, type);
	double reuse_factor = total_volume / unique_volume;
	return reuse_factor;
}

// this function is used to convert a union piecewise quasi-polynomial function that
// HAVE A EMPTY DOMAIN (that is, only have a value) to int 
// upwqp is freed by this function.
double
Dataflow::convert_upwqp_to_int(isl_union_pw_qpolynomial *upwqp)
{
	isl_printer *p = isl_printer_to_str(isl_union_pw_qpolynomial_get_ctx(upwqp));
	p = isl_printer_set_output_format(p, ISL_FORMAT_ISL);
	p = isl_printer_print_union_pw_qpolynomial(p, upwqp);
	char *s = isl_printer_get_str(p);
	double ret = atoi(s + 1);
	isl_union_pw_qpolynomial_free(upwqp);
	isl_printer_free(p);
	return ret;
}
double
Dataflow::GetDomainSize()
{
	isl_union_set *domain = _st.GetDomain();
	isl_union_pw_qpolynomial* domain_size = isl_union_set_card(domain);
	double dsize = convert_upwqp_to_int(domain_size);
	return dsize;
}
/* Calculate the number of MACs by calculating number of instances* MACs 
 * per instance
*/
double
Dataflow::GetMacNum(int mac_per_instance)
{
	double dsize = GetDomainSize();
	return dsize * mac_per_instance;
}

double
Dataflow::GetTotalTime()
{
	isl_union_set *time_domain = GetTimeDomain();
	isl_union_pw_qpolynomial* domain_size = isl_union_set_card(time_domain);
	double time_elapse = convert_upwqp_to_int(domain_size);
	return time_elapse;
}
double
Dataflow::GetPENum()
{
	isl_union_set *space_domain = GetSpaceDomain();
	isl_union_pw_qpolynomial* domain_size = isl_union_set_card(space_domain);
	double dsize = convert_upwqp_to_int(domain_size);
	return dsize;
}
double
Dataflow::GetMacNumPerPE(int mac_per_instance)
{
	double mac_num = GetMacNum(mac_per_instance);
	// use GetSpaceDomain instead of pe.Getdomain() here in case some pes
	// are idle
	double dsize = GetPENum();
	return mac_num / dsize;
}

double
Dataflow::GetActivePENum()
{
	isl_union_set *space_domain = GetSpaceDomain();
	isl_union_pw_qpolynomial* domain_size = isl_union_set_card(space_domain);
	double dsize = convert_upwqp_to_int(domain_size);
	return dsize;
}

/* return the average active PE num over time-domain*/
double
Dataflow::GetAverageActivePENum()
{
	isl_union_set *space_time_domain = GetSpaceTimeDomain();
	isl_union_pw_qpolynomial* space_time_domain_size = isl_union_set_card(space_time_domain);
	double stsize = convert_upwqp_to_int(space_time_domain_size);
	isl_union_set *time_domain = GetTimeDomain();
	isl_union_pw_qpolynomial* time_domain_size = isl_union_set_card(time_domain);
	double tsize = convert_upwqp_to_int(time_domain_size);
	double avg_active_pe = (double)stsize / tsize;
	return avg_active_pe;
}

double
Dataflow::GetIngressDelay(isl_union_map* space_time_to_neighbor, string tensor_name)
{
	long long ingress_volume =
		GetUniqueVolume(tensor_name, AccessType::READ, space_time_to_neighbor)*BIT_PER_ITEM;
	return ingress_volume / _pe.GetBandwidth() + _pe.GetAvgLatency() - 1;
}

double
Dataflow::GetEgressDelay(isl_union_map* space_time_to_neighbor, string tensor_name)
{
	long long egress_volume =
		GetUniqueVolume(tensor_name, AccessType::WRITE, space_time_to_neighbor)*BIT_PER_ITEM;
	return egress_volume / _pe.GetBandwidth() + _pe.GetAvgLatency() - 1;
}

double
Dataflow::GetComputationDelay()
{
	return GetMacNumPerPE();
}

double
Dataflow::GetDelay(isl_union_map* space_time_to_neighbor)
{
	// int max_delay = 0;
	double ingress_delay = GetIngressDelay(isl_union_map_copy(space_time_to_neighbor));
	double egress_delay = GetEgressDelay(space_time_to_neighbor);
	double compute_delay = GetComputationDelay();

	return max(max(ingress_delay, egress_delay), compute_delay);
}

double
Dataflow::GetL1Read(string tensor_name, AccessType type)
{
	if (type == AccessType::READ || type == AccessType::WRITE)
		return GetTotalVolume(tensor_name, type);
	else
		// for tensor that both be input and output, one L1 read for L1->PE and one for L1->L2
		return GetTotalVolume(tensor_name, AccessType::READ) + GetTotalVolume(tensor_name, AccessType::WRITE);
}

double
Dataflow::GetL1Write(string tensor_name, AccessType type)
{
	if (type == AccessType::READ || type == AccessType::WRITE)
		return GetTotalVolume(tensor_name, type);
	else
		// for tensor that both be input and output, one L1 write for L2->L1 and one for PE->L1
		return GetTotalVolume(tensor_name, AccessType::READ) + GetTotalVolume(tensor_name, AccessType::WRITE);
}

double
Dataflow::GetL2Read(
	string tensor_name,
	AccessType type,
	isl_union_map* space_time_to_neighbor)
{
	if (type == AccessType::READ || type == AccessType::WRITE)
		// reuse can be exploited  to reduce L2 read
		return GetUniqueVolume(tensor_name, type, space_time_to_neighbor);
	else
		// for tensor that both be input and output, one L2 read for L2->L1 and one for L2->DRAM
		return GetUniqueVolume( tensor_name, AccessType::READ,
					isl_union_map_copy(space_time_to_neighbor)
				) + GetUniqueVolume(tensor_name, AccessType::WRITE, space_time_to_neighbor);
}

double
Dataflow::GetL2Write(
	string tensor_name,
	AccessType type,
	isl_union_map* space_time_to_neighbor)
{
	if (type == AccessType::READ || type == AccessType::WRITE)
		// reuse can be exploited to reduce L2 write
		return GetUniqueVolume(tensor_name, type, space_time_to_neighbor);
	else
		// for tensor that both be input and output, one L2 write for DRAM->L2 and one for L1->L2
		return GetUniqueVolume(tensor_name, AccessType::READ, isl_union_map_copy(space_time_to_neighbor)) +
		GetUniqueVolume(tensor_name, AccessType::WRITE, space_time_to_neighbor);
}

double
Dataflow::GetEnergy(isl_union_map* space_time_to_neighbor)
{
	double energy = GetMacNum();  // energy cost of MAC
	auto [input, output] = _st.GetTensorList();
	for (auto& iter : input)
	{
		energy += l1_multiplier * GetL1Read(iter, AccessType::READ);
		energy += l1_multiplier * GetL1Write(iter, AccessType::READ);
		energy += l2_multiplier * GetL2Read(iter, AccessType::READ,
			isl_union_map_copy(space_time_to_neighbor));
		energy += l2_multiplier * GetL2Write(iter, AccessType::READ,
			isl_union_map_copy(space_time_to_neighbor));
	}
	for (auto& iter:output)
	{
		energy += l1_multiplier * GetL1Read(iter, AccessType::WRITE);
		energy += l1_multiplier * GetL1Write(iter, AccessType::WRITE);
		energy += l2_multiplier * GetL2Read(iter, AccessType::WRITE,
			isl_union_map_copy(space_time_to_neighbor));
		energy += l2_multiplier * GetL2Write(iter, AccessType::WRITE,
			isl_union_map_copy(space_time_to_neighbor));
	}
	isl_union_map_free(space_time_to_neighbor);
	return energy;
}

Dataflow
Dataflow::copy() const
{
	return Dataflow(_st.copy(), _pe.copy(), _mp.copy());
}
