#pragma once
#include "GumbyADMaterialBase.h"

class GumbyADNeohookean : public GumbyADMaterialBase
{
public:
  static InputParameters validParams();
  GumbyADNeohookean(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  ADReal computeStrainEnergy(ADReal,ADReal,ADRankTwoTensor);
  ADRankTwoTensor computePK1Stress(ADReal,ADReal,ADRankTwoTensor);

  // RankTwoTensor computePK1Stress();

  const ADReal _bulk_modulus;
  const ADReal _shear_modulus;
};
