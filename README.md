# dyloc - Dynamic Hardware Locality

## Synopsis

~~~
local hardware locality discovery (hwloc, PAPI, libnuma, ...)
-> node-level hardware topology (ltopo)

collective communication sub-system, based on DART
-> global locality domain graph (gtopo), based on Boost Graph Library

operations on gtopo (filter, partition, map, po-set operations, ...)
-> topology view (vtopo)

hwloc adapter for topology view
-> hwloc_topology_t
~~~


