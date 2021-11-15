#pragma once
// #include "GumbyMaterialBase.h"
#include "GumbyADNeohookean.h"

class GumbyADHyperelasticDamage : public GumbyADNeohookean
{
public:
  static InputParameters validParams();
  GumbyADHyperelasticDamage(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  void updateStateVariables();

  ADRankTwoTensor computePK1Stress(ADReal,ADReal,ADRankTwoTensor);

  // material properties
  //
  const ADReal _bulk_modulus;
  const ADReal _shear_modulus;
  const ADReal _damage_max;
  const ADReal _damage_saturation;

  // state variables
  //
  ADMaterialProperty<Real> & _alpha_new;
  const MaterialProperty<Real> & _alpha_old;

  ADMaterialProperty<Real> & _damage_new;
};
