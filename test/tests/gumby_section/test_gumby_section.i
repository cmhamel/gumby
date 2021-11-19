[Mesh]
  [generated_mesh]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 2
    ny = 2
    nz = 2
    xmin = 0
    ymin = 0
    zmin = 0
    xmax = 1
    ymax = 1
    zmax = 1
  []
  [assign_block]
    type = SubdomainBoundingBoxGenerator
    input = generated_mesh
    bottom_left = '0 0 0'
    top_right = '1 1 1'
    block_id = 1
  []
[]

[GumbyVariables]
  displacements = 'displ_x displ_y displ_z'
  displacements_family = LAGRANGE
  displacements_order = FIRST
[]

[GumbySections]
  [./rubber_section]
    displacements = 'displ_x displ_y displ_z'
    base_name = 'rubber'
    block = 1
  [../]
[]

[Materials]
  [rubber]
    type = GumbyADNeohookean
    block = 1
    displacements = 'displ_x displ_y displ_z'
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
  [left_fixed_x]
    type = DirichletBC
    variable = displ_x
    boundary = 'left'
    value = 0.0
  []
  [left_fixed_y]
    type = DirichletBC
    variable = displ_y
    boundary = 'left'
    value = 0.0
  []
  [left_fixed_z]
    type = DirichletBC
    variable = displ_z
    boundary = 'left'
    value = 0.0
  []
  [right_displace_x]
    type = DirichletBC
    variable = displ_x
    boundary = 'right'
    value = 0.0
  []
  [right_displace_y]
    type = FunctionDirichletBC
    variable = displ_y
    boundary = 'right'
    function = ramp
  []
  [right_displace_z]
    type = DirichletBC
    variable = displ_z
    boundary = 'right'
    value = 0.0
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
  # solve_type = 'newton'
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
