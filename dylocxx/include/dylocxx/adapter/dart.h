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

static inline dart_global_unit_t l2g(dart_team_t t, dart_team_unit_t lid) {
  dart_global_unit_t gid;
  if (dart_team_unit_l2g(t, lid, &gid)) {
    return gid;
  }
  return DART_UNDEFINED_UNIT_ID;
}

static inline dart_team_unit_t g2l(dart_team_t t, dart_global_unit_t gid) {
  dart_team_unit_t lid;
  if (dart_team_unit_g2l(t, gid, &lid) == DART_OK) {
    return lid;
  }
  return DART_UNDEFINED_UNIT_ID;
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
