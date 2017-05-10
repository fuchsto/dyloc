#ifndef DYLOCXX__INIT_H__INCLUDED
#define DYLOCXX__INIT_H__INCLUDED

#include <dylocxx/topology.h>

#include <dyloc/common/types.h>


namespace dyloc {

void init(int * argc, char *** argv);
void finalize();
bool is_initialized();

const dyloc_unit_locality_t & query_unit_locality(
        dart_global_unit_t u);

const dyloc_unit_locality_t & query_unit_locality(
        dart_team_t t,
        dart_team_unit_t u);

topology & team_topology(
  dart_team_t t = DART_TEAM_ALL);

} // namespace dyloc

#endif // DYLOCXX__INIT_H__INCLUDED
