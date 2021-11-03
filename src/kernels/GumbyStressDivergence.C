#include "GumbyStressDivergence.h"

registerMooseObject("GumbyApp", GumbyStressDivergence);

defineLegacyParams(GumbyStressDivergence);

InputParameters
GumbyStressDivergence::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription("Stress Divergence operator");
  params.addRequiredRangeCheckedParam<unsigned int>("component",
                                                    "component < 3",
                                                    "In integer");
  params.addRequiredCoupledVar("displacements",
                               "The string of displacment variables");
  params.addParam<std::string>("base_name", "Material base name");

  return params;
}

GumbyStressDivergence::
GumbyStressDivergence(const InputParameters & parameters)
  : Kernel(parameters),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _component(getParam<unsigned int>("component")),
    _ndisp(coupledComponents("displacements")),
    _disp_var(_ndisp),
    _pk1_stress(getMaterialPropertyByName<RankTwoTensor>(_base_name + "pk1_stress")),
    _material_tangent(getMaterialPropertyByName<RankFourTensor>(_base_name + "material_tangent"))
{
  // _F_new = &getMaterialProperty<RankTwoTensor>(_base_name + "deformation_gradient_new");
  // _F_old = &getMaterialProperty<RankTwoTensor>(_base_name + "deformation_gradient_old");
}

Real
GumbyStressDivergence::computeQpResidual()
{
  return _pk1_stress[_qp].row(_component) * _grad_test[_i][_qp];
}

Real
GumbyStressDivergence::computeQpJacobian()
{
  Real jacobian_1 = (_material_tangent[_qp](_component, 0, _component, 0) * _grad_phi[_j][_qp](0) +
                     _material_tangent[_qp](_component, 0, _component, 1) * _grad_phi[_j][_qp](1) +
                     _material_tangent[_qp](_component, 0, _component, 2) * _grad_phi[_j][_qp](2)) * _grad_test[_i][_qp](0);
  Real jacobian_2 = (_material_tangent[_qp](_component, 1, _component, 0) * _grad_phi[_j][_qp](0) +
                     _material_tangent[_qp](_component, 1, _component, 1) * _grad_phi[_j][_qp](1) +
                     _material_tangent[_qp](_component, 1, _component, 2) * _grad_phi[_j][_qp](2)) * _grad_test[_i][_qp](1);
  Real jacobian_3 = (_material_tangent[_qp](_component, 2, _component, 0) * _grad_phi[_j][_qp](0) +
                     _material_tangent[_qp](_component, 2, _component, 1) * _grad_phi[_j][_qp](1) +
                     _material_tangent[_qp](_component, 2, _component, 2) * _grad_phi[_j][_qp](2)) * _grad_test[_i][_qp](2);

  return jacobian_1 + jacobian_2 + jacobian_3;
}

Real
GumbyStressDivergence::computeQpOffDiagJacobian(unsigned int jvar)
{
  for (unsigned int coupled_component = 0; coupled_component < _ndisp; ++coupled_component)
  {
    if (jvar == _disp_var[coupled_component])
    {
      Real jacobian_1 = (_material_tangent[_qp](_component, 0, coupled_component, 0) * _grad_phi[_j][_qp](0) +
                         _material_tangent[_qp](_component, 0, coupled_component, 1) * _grad_phi[_j][_qp](1) +
                         _material_tangent[_qp](_component, 0, coupled_component, 2) * _grad_phi[_j][_qp](2)) * _grad_test[_i][_qp](0);
      Real jacobian_2 = (_material_tangent[_qp](_component, 1, coupled_component, 0) * _grad_phi[_j][_qp](0) +
                         _material_tangent[_qp](_component, 1, coupled_component, 1) * _grad_phi[_j][_qp](1) +
                         _material_tangent[_qp](_component, 1, coupled_component, 2) * _grad_phi[_j][_qp](2)) * _grad_test[_i][_qp](1);
      Real jacobian_3 = (_material_tangent[_qp](_component, 2, coupled_component, 0) * _grad_phi[_j][_qp](0) +
                         _material_tangent[_qp](_component, 2, coupled_component, 1) * _grad_phi[_j][_qp](1) +
                         _material_tangent[_qp](_component, 2, coupled_component, 2) * _grad_phi[_j][_qp](2)) * _grad_test[_i][_qp](2);
      return jacobian_1 + jacobian_2 + jacobian_3;
    }
    else
    {
      return 0.0;
    }
  }
  return 0.0;
}
