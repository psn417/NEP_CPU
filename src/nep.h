/*
    Copyright 2017 Zheyong Fan, Ville Vierimaa, Mikko Ervasti, and Ari Harju
    This file is part of GPUMD.
    GPUMD is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    GPUMD is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with GPUMD.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <string>
#include <vector>

class NEP3
{
public:
  struct ParaMB {
    int version = 2;
    double rc_radial = 0.0;
    double rc_angular = 0.0;
    double rcinv_radial = 0.0;
    double rcinv_angular = 0.0;
    int n_max_radial = 0;
    int n_max_angular = 0;
    int L_max = 0;
    int dim_angular;
    int num_L;
    int basis_size_radial = 8;
    int basis_size_angular = 8;
    int num_types_sq = 0;
    int num_c_radial = 0;
    int num_types = 0;
    double q_scaler[140];
  };

  struct ANN {
    int dim = 0;
    int num_neurons1 = 0;
    int num_para = 0;
    const double* w0;
    const double* b0;
    const double* w1;
    const double* b1;
    const double* c;
  };

  struct ZBL {
    bool enabled = false;
    double rc_inner = 1.0;
    double rc_outer = 2.0;
    double atomic_numbers[10];
  };

  NEP3(const std::string& potential_filename);

  // type[num_atoms] should be integers 0, 1, ..., mapping to the atom types in nep.txt in order
  // box[9] is ordered as ax, bx, cx, ay, by, cy, az, bz, cz
  // position[num_atoms * 3] is ordered as x[num_atoms], y[num_atoms], z[num_atoms]
  // potential[num_atoms]
  // force[num_atoms * 3] is ordered as fx[num_atoms], fy[num_atoms], fz[num_atoms]
  // virial[num_atoms * 9] is ordered as v_xx[num_atoms], v_xy[num_atoms], v_xz[num_atoms],
  // v_yx[num_atoms], v_yy[num_atoms], v_yz[num_atoms], v_zx[num_atoms], v_zy[num_atoms],
  // v_zz[num_atoms]
  // descriptor[num_atoms * dim] is ordered as d0[num_atoms], d1[num_atoms], ...

  void compute(
    const std::vector<int>& type,
    const std::vector<double>& box,
    const std::vector<double>& position,
    std::vector<double>& potential,
    std::vector<double>& force,
    std::vector<double>& virial);

  void find_descriptor(
    const std::vector<int>& type,
    const std::vector<double>& box,
    const std::vector<double>& position,
    std::vector<double>& descriptor);

  void find_latent_space(
    const std::vector<int>& type,
    const std::vector<double>& box,
    const std::vector<double>& position,
    std::vector<double>& latent_space);

  void compute_for_lammps(
    const int N,        // number of atoms in the current processor
    const int* ilist,   // ilist[ii] = i is the ii-th atom in the current processor
    const int* NN,      // NN[i] is the number of neighbors of atom i
    const int** NL,     // NL[i][jj] = j is the jj-th neighbor of atom i
    const int* type,    // type[i] - 1 will be the type of atom i used by NEP
    const double** pos, // pos[i][0-2] = x, y, and z of atom i
    double* potential,  // potential[i] = potential energy of atom i
    double** force,     // force[i][0-2] = fx, fy, and fz of atom i
    double** virial     // virial[i][0-8] = vxx, vxy, vxz, vyx, vyy, vyz, vzx, vzy, vzz of atom i
  );

  int num_atoms = 0;
  int num_cells[3];
  double ebox[18];
  ParaMB paramb;
  ANN annmb;
  ZBL zbl;
  std::vector<int> NN_radial, NL_radial, NN_angular, NL_angular;
  std::vector<double> r12;
  std::vector<double> Fp;
  std::vector<double> sum_fxyz;
  std::vector<double> parameters;
  void update_potential(const double* parameters, ANN& ann);
  void allocate_memory(const int N);
};
