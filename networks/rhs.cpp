#include <rhs.H>

#ifdef NEW_NETWORK_IMPLEMENTATION

AMREX_GPU_MANAGED Array3D<Real, 1, Rates::NumRates, 1, 2, 1, RHS::nrattab, Order::C> RHS::rattab;
AMREX_GPU_MANAGED Array3D<Real, 1, Rates::NumRates, 1, 2, 1, RHS::nrattab, Order::C> RHS::drattabdt;
AMREX_GPU_MANAGED Array1D<Real, 1, RHS::nrattab> RHS::ttab;

#endif
