#ifndef DYLOCXX__UNIT_LOCALITY_H__INCLUDED
#define DYLOCXX__UNIT_LOCALITY_H__INCLUDED

#include <dash/dart/if/dart_types.h>

#include <vector>


class unit_locality {
};

class unit_mapping {
  std::vector<unit_locality> _unit_localities;
  dart_team_t                _team;
};

#endif // DYLOCXX__UNIT_LOCALITY_H__INCLUDED
