#include "GumbyNeohookean.h"

registerMooseObject("GumbyApp", GumbyNeohookean);

InputParameters
GumbyNeohookean::validParams()
{
  InputParameters params = GumbyMaterialBase::validParams();
  params.addRequiredParam<Real>("bulk_modulus", "bulk modulus");
  params.addRequiredParam<Real>("shear_modulus", "shear modulus");
  return params;
}

GumbyNeohookean::GumbyNeohookean(const InputParameters & parameters)
  : GumbyMaterialBase(parameters),
    _bulk_modulus(getParam<Real>("bulk_modulus")),
    _shear_modulus(getParam<Real>("shear_modulus"))
{}

void
GumbyNeohookean::initQpStatefulProperties()
{
  GumbyMaterialBase::initQpStatefulProperties();
}

void
GumbyNeohookean::computeQpProperties()
{
  // kinematics
  //
  computeDeformationGradient();

  // constitutive response
  //
  _pk1_stress[_qp] = computePK1Stress(_bulk_modulus, _shear_modulus, _F_new[_qp]);
  _material_tangent[_qp] = computeMaterialTangent(_bulk_modulus, _shear_modulus, _F_new[_qp]);
}

Real
GumbyNeohookean::computeStrainEnergy(Real K, Real G, RankTwoTensor F)
{
  const Real I_1 = (F * F.transpose()).trace();
  const Real J = F.det();
  const Real J_minus_2_3 = std::pow(J, -2.0 / 3.0);

  Real W = (G / 2.0) * (J_minus_2_3 * I_1 - 3.0) + K * (J * J - 1.0);
  return W;
}

RankTwoTensor
GumbyNeohookean::computePK1Stress(Real K, Real G, RankTwoTensor F)
{
  // kinematics
  //
  const RankTwoTensor F_inv_T = F.inverse().transpose();
  const Real I_1 = (F * F.transpose()).trace();
  const Real J = F.det();
  const Real J_minus_2_3 = std::pow(J, -2.0 / 3.0);
  const Real third = 1.0 / 3.0;

  // calculate response
  //
  RankTwoTensor P;
  P = K * J * std::log(J) * F_inv_T;
  P += G * J_minus_2_3 * (F - third * I_1 * F_inv_T);
  return P;
}

RankFourTensor
GumbyNeohookean::computeMaterialTangent(Real K, Real G, RankTwoTensor F)
{
  // some kinematics
  //
  const RankTwoTensor F_inv = F.inverse();
  const Real I_1 = (F * F.transpose()).trace();
  const Real J = F.det();
  const Real J_minus_2_3 = std::pow(J, -2.0 / 3.0);
  const Real third = 1.0 / 3.0;
  const Real two_thirds = 2.0 / 3.0;
  const Real two_ninths = 2.0 / 9.0;

  // tangent calculation
  //
  RankFourTensor A;
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
        for (int l = 0; l < 3; ++l)
        {
          A(i, j, k, l) =
          J_minus_2_3 * G *
          (
            -two_thirds * F(i, j) * F_inv(l, k) +
            two_ninths * I_1 * F_inv(j, i) * F_inv(l, k) +
            _I(i, k) * _I(j, l) +
            -two_thirds * F_inv(j, i) * F(k, l) +
            third * I_1 * F_inv(l, i) * F_inv(j, k)
          ) +
          K *
          (
            F_inv(j, i) * F_inv(l, k) +
            -std::log(J) * F_inv(l, i) * F_inv(j, k)
          );
        }
  return A;
}
