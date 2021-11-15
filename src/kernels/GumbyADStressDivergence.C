#include "GumbyADStressDivergence.h"

registerMooseObject("GumbyApp", GumbyADStressDivergence);

defineLegacyParams(GumbyADStressDivergence);

InputParameters
GumbyADStressDivergence::validParams()
{
  InputParameters params = ADKernel::validParams();
  params.addClassDescription("Stress Divergence operator");
  params.addRequiredRangeCheckedParam<unsigned int>("component",
                                                    "component < 3",
                                                    "In integer");
  params.addRequiredCoupledVar("displacements",
                               "The string of displacment variables");
  params.addParam<std::string>("base_name", "Material base name");
  std::cout << "in validParams in GumbyADStressDivergence" << std::endl;
  return params;
}

GumbyADStressDivergence::
GumbyADStressDivergence(const InputParameters & parameters)
  : ADKernel(parameters),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _component(getParam<unsigned int>("component")),
    _ndisp(coupledComponents("displacements")),
    _disp_var(_ndisp),
    _pk1_stress(getADMaterialPropertyByName<RankTwoTensor>(_base_name + "pk1_stress"))
{
  std::cout << "in constructor in GumbyADStressDivergence" << std::endl;
}

ADReal
GumbyADStressDivergence::computeQpResidual()
{
  ADReal residual = _pk1_stress[_qp].row(_component) * _grad_test[_i][_qp];
  return residual;
}
