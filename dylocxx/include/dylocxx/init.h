#ifndef DYLOCXX__INIT_H__INCLUDED
#define DYLOCXX__INIT_H__INCLUDED

#include <dylocxx/topology.h>

#include <dyloc/common/types.h>


namespace dyloc {

void init(int * argc, char *** argv);
void finalize();
bool is_initialized();

#if 0
const unit_locality & query_unit_locality(
        dart_global_unit_t u);

const unit_locality & query_unit_locality(
        dart_team_t t,
        dart_team_unit_t u);
#endif

topology & team_topology(
  dart_team_t t = DART_TEAM_ALL);

const dyloc::host_topology & team_host_topology(
  dart_team_t t = DART_TEAM_ALL);

} // namespace dyloc

#endif // DYLOCXX__INIT_H__INCLUDED
