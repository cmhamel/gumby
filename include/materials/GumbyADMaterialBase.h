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

  // debug stuff
  //
  void printTensor(std::string name, ADRankTwoTensor A);

  // strain methods
  //
  void computeDeformationGradient(void);
  void computePolarDecomposition(ADRankTwoTensor & F,
                                 ADRankTwoTensor & R,
                                 ADRankTwoTensor & U);
  ADRankTwoTensor computeFbar(ADRankTwoTensor);
  ADRankTwoTensor computeLinearStrain(ADRankTwoTensor);
  ADRankTwoTensor computeGreenLagrangeStrain(ADRankTwoTensor);
  ADRankTwoTensor computeUnrotatedLogStrain(ADRankTwoTensor);
  ADRankTwoTensor computeTensorExponential(ADRankTwoTensor);
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
  ADRankTwoTensor _I = ADRankTwoTensor::initIdentity;
  ADRankFourTensor _IxI = _I.outerProduct(_I);
  ADRankFourTensor _II = _I.mixedProductIkJl(_I);

  // Quadrature gradient values for all quad points
  //
  std::vector<const VariableGradient *> _grad_disp_old;
  std::vector<const ADVariableGradient *> _grad_disp_new;

  // material property to be filled
  //
  ADMaterialProperty<RankTwoTensor> & _F_new;
  ADMaterialProperty<RankTwoTensor> & _F_old;

  ADMaterialProperty<RankTwoTensor> & _pk1_stress;
};
