#ifndef DYLOCXX__ADAPTER__DART_H__INCLUDED
#define DYLOCXX__ADAPTER__DART_H__INCLUDED

#include <dash/dart/if/dart_types.h>
#include <dash/dart/if/dart_team_group.h>


namespace dyloc {

static inline dart_global_unit_t myid() {
  dart_global_unit_t myid;
  dart_myid(&myid);
  return myid;
}

static inline dart_team_unit_t myid(dart_team_t t) {
  dart_team_unit_t myid;
  dart_team_myid(t, &myid);
  return myid;
}

static inline size_t num_units() {
  size_t nunits;
  ::dart_size(&nunits);
  return nunits;
}

static inline size_t num_units(dart_team_t t) {
  size_t nunits;
  ::dart_team_size(t, &nunits);
  return nunits;
}

} // namespace dyloc

#endif // DYLOCXX__ADAPTER__DART_H__INCLUDED
