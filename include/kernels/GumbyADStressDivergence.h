#pragma once
#include "ADKernel.h"
#include "RankTwoTensor.h"

class GumbyADStressDivergence : public ADKernel
{
public:
  static InputParameters validParams();
  GumbyADStressDivergence(const InputParameters & parameters);

protected:
  virtual ADReal computeQpResidual() override;

  const std::string _base_name;
  const unsigned int _component;
  unsigned int _ndisp;
  std::vector<unsigned int> _disp_var;

  const ADMaterialProperty<RankTwoTensor> & _pk1_stress;
};
