#pragma once
#include "Material.h"
#include "RankTwoTensor.h"
#include "RankFourTensor.h"

class GumbyMaterialBase : public Material
{
public:
  static InputParameters validParams();
  GumbyMaterialBase(const InputParameters & parameters);

protected:

  // standard methods
  //
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  // methods to override for calculating stress and tangent
  //
  // virtual RankTwoTensor computePK1Stress();
  // virtual RankFourTensor computeMaterialTangent();

  // strain methods
  //
  void computeDeformationGradient(void);
  void computePolarDecomposition(void);
  RankTwoTensor computeFbar(RankTwoTensor);
  RankTwoTensor computeLinearStrain(RankTwoTensor);
  RankTwoTensor computeGreenLagrangeStrain(RankTwoTensor);
  // log strain
  // other strain measures?

  // TODO: functions to map different stresses between each other
  //
  RankTwoTensor mapPK2StressToPK1Stress(RankTwoTensor);
  RankFourTensor mapPK2TangentToPK1Tangent(RankTwoTensor, RankFourTensor);

  // general class variables that should apply to all material models
  // this is purposely a minimal set
  //
  const std::string _base_name;
  const bool _plane_strain;  // TODO: add plane stress support eventually

  unsigned int _ndisp;

  // convenient tensors
  //
  RankTwoTensor _I;
  RankFourTensor _IxI;
  RankFourTensor _II;

  // Quadrature gradient values for all quad points
  //
  std::vector<const VariableGradient *> _grad_disp_old;
  std::vector<const VariableGradient *> _grad_disp_new;

  // material property to be filled
  //
  MaterialProperty<RankTwoTensor> & _F_old;
  MaterialProperty<RankTwoTensor> & _F_new;

  // think this out more and make right vs. left stretch an option
  //
  // MaterialProperty<RankTwoTensor> & _R_old;
  // MaterialProperty<RankTwoTensor> & _R_new;
  // MaterialProperty<RankTwoTensor> & _U_old;
  // MaterialProperty<RankTwoTensor> & _U_new;
  RankTwoTensor _R_old;
  RankTwoTensor _R_new;
  RankTwoTensor _U_old;
  RankTwoTensor _U_new;

  // constitutive variables
  //
  MaterialProperty<RankTwoTensor> & _pk1_stress;
  MaterialProperty<RankFourTensor> & _material_tangent;
};
