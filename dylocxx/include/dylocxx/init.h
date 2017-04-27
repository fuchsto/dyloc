#ifndef DYLOCXX__INIT_H__INCLUDED
#define DYLOCXX__INIT_H__INCLUDED

#include <dyloc/common/types.h>


namespace dyloc {

void init(int * argc, char *** argv);
void finalize();
bool is_initialized();

} // namespace dyloc

#endif // DYLOCXX__INIT_H__INCLUDED
