#pragma once
#include "GumbyMaterialBase.h"

class GumbyLinearElasticity : public GumbyMaterialBase
{
public:
  static InputParameters validParams();
  GumbyLinearElasticity(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  const Real _lambda;
  const Real _mu;
};
