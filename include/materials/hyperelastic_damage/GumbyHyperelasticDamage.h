#pragma once
// #include "GumbyMaterialBase.h"
#include "GumbyNeohookean.h"

// class GumbyHyperelasticDamage : public GumbyMaterialBase
class GumbyHyperelasticDamage : public GumbyNeohookean
{
public:
  static InputParameters validParams();
  GumbyHyperelasticDamage(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  void updateStateVariables();

  RankTwoTensor computePK1Stress(Real,Real,RankTwoTensor);
  RankFourTensor computeMaterialTangent(Real,Real,RankTwoTensor);

  // material properties
  //
  const Real _bulk_modulus;
  const Real _shear_modulus;
  const Real _damage_max;
  const Real _damage_saturation;

  // state variables
  //
  MaterialProperty<Real> & _alpha_new;
  const MaterialProperty<Real> & _alpha_old;

  MaterialProperty<Real> & _damage_new;
  // const MaterialProperty<Real> & _damage_old;

  // fill in the below for calculating the jacobian;
  // RankTwoTensor _ddamage_dF;
};
