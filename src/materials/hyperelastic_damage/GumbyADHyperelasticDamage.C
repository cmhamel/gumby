#include "GumbyADHyperelasticDamage.h"
#include "GumbyADNeohookean.h"

registerMooseObject("GumbyApp", GumbyADHyperelasticDamage);

InputParameters
GumbyADHyperelasticDamage::validParams()
{
  InputParameters params = GumbyADNeohookean::validParams();
  params.addRequiredParam<Real>("bulk_modulus", "bulk modulus");
  params.addRequiredParam<Real>("shear_modulus", "shear modulus");
  params.addRequiredParam<Real>("damage_max", "max damage");
  params.addRequiredParam<Real>("damage_saturation", "damage saturation");
  return params;
}

GumbyADHyperelasticDamage::GumbyADHyperelasticDamage(const InputParameters & parameters)
  : GumbyADNeohookean(parameters),
    _bulk_modulus(getParam<Real>("bulk_modulus")),
    _shear_modulus(getParam<Real>("shear_modulus")),
    _damage_max(getParam<Real>("damage_max")),
    _damage_saturation(getParam<Real>("damage_saturation")),
    _alpha_new(declareADProperty<Real>("alpha")),
    _alpha_old(getMaterialPropertyOld<Real>("alpha")),
    _damage_new(declareADProperty<Real>("damage"))
{}

void
GumbyADHyperelasticDamage::initQpStatefulProperties()
{
  GumbyADMaterialBase::initQpStatefulProperties();

  // initialize state variables to zero
  // you could add initial conditions for these later on
  //
  _alpha_new[_qp] = 0.0;
  _damage_new[_qp] = 0.0;
}

void
GumbyADHyperelasticDamage::computeQpProperties()
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
}

void
GumbyADHyperelasticDamage::updateStateVariables()
{
  ADReal psi;
  psi = GumbyADNeohookean::computeStrainEnergy(_bulk_modulus, _shear_modulus, _F_new[_qp]);

  _alpha_new[_qp] = std::max(_alpha_old[_qp], psi);
  _damage_new[_qp] = _damage_max * (1.0 - exp(-_alpha_new[_qp] / _damage_saturation));

}

ADRankTwoTensor
GumbyADHyperelasticDamage::computePK1Stress(ADReal K, ADReal G, ADRankTwoTensor F)
{
  ADRankTwoTensor P;
  P = (1.0 - _damage_new[_qp]) *
      GumbyADNeohookean::computePK1Stress(_bulk_modulus, _shear_modulus, _F_new[_qp]);
  return P;
}
