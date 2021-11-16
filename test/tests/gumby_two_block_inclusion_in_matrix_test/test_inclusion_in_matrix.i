[Mesh]
  type = FileMesh
  file = inclusion_in_matrix.e
[]

[GlobalParams]
  displacements = 'displ_x displ_y'
[]

[GumbyVariables]
  displacements_family = LAGRANGE
  displacements_order = FIRST
[]

[GumbySections]
  [./soft_rubber_section]
    base_name = 'soft_rubber'
    block = 1
  [../]
  [./hard_rubber_section]
    base_name = 'hard_rubber'
    block = 2
  [../]
[]

[Materials]
  [soft_rubber]
    type = GumbyADNeohookean
    block = 1
    base_name = 'soft_rubber'
    plane_strain = on
    bulk_modulus = 100.0e6
    shear_modulus = 0.1e6
  []
  [hard_rubber]
    type = GumbyADNeohookean
    block = 2
    base_name = 'hard_rubber'
    plane_strain = on
    bulk_modulus = 1000.0e6
    shear_modulus = 0.1e6
  []
[]

[Functions]
  [ramp]
    type = ParsedFunction
    value = 't'
  []
[]

[BCs]
  [./GumbyFixedDisplacementBC]
    [./left_fixed_xy]
      components = 'x y'
      boundary = 1
    [../]
    [./right_fixed_y]
      components = 'y'
      boundary = 2
    [../]
  [../]
  [./right_displace_x]
    type = FunctionDirichletBC
    variable = displ_x
    boundary = 2
    function = ramp
  [../]
[]

# [GumbyOutputElementVariables]
#   base_names = 'soft_rubber'
#   blocks = '1 2'
#   deformation_gradient = 'F'
#   pk1_stress = 'P'
# []

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
  start_time = 0.0
  end_time = 1.0
  dt = 0.01
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-10
  l_max_its = 1000
  nl_max_its = 250
[]

[Outputs]
  exodus = true
  perf_graph = true
  print_linear_residuals = false
[]
