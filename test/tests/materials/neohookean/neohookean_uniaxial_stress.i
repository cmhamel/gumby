[Mesh]
  [generated_mesh]
    type = GeneratedMeshGenerator
    dim = 3
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
  [node_set]
    type = BoundingBoxNodeSetGenerator
    input = assign_block
    bottom_left = '0 0 0'
    top_right = '0 0 0'
    new_boundary = 'corner'
  []
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
    base_name = 'rubber'
    bulk_modulus = 1000.0e6
    shear_modulus = 1.0e6
  []
[]

[Functions]
  [ramp]
    type = ParsedFunction
    value = '2 * t'
  []
[]

[BCs]
  [./GumbyFixedDisplacementBC]
    [./corner_fixed_xyz]
      components = 'x y z'
      boundary = 'corner'
    [../]
    [./left_fixed_x]
      components = 'x'
      boundary = 'left'
    [../]
    [./bottom_fixed_y]
      components = 'y'
      boundary = 'bottom'
    [../]
    [./back_fixed_z]
      components = 'z'
      boundary = 'back'
    [../]
  [../]
  [./right_displace_x]
    type = FunctionDirichletBC
    variable = displ_x
    boundary = 'right'
    function = ramp
  [../]
[]

[GumbyOutputElementVariables]
  base_names = 'rubber'
  blocks = '1'
  deformation_gradient = 'F_field'
  pk1_stress = 'P_field'
[]

[Postprocessors]
  [./F_xx]
    type = PointValue
    point = '0 0 0'
    variable = F_field_xx
  [../]
  [./F_yy]
    type = PointValue
    point = '0 0 0'
    variable = F_field_yy
  [../]
  [./F_zz]
    type = PointValue
    point = '0 0 0'
    variable = F_field_zz
  [../]
  [./P_xx]
    type = PointValue
    point = '0 0 0'
    variable = P_field_xx
  [../]
  [./P_yy]
    type = PointValue
    point = '0 0 0'
    variable = P_field_yy
  [../]
  [./P_zz]
    type = PointValue
    point = '0 0 0'
    variable = P_field_zz
  [../]
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  solve_type = 'newton'
  start_time = 0.0
  end_time = 1.0
  dt = 0.01
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-10
  l_max_its = 100
  nl_max_its = 250
[]

[Outputs]
  exodus = true
  file_base = neohookean_uniaxial_stress_out
  perf_graph = true
  print_linear_residuals = false
  execute_on = 'INITIAL TIMESTEP_END'
  [./csv]
    type = CSV
  [../]
[]
