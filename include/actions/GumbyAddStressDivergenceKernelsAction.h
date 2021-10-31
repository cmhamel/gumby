#pragma once
#include "Action.h"

class GumbyAddStressDivergenceKernelsAction : public Action
{
public:
  GumbyAddStressDivergenceKernelsAction(InputParameters params);

  virtual void act() override;

protected:
  std::vector<VariableName> _displacements;
  std::string _base_name;
};

template <>
InputParameters validParams<GumbyAddStressDivergenceKernelsAction>();
