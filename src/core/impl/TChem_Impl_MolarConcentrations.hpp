/* =====================================================================================
TChem version 2.1.0
Copyright (2020) NTESS
https://github.com/sandialabs/TChem

Copyright 2020 National Technology & Engineering Solutions of Sandia, LLC (NTESS). 
Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains 
certain rights in this software.

This file is part of TChem. TChem is open-source software: you can redistribute it
and/or modify it under the terms of BSD 2-Clause License
(https://opensource.org/licenses/BSD-2-Clause). A copy of the license is also
provided under the main directory

Questions? Contact Cosmin Safta at <csafta@sandia.gov>, or
           Kyungjoo Kim at <kyukim@sandia.gov>, or
           Oscar Diaz-Ibarra at <odiazib@sandia.gov>

Sandia National Laboratories, Livermore, CA, USA
===================================================================================== */


#ifndef __TCHEM_IMPL_MOLARCONCENTRATIONS_HPP__
#define __TCHEM_IMPL_MOLARCONCENTRATIONS_HPP__

#include "TChem_Impl_RhoMixMs.hpp"
#include "TChem_Util.hpp"

namespace TChem {

namespace Impl {

/// Computes molar concentrations based on temperature and species mass
/// fractions (getMs2Cc) \f[ [X_k]=Y_k\cdot\rho/W_k \f]
///
struct MolarConcentrations
{
  template<typename MemberType,
           typename RealType1DViewType,
           typename KineticModelConstDataType>
  KOKKOS_INLINE_FUNCTION static void team_invoke(
    const MemberType& member,
    /// input
    const real_type& t,           /// temperature
    const real_type& p,           /// pressure
    const RealType1DViewType& Ys, /// mole fractions? mass fraction
    /// output
    const RealType1DViewType& concX,
    /// const input from kinetic model
    const KineticModelConstDataType& kmcd)
  {
    const real_type rhomix = kmcd.rho < real_type(0)
                               ? RhoMixMs::team_invoke(member, t, p, Ys, kmcd)
                               : kmcd.rho;
    Kokkos::parallel_for(Kokkos::TeamVectorRange(member, kmcd.nSpec),
                         [&](const ordinal_type& i) {
                           concX(i) = Ys(i) * rhomix / kmcd.sMass(i);
                         });
#if defined(TCHEM_ENABLE_SERIAL_TEST_OUTPUT) && !defined(__CUDA_ARCH__)
    if (member.league_rank() == 0) {
      FILE* fs = fopen("MolarConcentrations.team_invoke.test.out", "a+");
      fprintf(fs, ":: MolarConcentrations::team_invoke\n");
      fprintf(fs, ":::: input\n");
      fprintf(fs,
              "     nSpec %3d, t %e, p %e, kmcd.rho %e, rhomix %e\n",
              kmcd.nSpec,
              t,
              p,
              kmcd.rho,
              rhomix);
      for (int i = 0; i < kmcd.nSpec; ++i)
        fprintf(fs, "     i %3d, Ys %e, sMass %e\n", i, Ys(i), kmcd.sMass(i));
      fprintf(fs, ":::: output\n");
      for (int i = 0; i < kmcd.nSpec; ++i)
        fprintf(fs, "     i %3d, concX %e\n", i, concX(i));
    }
#endif
  }
};

} // namespace Impl
} // namespace TChem

#endif
