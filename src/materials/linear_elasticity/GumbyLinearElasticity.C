#include "GumbyLinearElasticity.h"

registerMooseObject("GumbyApp", GumbyLinearElasticity);

InputParameters
GumbyLinearElasticity::validParams()
{
  InputParameters params = GumbyMaterialBase::validParams();
  params.addRequiredParam<Real>("shear_modulus",
                                "shear modulus");
  params.addRequiredParam<Real>("lambda",
                                "lame parameter");
  return params;
}

GumbyLinearElasticity::GumbyLinearElasticity(const InputParameters & parameters)
  : GumbyMaterialBase(parameters),
    _lambda(getParam<Real>("lambda")),
    _mu(getParam<Real>("shear_modulus"))
{}

void
GumbyLinearElasticity::initQpStatefulProperties()
{
  GumbyMaterialBase::initQpStatefulProperties();
}

void
GumbyLinearElasticity::computeQpProperties()
{
  computeDeformationGradient();
  RankTwoTensor strain = GumbyMaterialBase::computeLinearStrain(_F_new[_qp]);
  Real trace_strain = strain.trace();

  // although the variable name is pk1 stress it should be noted that
  // for small linear strain (i.e. small deformations and rotations) that
  // the pk1 stress and the cauchy stress are identical
  //
  _pk1_stress[_qp] = 2.0 * _mu * strain + _lambda * trace_strain * _I;
  _material_tangent[_qp] = 2.0 * _mu * _II + _lambda * _IxI;
}
