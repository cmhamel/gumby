#pragma once
#include "Kernel.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"

class GumbyStressDivergence : public Kernel
{
public:
  static InputParameters validParams();
  GumbyStressDivergence(const InputParameters & parameters);
protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  // stuff for using displacement as a "coupling variable"
  //
  const std::string _base_name;
  const unsigned int _component;
  unsigned int _ndisp;
  std::vector<unsigned int> _disp_var;

  // Material properties
  //
  const MaterialProperty<RankTwoTensor> * _F_old;
  const MaterialProperty<RankTwoTensor> * _F_new;
  const MaterialProperty<RankTwoTensor> & _pk1_stress;
  const MaterialProperty<RankFourTensor> & _material_tangent;
};
