#pragma once
#include "ADMaterial.h"
#include "RankTwoTensor.h"

class GumbyADMaterialBase : public ADMaterial
{
public:
  static InputParameters validParams();
  GumbyADMaterialBase(const InputParameters & parameters);

protected:

  // standard methods
  //
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  // strain methods
  //
  void computeDeformationGradient(void);
  void computePolarDecomposition(void);
  ADRankTwoTensor computeFbar(ADRankTwoTensor);
  ADRankTwoTensor computeLinearStrain(ADRankTwoTensor);
  ADRankTwoTensor computeGreenLagrangeStrain(ADRankTwoTensor);
  // log strain
  // other strain measures?

  // TODO: functions to map different stresses between each other
  //
  ADRankTwoTensor mapPK2StressToPK1Stress(ADRankTwoTensor);

  // general class variables that should apply to all material models
  // this is purposely a minimal set
  //
  const std::string _base_name;
  const bool _plane_strain;  // TODO: add plane stress support eventually
  unsigned int _ndisp;

  // unsigned int _ndisp;

  // convenient tensors
  //
  ADRankTwoTensor _I;
  ADRankFourTensor _IxI;
  ADRankFourTensor _II;

  // Quadrature gradient values for all quad points
  //
  // const VectorVariableGradient & _grad_disp_old;
  // const ADVectorVariableGradient & _grad_disp_new;
  std::vector<const VariableGradient *> _grad_disp_old;
  std::vector<const ADVariableGradient *> _grad_disp_new;

  // material property to be filled
  //
  ADMaterialProperty<RankTwoTensor> & _F_old;
  ADMaterialProperty<RankTwoTensor> & _F_new;

  // think this out more and make right vs. left stretch an option
  //
  ADRankTwoTensor _R_old;
  ADRankTwoTensor _U_old;

  ADRankTwoTensor _R_new;
  ADRankTwoTensor _U_new;

  ADMaterialProperty<RankTwoTensor> & _pk1_stress;
};
