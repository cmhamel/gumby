#include "GumbySaintVenantKirchoff.h"

registerMooseObject("GumbyApp", GumbySaintVenantKirchoff);

InputParameters
GumbySaintVenantKirchoff::validParams()
{
  InputParameters params = GumbyMaterialBase::validParams();
  params.addRequiredParam<Real>("bulk_modulus", "bulk modulus");
  params.addRequiredParam<Real>("shear_modulus", "shear modulus");
  return params;
}

GumbySaintVenantKirchoff::GumbySaintVenantKirchoff(const InputParameters & parameters)
  : GumbyMaterialBase(parameters),
    _bulk_modulus(getParam<Real>("bulk_modulus")),
    _shear_modulus(getParam<Real>("shear_modulus"))
{}

void GumbySaintVenantKirchoff::initQpStatefulProperties()
{
  GumbyMaterialBase::initQpStatefulProperties();
}

void GumbySaintVenantKirchoff::computeQpProperties()
{
  // kinematic calculations
  //
  computeDeformationGradient();
  RankTwoTensor strain = GumbyMaterialBase::computeGreenLagrangeStrain(_F_new[_qp]);
  Real trace_strain = strain.trace();
  RankTwoTensor strain_dev = strain.deviatoric();

  // PK2 stress quantities
  //
  RankTwoTensor S = 2.0 * _shear_modulus * strain_dev +
                    _bulk_modulus * trace_strain * _I;
  RankFourTensor C = 2.0 * _shear_modulus * (_II - (1.0 / 3.0) * _IxI) +
                     _bulk_modulus * _IxI;

  // Map to PK1 quantities
  //
  _pk1_stress[_qp] = mapPK2StressToPK1Stress(S);
  _material_tangent[_qp] = mapPK2TangentToPK1Tangent(S, C);
}
