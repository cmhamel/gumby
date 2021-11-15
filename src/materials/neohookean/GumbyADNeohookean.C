#include "GumbyADNeohookean.h"

registerMooseObject("GumbyApp", GumbyADNeohookean);

InputParameters
GumbyADNeohookean::validParams()
{
  InputParameters params = GumbyADMaterialBase::validParams();
  params.addRequiredParam<Real>("bulk_modulus", "bulk modulus");
  params.addRequiredParam<Real>("shear_modulus", "shear modulus");
  return params;
}

GumbyADNeohookean::GumbyADNeohookean(const InputParameters & parameters)
  : GumbyADMaterialBase(parameters),
    _bulk_modulus(getParam<Real>("bulk_modulus")),
    _shear_modulus(getParam<Real>("shear_modulus"))
{}

void
GumbyADNeohookean::initQpStatefulProperties()
{
  GumbyADMaterialBase::initQpStatefulProperties();
}

void
GumbyADNeohookean::computeQpProperties()
{
  // kinematics
  //
  computeDeformationGradient();

  // constitutive response
  //
  _pk1_stress[_qp] = computePK1Stress(_bulk_modulus, _shear_modulus, _F_new[_qp]);
}

ADReal
GumbyADNeohookean::computeStrainEnergy(ADReal K, ADReal G, ADRankTwoTensor F)
{
  ADReal I_1 = (F * F.transpose()).trace();
  ADReal J = F.det();
  ADReal J_minus_2_3 = std::pow(J, -2.0 / 3.0);
  ADReal W = (G / 2.0) * (J_minus_2_3 * I_1 - 3.0) + K * (J * J - 1.0);
  return W;
}

ADRankTwoTensor
GumbyADNeohookean::computePK1Stress(ADReal K, ADReal G, ADRankTwoTensor F)
{
  // kinematics
  //
  ADRankTwoTensor F_inv_T = F.inverse().transpose();
  ADReal I_1 = (F * F.transpose()).trace();
  ADReal J = F.det();
  ADReal J_minus_2_3 = std::pow(J, -2.0 / 3.0);
  ADReal third = 1.0 / 3.0;

  // calculate response
  //
  ADRankTwoTensor P;
  P = K * J * std::log(J) * F_inv_T;
  P += G * J_minus_2_3 * (F - third * I_1 * F_inv_T);
  return P;
}
