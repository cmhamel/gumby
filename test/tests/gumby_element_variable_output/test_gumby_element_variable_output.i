[Mesh]
  type = FileMesh
  file = cube.e
[]

[GlobalParams]
  displacements = 'displ_x displ_y displ_z'
[]

[GumbyVariables]
  displacements_family = LAGRANGE
  displacements_order = FIRST
[]

[GumbySections]
  [./rubber_section]
    base_name = 'rubber'
    block = 1
  [../]
[]

[Materials]
  [rubber]
    type = GumbyADNeohookean
    block = 1
    base_name = 'rubber'
    bulk_modulus = 1000.0e6
    shear_modulus = 1.0e6
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
    [./corner_fixed_xyz]
      components = 'x y z'
      boundary = 7
    [../]
    [./left_fixed_xyz]
      components = 'x y z'
      boundary = 1
    [../]
    [./right_fixed_xz]
      components = 'x z'
      boundary = 4
    [../]
  [../]
  [right_displace_y]
    type = FunctionDirichletBC
    variable = displ_y
    boundary = 4
    function = ramp
  []
[]

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

[GumbyOutputElementVariables]
  # [./rubber_section_outputs]
  base_names = 'rubber'
  deformation_gradient = 'F'
  pk1_stress = 'P'
  # [../]
[]

[Outputs]
  exodus = true
  perf_graph = true
  print_linear_residuals = false
[]
