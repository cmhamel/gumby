#pragma once
#include "GumbyMaterialBase.h"

class GumbyNeohookean : public GumbyMaterialBase
{
public:
  static InputParameters validParams();
  GumbyNeohookean(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  RankTwoTensor computePK1Stress(Real,Real,RankTwoTensor);
  RankFourTensor computeMaterialTangent(Real,Real,RankTwoTensor);

  // RankTwoTensor computePK1Stress();

  const Real _bulk_modulus;
  const Real _shear_modulus;
};
