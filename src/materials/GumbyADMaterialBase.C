#include "GumbyADMaterialBase.h"

registerMooseObject("GumbyApp", GumbyADMaterialBase);

InputParameters
GumbyADMaterialBase::validParams()
{
  InputParameters params = ADMaterial::validParams();
  params.addParam<std::string>("base_name",
                               "material base name");
  params.addRequiredCoupledVar("displacements",
                               "The string of displacement variables");
  params.addParam<std::string>("plane_strain", "boolean for plane strain");
  std::cout << "in validParams in GumbyADMaterialBase" << std::endl;

  return params;
}

GumbyADMaterialBase::GumbyADMaterialBase(const InputParameters & parameters)
  : ADMaterial(parameters),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _plane_strain((isParamValid("plane_strain") &&
                   getParam<std::string>("plane_strain") == "on") ? true : false),
    _ndisp(coupledComponents("displacements")),
    _grad_disp_new(adCoupledGradients("displacements")),
    _grad_disp_old(coupledGradientsOld("displacements")),
    _F_old(declareADProperty<RankTwoTensor>(_base_name + "deformation_gradient")),
    _F_new(declareADProperty<RankTwoTensor>(_base_name + "deformation_gradient")),
    _pk1_stress(declareADProperty<RankTwoTensor>(_base_name + "pk1_stress"))
{
  std::cout << "in constructor in GumbyADMaterialBase" << std::endl;
  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    if (i < _ndisp)
    {
      _grad_disp_old[i] = &coupledGradientOld("displacements", i);
      _grad_disp_new[i] = &adCoupledGradient("displacements", i);
    }
    else
    {
      _grad_disp_old[i] = &_grad_zero;
      _grad_disp_new[i] = &_ad_grad_zero;
    }
  }
}

// General methods to be overridden
//
void
GumbyADMaterialBase::initQpStatefulProperties()
{
  // _overrides_init_stateful_props = false;
  // std::cout << "in this init method for some odd reason" << std::endl;
  _F_old[_qp].setToIdentity();
  _F_new[_qp].setToIdentity();
  // _pk1_stress[_qp].zero();
}

void
GumbyADMaterialBase::computeQpProperties()
{
  computeDeformationGradient();
}

// debug stuff
//
void GumbyADMaterialBase::printTensor(std::string name, ADRankTwoTensor A)
{
  std::cout << name << " = " << std::endl;
  std::cout << A(0, 0) << "\t" << A(0, 1) << "\t" << A(0, 2) << std::endl;
  std::cout << A(1, 0) << "\t" << A(1, 1) << "\t" << A(1, 2) << std::endl;
  std::cout << A(2, 0) << "\t" << A(2, 1) << "\t" << A(2, 2) << std::endl;
}

// strain methods
//
void
GumbyADMaterialBase::computeDeformationGradient()
{
  if (_ndisp < 3)
  {
    if (_plane_strain)
    {
      _F_old[_qp](0, 0) = (*_grad_disp_old[0])[_qp](0) + 1.0;
      _F_old[_qp](0, 1) = (*_grad_disp_old[0])[_qp](1);
      _F_old[_qp](1, 0) = (*_grad_disp_old[1])[_qp](0);
      _F_old[_qp](1, 1) = (*_grad_disp_old[1])[_qp](1) + 1.0;
      _F_old[_qp](2, 2) = 1.0;

      _F_new[_qp](0, 0) = (*_grad_disp_new[0])[_qp](0) + 1.0;
      _F_new[_qp](0, 1) = (*_grad_disp_new[0])[_qp](1);
      _F_new[_qp](1, 0) = (*_grad_disp_new[1])[_qp](0);
      _F_new[_qp](1, 1) = (*_grad_disp_new[1])[_qp](1) + 1.0;
      _F_new[_qp](2, 2) = 1.0;
    }
    else
    {
      mooseError("Unsupported 2D analysis type");
    }
  }
  else
  {
    ADRankTwoTensor grad_u_old((*_grad_disp_old[0])[_qp],
                               (*_grad_disp_old[1])[_qp],
                               (*_grad_disp_old[2])[_qp]);
    ADRankTwoTensor grad_u_new((*_grad_disp_new[0])[_qp],
                               (*_grad_disp_new[1])[_qp],
                               (*_grad_disp_new[2])[_qp]);

    _F_old[_qp] = grad_u_old + _I;
    _F_new[_qp] = grad_u_new + _I;
  }
}

void
GumbyADMaterialBase::computePolarDecomposition(ADRankTwoTensor & F,
                                               ADRankTwoTensor & R,
                                               ADRankTwoTensor & U)
{
  // do the usual thing
  //
  ADRankTwoTensor C_temp, eigen_vectors, N_1, N_2, N_3, U_inv_temp;
  std::vector<ADReal> eigen_values(3);
  ADReal lambda_1, lambda_2, lambda_3;

  C_temp = F.transpose() * F;
  C_temp.symmetricEigenvaluesEigenvectors(eigen_values, eigen_vectors);

  lambda_1 = std::sqrt(eigen_values[0]);
  lambda_2 = std::sqrt(eigen_values[1]);
  lambda_3 = std::sqrt(eigen_values[2]);

  N_1.vectorOuterProduct(eigen_vectors.column(0), eigen_vectors.column(0));
  N_2.vectorOuterProduct(eigen_vectors.column(1), eigen_vectors.column(1));
  N_3.vectorOuterProduct(eigen_vectors.column(2), eigen_vectors.column(2));

  U = lambda_1 * N_1 + lambda_2 * N_2 + lambda_3 * N_3;
  U_inv_temp = U.inverse();
  R = F * U_inv_temp;
}

ADRankTwoTensor
GumbyADMaterialBase::computeFbar(ADRankTwoTensor F)
{
  ADReal J = F.det();
  ADRankTwoTensor Fbar = std::pow(J, -1.0 / 3.0) * F;
  return Fbar;
}

ADRankTwoTensor
GumbyADMaterialBase::computeLinearStrain(ADRankTwoTensor F)
{
  ADRankTwoTensor grad_u = F - _I;
  ADRankTwoTensor strain = 0.5 * (grad_u + grad_u.transpose());

  return strain;
}

ADRankTwoTensor
GumbyADMaterialBase::computeGreenLagrangeStrain(ADRankTwoTensor F)
{
  ADRankTwoTensor C = F.transpose() * F;
  ADRankTwoTensor E = 0.5 * (C - _I);
  return E;
}

ADRankTwoTensor
GumbyADMaterialBase::computeUnrotatedLogStrain(ADRankTwoTensor U)
{
  ADRankTwoTensor eigen_vectors, N_1, N_2, N_3;
  std::vector<ADReal> eigen_values(3);
  ADReal lambda_1, lambda_2, lambda_3;
  U.symmetricEigenvaluesEigenvectors(eigen_values, eigen_vectors);

  lambda_1 = std::log(eigen_values[0]);
  lambda_2 = std::log(eigen_values[1]);
  lambda_3 = std::log(eigen_values[2]);

  N_1.vectorOuterProduct(eigen_vectors.column(0), eigen_vectors.column(0));
  N_2.vectorOuterProduct(eigen_vectors.column(1), eigen_vectors.column(1));
  N_3.vectorOuterProduct(eigen_vectors.column(2), eigen_vectors.column(2));

  ADRankTwoTensor logU = lambda_1 * N_1 + lambda_2 * N_2 + lambda_3 * N_3;
  return logU;
}

ADRankTwoTensor
GumbyADMaterialBase::computeTensorExponential(ADRankTwoTensor A)
{
  ADRankTwoTensor eigen_vectors, N_1, N_2, N_3;
  std::vector<ADReal> eigen_values(3);
  ADReal lambda_1, lambda_2, lambda_3;
  A.symmetricEigenvaluesEigenvectors(eigen_values, eigen_vectors);

  lambda_1 = std::exp(eigen_values[0]);
  lambda_2 = std::exp(eigen_values[1]);
  lambda_3 = std::exp(eigen_values[2]);

  N_1.vectorOuterProduct(eigen_vectors.column(0), eigen_vectors.column(0));
  N_2.vectorOuterProduct(eigen_vectors.column(1), eigen_vectors.column(1));
  N_3.vectorOuterProduct(eigen_vectors.column(2), eigen_vectors.column(2));

  ADRankTwoTensor expA = lambda_1 * N_1 + lambda_2 * N_2 + lambda_3 * N_3;
  return expA;
}

// mapping between stress quantities methods
//
ADRankTwoTensor
GumbyADMaterialBase::mapPK2StressToPK1Stress(ADRankTwoTensor S)
{
  // P = FS
  //
  ADRankTwoTensor P = _F_new[_qp] * S;
  return P;
}
