#include "GumbyHyperelasticDamage.h"
#include "GumbyNeohookean.h"

registerMooseObject("GumbyApp", GumbyHyperelasticDamage);

InputParameters
GumbyHyperelasticDamage::validParams()
{
  InputParameters params = GumbyNeohookean::validParams();
  params.addRequiredParam<Real>("bulk_modulus", "bulk modulus");
  params.addRequiredParam<Real>("shear_modulus", "shear modulus");
  params.addRequiredParam<Real>("damage_max", "max damage");
  params.addRequiredParam<Real>("damage_saturation", "damage saturation");
  return params;
}

GumbyHyperelasticDamage::GumbyHyperelasticDamage(const InputParameters & parameters)
  : GumbyNeohookean(parameters),
    _bulk_modulus(getParam<Real>("bulk_modulus")),
    _shear_modulus(getParam<Real>("shear_modulus")),
    _damage_max(getParam<Real>("damage_max")),
    _damage_saturation(getParam<Real>("damage_saturation")),
    _alpha_new(declareProperty<Real>("alpha")),
    _alpha_old(getMaterialPropertyOld<Real>("alpha")),
    _damage_new(declareProperty<Real>("damage"))
{}

void
GumbyHyperelasticDamage::initQpStatefulProperties()
{
  GumbyMaterialBase::initQpStatefulProperties();

  // initialize state variables to zero
  // you could add initial conditions for these later on
  //
  _alpha_new[_qp] = 0.0;
  _damage_new[_qp] = 0.0;
}

void
GumbyHyperelasticDamage::computeQpProperties()
{
  // kinematics
  //
  computeDeformationGradient();

  // update state variables
  //
  updateStateVariables();

  // constitutive response
  //
  _pk1_stress[_qp] = computePK1Stress(_bulk_modulus, _shear_modulus, _F_new[_qp]);
  _material_tangent[_qp] = computeMaterialTangent(_bulk_modulus, _shear_modulus, _F_new[_qp]);
}

void
GumbyHyperelasticDamage::updateStateVariables()
{
  Real psi;
  psi = GumbyNeohookean::computeStrainEnergy(_bulk_modulus, _shear_modulus, _F_new[_qp]);

  _alpha_new[_qp] = std::max(_alpha_old[_qp], psi);
  _damage_new[_qp] = _damage_max * (1.0 - exp(-_alpha_new[_qp] / _damage_saturation));

}

RankTwoTensor
GumbyHyperelasticDamage::computePK1Stress(Real K, Real G, RankTwoTensor F)
{
  RankTwoTensor P;
  P = (1.0 - _damage_new[_qp]) *
      GumbyNeohookean::computePK1Stress(_bulk_modulus, _shear_modulus, _F_new[_qp]);
  return P;
}

RankFourTensor
GumbyHyperelasticDamage::computeMaterialTangent(Real K, Real G, RankTwoTensor F)
{
  RankTwoTensor P = computePK1Stress(K, G, F);
  RankFourTensor A;
  A = (1.0 - _damage_new[_qp]) *
      GumbyNeohookean::computeMaterialTangent(_bulk_modulus, _shear_modulus, _F_new[_qp]);
  Real factor;
  if (_alpha_new[_qp] > _alpha_old[_qp])
    factor = -1.0 * (_damage_max / _damage_saturation) * exp(-_alpha_new[_qp] / _damage_saturation);
  else
    factor = 0.0;

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      for (int k = 0; k < 3; ++k)
        for (int l = 0; l < 3; ++l)
          A(i, j, k, l) += factor * P(i, j) * P(k, l);
  return A;
}
