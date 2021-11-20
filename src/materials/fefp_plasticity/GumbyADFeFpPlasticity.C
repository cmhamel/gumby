#include "GumbyADFeFpPlasticity.h"

registerMooseObject("GumbyApp", GumbyADFeFpPlasticity);

InputParameters
GumbyADFeFpPlasticity::validParams()
{
  InputParameters params = GumbyADMaterialBase::validParams();
  params.addRequiredParam<Real>("bulk_modulus", "Bulk Modulus");
  params.addRequiredParam<Real>("shear_modulus", "Shear Modulus");
  params.addRequiredParam<Real>("yield_stress", "Yield Stress");
  params.addRequiredParam<Real>("hardening_modulus", "Hardening Modulus");
  return params;
}

GumbyADFeFpPlasticity::
GumbyADFeFpPlasticity(const InputParameters & parameters)
  : GumbyADMaterialBase(parameters),
    _bulk_modulus(getParam<Real>("bulk_modulus")),
    _shear_modulus(getParam<Real>("shear_modulus")),
    _yield_stress(getParam<Real>("yield_stress")),
    _hardening_modulus(getParam<Real>("hardening_modulus")),
    _F_p_old(getMaterialPropertyOld<RankTwoTensor>(_base_name +
      "plastic_deformation_gradient")),
    _F_p_new(declareADProperty<RankTwoTensor>(_base_name +
      "plastic_deformation_gradient")),
    _alpha_old(getMaterialPropertyOld<Real>(_base_name +
      "hardening_variable")),
    _alpha_new(declareADProperty<Real>(_base_name +
      "hardening_variable"))
{
  // check elastic constants and set up other elastic constants
  //
  std::cout << "in this constructor" << std::endl;
  if (_shear_modulus <= 0.0)
    mooseError("shear_modulus <= 0.0");

  if (_bulk_modulus <= 0.0)
    mooseError("bulk_modulus <= 0.0");

  // check yield and hardening_modulus for positivity
  //
  if (_yield_stress <= 0.0)
    mooseError("yield_stress <= 0.0");

  if (_hardening_modulus < 0.0)
    mooseError("hardening_modulus < 0.0");
}

void
GumbyADFeFpPlasticity::initQpStatefulProperties()
{
  // initialize general kinematics and PK1 stress with
  // GumbyADMaterialBase
  //

  // now set state variables initial conditions
  //
  _F_p_new[_qp].setToIdentity();
  _alpha_new[_qp] = 0.0;
}

void
GumbyADFeFpPlasticity::computeQpProperties()
{
  // Step 1: Calculate elastic trial deformation gradient
  //
  computeDeformationGradient();

  // printTensor("F_old", _F_old[_qp]);
  // printTensor("F_new", _F_new[_qp]);
  // printTensor("F_p_old", _F_p_old[_qp]);
  // printTensor("F_p_new", _F_p_new[_qp]);

  // calculate elastic trial log strain
  //
  ADRankTwoTensor F_e_trial, R_e_trial, U_e_trial, E_e_trial;
  F_e_trial = _F_new[_qp] * _F_p_old[_qp].inverse();
  computePolarDecomposition(F_e_trial, R_e_trial, U_e_trial);
  E_e_trial = computeUnrotatedLogStrain(U_e_trial);

  // calculate elastic trial Mandel stress
  //
  ADRankTwoTensor devM_e_trial, N_p, A_p, expA_p;
  ADReal devM_e_bar, delta_gamma, f_trial;
  devM_e_trial = 2.0 * _shear_modulus * E_e_trial.deviatoric();
  devM_e_bar = std::sqrt(1.0 / 2.0) * devM_e_trial.L2norm();

  // check for yield
  //
  f_trial = devM_e_bar - std::sqrt(2.0 / 3.0) * (_hardening_modulus * _alpha_old[_qp] + _yield_stress);

  if (f_trial <= 0.0)
  {
    // elastic step, so set state variables to previous state
    //
    _F_p_new[_qp] = _F_p_old[_qp];
    _alpha_new[_qp] = _alpha_old[_qp];
  }
  else
  {
    // plastic step, so update state variables based on integration rule
    //
    if (devM_e_bar <= 0)
    {
      N_p = _I;
      delta_gamma = 0.0;
      _F_p_new[_qp] = _F_p_old[_qp];
      _alpha_new[_qp] = _alpha_old[_qp];
    }
    else
    {
      N_p = devM_e_trial / (std::sqrt(2.0) * devM_e_bar);
      delta_gamma = (f_trial / (2.0 * _shear_modulus)) / (1.0 + (_hardening_modulus / (3.0 * _shear_modulus)));
      A_p = std::sqrt(1.0 / 2.0) * delta_gamma * N_p;
      expA_p = computeTensorExponential(A_p);
      _F_p_new[_qp] = expA_p * _F_p_old[_qp];
      _alpha_new[_qp] = _alpha_old[_qp] + std::sqrt(2.0 / 3.0) * delta_gamma;
    }
  }

  ADRankTwoTensor F_e_new, R_e_new, U_e_new, E_e_new;
  F_e_new = _F_new[_qp] * _F_p_new[_qp].inverse();
  computePolarDecomposition(F_e_new, R_e_new, U_e_new);
  E_e_new = computeUnrotatedLogStrain(U_e_new);

  ADRankTwoTensor M_e_new;
  M_e_new = (devM_e_trial - std::sqrt(2.0) * _shear_modulus * delta_gamma * N_p)
          + _bulk_modulus * E_e_new.trace() * _I;

  // map to pk1 stress using F_e_new
  //
  _pk1_stress[_qp] = F_e_new.inverse().transpose() * M_e_new;
}
