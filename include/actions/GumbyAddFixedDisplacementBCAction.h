#pragma once
#include "Action.h"

class GumbyAddFixedDisplacementBCAction : public Action
{
public:
  static InputParameters validParams();
  GumbyAddFixedDisplacementBCAction(const InputParameters & params);
  virtual void act() override;

protected:
  std::vector<VariableName> _displacements;
  std::vector<std::string> _components;
};
