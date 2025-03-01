/* =====================================================================================
TChem version 2.0
Copyright (2020) NTESS
https://github.com/sandialabs/TChem

Copyright 2020 National Technology & Engineering Solutions of Sandia, LLC (NTESS).
Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government retains
certain rights in this software.

This file is part of TChem. TChem is open source software: you can redistribute it
and/or modify it under the terms of BSD 2-Clause License
(https://opensource.org/licenses/BSD-2-Clause). A copy of the licese is also
provided under the main directory

Questions? Contact Cosmin Safta at <csafta@sandia.gov>, or
           Kyungjoo Kim at <kyukim@sandia.gov>, or
           Oscar Diaz-Ibarra at <odiazib@sandia.gov>

Sandia National Laboratories, Livermore, CA, USA
===================================================================================== */
#ifndef __TCHEM_IMPL_NUM_JACOBIAN_TCSTR_HPP__
#define __TCHEM_IMPL_NUM_JACOBIAN_TCSTR_HPP__

#include "TChem_Util.hpp"
#include "TChem_Impl_TransientContStirredTankReactor_Problem.hpp"

namespace TChem {
namespace Impl {

template<typename ValueType, typename DeviceType>
struct TransientContStirredTankReactorNumJacobian
{

  using value_type = ValueType;
  using device_type = DeviceType;
  using scalar_type = typename ats<value_type>::scalar_type;

  using real_type = scalar_type;
  using real_type_1d_view_type = Tines::value_type_1d_view<real_type,device_type>;
  using real_type_2d_view_type = Tines::value_type_2d_view<real_type,device_type>;

  /// sacado is value type
  using value_type_1d_view_type = Tines::value_type_1d_view<value_type,device_type>;
  using kinetic_model_type      = KineticModelConstData<device_type>;
  using kinetic_surf_model_type = KineticSurfModelConstData<device_type>;
  using cstr_data_type = TransientContStirredTankReactorData<device_type>;

  KOKKOS_INLINE_FUNCTION static ordinal_type getWorkSpaceSize(
    const kinetic_model_type& kmcd,
    const kinetic_surf_model_type& kmcdSurf)
  {
    using problem_type =
      TChem::Impl::TransientContStirredTankReactor_Problem<real_type,device_type>;
    problem_type problem;
    /// problem workspace
    const ordinal_type problem_workspace_size =
      problem_type ::getWorkSpaceSize(kmcd, kmcdSurf);

    return problem_workspace_size ;
  }

  template<typename MemberType,
           typename WorkViewType>
  KOKKOS_INLINE_FUNCTION static void team_invoke(
    const MemberType& member,
    /// input
    const real_type_1d_view_type& x, ///
    /// output
    const real_type_2d_view_type& Jac,
    const real_type_1d_view_type& fac, /// numerica jacobian percentage
    // work
    const WorkViewType& work,
    /// const input from kinetic model
    const kinetic_model_type& kmcd,
    const kinetic_surf_model_type& kmcdSurf,
    // const input for plug flow reactor
    const cstr_data_type& cstr)
  {

    using problem_type =
      TChem::Impl::TransientContStirredTankReactor_Problem<real_type,device_type>;

    problem_type problem;

    /// problem workspace
    const ordinal_type problem_workspace_size =
      problem_type ::getWorkSpaceSize(kmcd, kmcdSurf);

    auto wptr = work.data();
    auto pw = real_type_1d_view_type(wptr, problem_workspace_size);
    wptr += problem_workspace_size;

    /// constant values of the problem
    problem._kmcd = kmcd;         // kinetic model
    problem._kmcdSurf = kmcdSurf; // surface kinetic model
    problem._cstr = cstr;
    problem._work = pw; // problem workspace array
    problem._fac = fac;    // fac for numerical jacobian

    problem.computeNumericalJacobianRichardsonExtrapolation(member, x, Jac);

    member.team_barrier();


#if defined(TCHEM_ENABLE_SERIAL_TEST_OUTPUT) && !defined(__CUDA_ARCH__)
    if (member.league_rank() == 0) {
    }
#endif

}

};

} // namespace Impl
} // namespace TChem

#endif
