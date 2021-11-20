#pragma once
#include "GumbyADMaterialBase.h"

class GumbyADFeFpPlasticity : public GumbyADMaterialBase
{
public:
  static InputParameters validParams();
  GumbyADFeFpPlasticity(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  // ADRankTwoTensor computePK1Stress();

  // material properties
  //
  const ADReal _bulk_modulus;
  const ADReal _shear_modulus;
  const ADReal _yield_stress;
  const ADReal _hardening_modulus;

  // state variables
  //
  ADMaterialProperty<RankTwoTensor> & _F_p_new;
  const MaterialProperty<RankTwoTensor> & _F_p_old;

  ADMaterialProperty<Real> & _alpha_new;
  const MaterialProperty<Real> & _alpha_old;
};
