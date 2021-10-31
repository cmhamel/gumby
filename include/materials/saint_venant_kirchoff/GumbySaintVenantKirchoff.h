#pragma once
#include "GumbyMaterialBase.h"

class GumbySaintVenantKirchoff : public GumbyMaterialBase
{
public:
  static InputParameters validParams();
  GumbySaintVenantKirchoff(const InputParameters & parameters);

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  const Real _bulk_modulus;
  const Real _shear_modulus;
};
