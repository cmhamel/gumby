#include "GumbyMaterialBase.h"

registerMooseObject("GumbyApp", GumbyMaterialBase);

InputParameters
GumbyMaterialBase::validParams()
{
  InputParameters params = Material::validParams();
  params.addParam<std::string>("base_name",
                               "material base name");
  params.addRequiredCoupledVar("displacements",
                               "The string of displacement variables");
  params.addParam<std::string>("plane_strain", "boolean for plane strain");
  return params;
}

GumbyMaterialBase::GumbyMaterialBase(const InputParameters & parameters)
  : Material(parameters),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _plane_strain((isParamValid("plane_strain") &&
                   getParam<std::string>("plane_strain") == "on") ? true : false),
    _ndisp(coupledComponents("displacements")),
    _I(RankTwoTensor::initIdentity),
    _IxI(_I.outerProduct(_I)),
    _II(_I.mixedProductIkJl(_I)),
    _grad_disp_old(coupledGradientsOld("displacements")),
    _grad_disp_new(coupledGradients("displacements")),
    _F_old(declareProperty<RankTwoTensor>(_base_name + "deformation_gradient_old")),
    _F_new(declareProperty<RankTwoTensor>(_base_name + "deformation_gradient_new")),
    // _R_old(declareProperty<RankTwoTensor>(_base_name + "rotation_tensor_old")),
    // _R_new(declareProperty<RankTwoTensor>(_base_name + "rotation_tensor_new")),
    // _U_old(declareProperty<RankTwoTensor>(_base_name + "right_stretch_tensor_old")),
    // _U_new(declareProperty<RankTwoTensor>(_base_name + "right_stretch_tensor_new")),
    _R_old(_I),
    _R_new(_I),
    _U_old(_I),
    _U_new(_I),
    _pk1_stress(declareProperty<RankTwoTensor>(_base_name + "pk1_stress")),
    _material_tangent(declareProperty<RankFourTensor>(_base_name + "material_tangent"))
{
  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    if (i < _ndisp)
    {
      _grad_disp_old[i] = &coupledGradientOld("displacements", i);
      _grad_disp_new[i] = &coupledGradient("displacements", i);
    }
    else
    {
      _grad_disp_old[i] = &_grad_zero;
      _grad_disp_new[i] = &_grad_zero;
    }
  }
}

// General methods to be overriddent
//
void
GumbyMaterialBase::initQpStatefulProperties()
{
  _F_old[_qp].setToIdentity();
  _F_new[_qp].setToIdentity();
  // _R_old[_qp].setToIdentity();
  // _R_new[_qp].setToIdentity();
  // _U_old[_qp].setToIdentity();
  // _U_new[_qp].setToIdentity();
  _pk1_stress[_qp].zero();
  _material_tangent[_qp].zero();
}

void
GumbyMaterialBase::computeQpProperties()
{
  computeDeformationGradient();
}

// strain methods
//
void
GumbyMaterialBase::computeDeformationGradient()
{
  // first step is to calculate the deformation gradients from
  // the displacement gradients
  //
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
    RankTwoTensor grad_u_old((*_grad_disp_old[0])[_qp],
                             (*_grad_disp_old[1])[_qp],
                             (*_grad_disp_old[2])[_qp]);
    RankTwoTensor grad_u_new((*_grad_disp_new[0])[_qp],
                             (*_grad_disp_new[1])[_qp],
                             (*_grad_disp_new[2])[_qp]);

    _F_old[_qp] = grad_u_old + _I;
    _F_new[_qp] = grad_u_new + _I;
  }
}

void
GumbyMaterialBase::computePolarDecomposition()
{
  // do the usual thing
  //
  RankTwoTensor C_temp, eigen_vectors, N_1, N_2, N_3, U_inv_temp;
  std::vector<Real> eigen_values(3);
  Real lambda_1, lambda_2, lambda_3;

  // old deformation gradient
  //
  C_temp = _F_old[_qp].transpose() * _F_old[_qp];
  C_temp.symmetricEigenvaluesEigenvectors(eigen_values, eigen_vectors);

  lambda_1 = std::sqrt(eigen_values[0]);
  lambda_2 = std::sqrt(eigen_values[1]);
  lambda_3 = std::sqrt(eigen_values[2]);

  N_1.vectorOuterProduct(eigen_vectors.column(0), eigen_vectors.column(0));
  N_2.vectorOuterProduct(eigen_vectors.column(1), eigen_vectors.column(1));
  N_3.vectorOuterProduct(eigen_vectors.column(2), eigen_vectors.column(2));

  // _U_old[_qp] = lambda_1 * N_1 + lambda_2 * N_2 + lambda_3 * N_3;
  // U_inv_temp = _U_old[_qp].inverse();
  // _R_old[_qp] = _F_old[_qp] * U_inv_temp;

  _U_old = lambda_1 * N_1 + lambda_2 * N_2 + lambda_3 * N_3;
  U_inv_temp = _U_old.inverse();
  _R_old = _F_old[_qp] * U_inv_temp;

  // new deformation gradient
  //
  C_temp = _F_new[_qp].transpose() * _F_new[_qp];
  C_temp.symmetricEigenvaluesEigenvectors(eigen_values, eigen_vectors);

  lambda_1 = std::sqrt(eigen_values[0]);
  lambda_2 = std::sqrt(eigen_values[1]);
  lambda_3 = std::sqrt(eigen_values[2]);

  N_1.vectorOuterProduct(eigen_vectors.column(0), eigen_vectors.column(0));
  N_2.vectorOuterProduct(eigen_vectors.column(1), eigen_vectors.column(1));
  N_3.vectorOuterProduct(eigen_vectors.column(2), eigen_vectors.column(2));

  // _U_new[_qp] = lambda_1 * N_1 + lambda_2 * N_2 + lambda_3 * N_3;
  // U_inv_temp = _U_new[_qp].inverse();
  // _R_new[_qp] = _F_new[_qp] * U_inv_temp;

  _U_new = lambda_1 * N_1 + lambda_2 * N_2 + lambda_3 * N_3;
  U_inv_temp = _U_new.inverse();
  _R_new = _F_new[_qp] * U_inv_temp;
}

RankTwoTensor
GumbyMaterialBase::computeFbar(RankTwoTensor F)
{
  Real J = F.det();
  RankTwoTensor Fbar = std::pow(J, -1.0 / 3.0) * F;
  return Fbar;
}

RankTwoTensor
GumbyMaterialBase::computeLinearStrain(RankTwoTensor F)
{
  RankTwoTensor grad_u = F - _I;
  RankTwoTensor strain = 0.5 * (grad_u + grad_u.transpose());

  return strain;
}

RankTwoTensor
GumbyMaterialBase::computeGreenLagrangeStrain(RankTwoTensor F)
{
  RankTwoTensor C = F.transpose() * F;
  RankTwoTensor E = 0.5 * (C - _I);
  return E;
}

// mapping between stress quantities methods
//
RankTwoTensor
GumbyMaterialBase::mapPK2StressToPK1Stress(RankTwoTensor S)
{
  // P = FS
  //
  RankTwoTensor P = _F_new[_qp] * S;
  return P;
}

RankFourTensor
GumbyMaterialBase::mapPK2TangentToPK1Tangent(RankTwoTensor S, RankFourTensor C)
{
  // A_ijkl = delta_ik S_jl + F_im F_kn C_mjnl
  //
  RankFourTensor A = _I.mixedProductIkJl(S);
  for (unsigned int i = 0; i < 3; ++i)
    for (unsigned int j = 0; j < 3; ++j)
      for (unsigned int k = 0; k < 3; ++k)
        for (unsigned int l = 0; l < 3; ++l)
          for (unsigned int m = 0; m < 3; ++m)
            for (unsigned int n = 0; n < 3; ++n)
              A(i, j, k, l) += _F_new[_qp](i, m) * _F_new[_qp](k, n) * C(m, j, n, l);
  return A;
}
