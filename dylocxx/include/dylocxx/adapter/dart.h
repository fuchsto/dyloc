#ifndef DYLOCXX__ADAPTER__DART_H__INCLUDED
#define DYLOCXX__ADAPTER__DART_H__INCLUDED

#include <dash/dart/if/dart_types.h>
#include <dash/dart/if/dart_team_group.h>


namespace dyloc {

dart_global_unit_t dart_myid() {
  dart_global_unit_t myid;
  dart_myid(&myid);
  return myid;
}

dart_team_unit_t dart_myid(dart_team_t t) {
  dart_team_unit_t myid;
  dart_team_myid(t, &myid);
  return myid;
}

} // namespace dyloc

#endif // DYLOCXX__ADAPTER__DART_H__INCLUDED
