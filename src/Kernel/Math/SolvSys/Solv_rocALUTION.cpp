/****************************************************************************
* Copyright (c) 2021, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        Solv_rocALUTION.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math/SolvSys
// Version:     /main/0
//
//////////////////////////////////////////////////////////////////////////////

#include <Solv_rocALUTION.h>
#include <Matrice_Morse.h>
#include <Matrice_Morse_Sym.h>
#include <DoubleVect.h>
#include <EChaine.h>
#include <Motcle.h>
#include <SFichier.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(Solv_rocALUTION,"Solv_rocALUTION",SolveurSys_base);

// printOn
Sortie& Solv_rocALUTION::printOn(Sortie& s ) const
{
  s << chaine_lue_;
  return s;
}

// readOn
Entree& Solv_rocALUTION::readOn(Entree& is)
{
  create_solver(is);
  return is;
}

Solv_rocALUTION::Solv_rocALUTION() {
  initialize();
}

Solv_rocALUTION::Solv_rocALUTION(const Solv_rocALUTION& org)
{
  initialize();
  // on relance la lecture ....
  EChaine recup(org.get_chaine_lue());
  readOn(recup);
}

Solv_rocALUTION::~Solv_rocALUTION() {
    if (ls!=nullptr) {
        ls->Clear();
        delete ls;
    }
    if (p!=nullptr)
    {
        p->Clear();
        delete p;
    }
}

void Solv_rocALUTION::initialize()
{
    ls = nullptr; p = nullptr;
}

// Lecture et creation du solveur
void Solv_rocALUTION::create_solver(Entree& entree)
{
  // Valeurs par defaut:
  atol_ = 1.e-12;
  rtol_ = 1.e-12;
  double div_tol = 1e3;

#ifdef ROCALUTION_ROCALUTION_HPP_
  lecture(entree);
  EChaine is(get_chaine_lue());

  Motcle accolade_ouverte("{"), accolade_fermee("}");
  Nom solver, precond, motlu;
  is >> solver;   // On lit le solveur en premier puis les options du solveur: PETSC ksp { ... }
  is >> motlu; // On lit l'accolade
  if (motlu != accolade_ouverte)
    {
      Cerr << "Error while reading the parameters of the solver " << solver << " { ... }" << finl;
      Cerr << "We expected " << accolade_ouverte << " instead of " << motlu << finl;
      exit();
    }
  Motcles les_solveurs(2);
  {
    les_solveurs[0] = "GCP";
    les_solveurs[1] = "GMRES";
  }
  int rang=les_solveurs.search(solver);
  switch(rang)
    {
    case 0:
      {
        // Linear Solver
        ls = new CG<LocalMatrix<double>, LocalVector<double>, double>();
        break;
      }
    case 1:
      {
        break;
      }
    default:
      {
        Cerr << solver << " solver not recognized for rocALUTION:" << finl << les_solveurs << finl;
        Process::exit();
      }
    }
  // Lecture des parametres du solver:
  Motcles les_parametres_solveur(5);
  {
    les_parametres_solveur[0] = "impr";
    les_parametres_solveur[1] = "seuil"; // Seuil absolu (atol)
    les_parametres_solveur[2] = "atol";
    les_parametres_solveur[3] = "rtol";
    les_parametres_solveur[4] = "precond";
  }
  is >> motlu;
  while (motlu!=accolade_fermee)
    {
      switch (les_parametres_solveur.search(motlu))
        {
        case 0:
          {
            fixer_limpr(1);
            break;
          }
        case 1:
        case 2:
          {
            is >> atol_;
            break;
          }
        case 3:
          {
            is >> rtol_;
            break;
          }
        default:
          {
            Cerr << solver << " keyword not recognized for solver:" << finl << les_parametres_solveur << finl;
            Process::exit();
          }
        }
        is >> motlu;
    }
    // Preconditioner
    p = new Jacobi<LocalMatrix<double>, LocalVector<double>, double>();
    //p = new GS<LocalMatrix<double>, LocalVector<double>, double>();
    //p->FlagPrecond();
    ls->SetPreconditioner(*p);
    // Tolerances (ToDo read in data file)
    ls->InitTol(atol_, rtol_, div_tol);
    //ls->InitMaxIter(500);
    //ls->InitMinIter(20);
#endif
}

// ToDo remonter dans Matrice_Morse
// Urgent de faire une reconception d'une classe mere de Solv_Petsc et Solv_rocALUTION qui factoriser
// les conversions de matrice...
void MorseSymToMorseToMatrice_Morse(const Matrice_Morse_Sym& MS, Matrice_Morse& M)
{
  M = MS;
  Matrice_Morse mattmp(MS);
  M.transpose(mattmp);
  int ordre = M.ordre();
  for (int i=0; i<ordre; i++)
    if (M.nb_vois(i))
      M(i, i) = 0.;
  M = mattmp + M;
}

void write_matrix(const Matrice_Base& a)
{
    // Save matrix
    if (Process::nproc() > 1) Process::exit("Error, matrix market format is not available yet in parallel.");
    Nom filename(Objet_U::nom_du_cas());
    filename += "_trust_matrix";
    filename += ".mtx";
    SFichier mtx(filename);
    mtx.precision(14);
    mtx.setf(ios::scientific);
    int rows = a.nb_lignes();
    mtx << "%%MatrixMarket matrix coordinate real " << (sub_type(Matrice_Morse_Sym, a) ? "symmetric" : "general") << finl;
    Cerr << "Matrix (" << rows << " lines) written into file: " << filename << finl;
    mtx << "%%matrix" << finl;
    Matrice_Morse csr = ref_cast(Matrice_Morse, a);
    mtx << rows << " " << rows << " " << csr.get_tab1()[rows] << finl;
    for (int row=0; row<rows; row++)
        for (int j=csr.get_tab1()[row]; j<csr.get_tab1()[row+1]; j++)
            mtx << row+1 << " " << csr.get_tab2()[j-1] << " " << csr.get_coeff()[j-1] << finl;
}

void write_matrix(const LocalMatrix<double>& mat)
{
    Nom filename2(Objet_U::nom_du_cas());
    filename2 += "_rocalution_matrix";
    filename2 += ".mtx";
    Cout << "Writing rocALUTION matrix into " << filename2 << finl;
    mat.WriteFileMTX(filename2.getString());
}

void check(const DoubleVect& t, LocalVector<double>& r, const Nom& nom)
{
    double norm_t = t.mp_norme_vect();
    double norm_r = r.Norm();
    double difference = std::abs((norm_t - norm_r)/(norm_t+norm_r+DMINFLOAT));
    if (difference>1.e-8)
    {
        Cerr << nom << " Trust " << norm_t << " rocALUTION " << norm_t << " difference " << difference << finl;
        Process::exit();
    }
}
double residual(const Matrice_Base& a, const DoubleVect& b, const DoubleVect& x)
{
    DoubleVect e(b);
    e*=-1;
    a.ajouter_multvect(x, e);
    return e.mp_norme_vect();
}
int Solv_rocALUTION::resoudre_systeme(const Matrice_Base& a, const DoubleVect& b, DoubleVect& x)
{
//#ifdef ROCALUTION_ROCALUTION_HPP_
  double tick;
  if (nouvelle_matrice()) {
      // Build matrix
      tick = rocalution_time();

      // Conversion matrice stockage symetrique vers matrice stockage general:
      Matrice_Morse csr;
      MorseSymToMorseToMatrice_Morse(ref_cast(Matrice_Morse_Sym, a), csr);
      // Save TRUST matrix to check:
      //write_matrix(a);

      int N = csr.get_tab1().size_array() - 1;
      ArrOfInt tab1_c(N+1); // Passage Fortran->C
      for (int i = 0; i < N+1; i++)
          tab1_c(i) = csr.get_tab1()[i] - 1;

      int nnz = csr.get_coeff().size_array();
      ArrOfInt tab2_c(nnz); // Passage Fortran->C
      for (int i = 0; i < nnz; i++)
          tab2_c(i) = csr.get_tab2()[i] - 1;

      // Copie de la matrice TRUST dans une matrice rocALUTION
      Cout << "Build a rocALUTION matrix with N= " << N << " and nnz=" << nnz << finl;
      mat.AllocateCSR("a", nnz, N, N);
      mat.CopyFromCSR(tab1_c.addr(), tab2_c.addr(), csr.get_coeff().addr());
      mat.Sort();
      Cout << "[rocALUTION] Time to build matrix :" << (rocalution_time() - tick) / 1e6 << finl;

      tick = rocalution_time();
      ls->Clear();
      mat.MoveToAccelerator(); // Important: move mat to device so after ls is built on device
      ls->SetOperator(mat);
      ls->Build();
      mat.Info();
      ls->Print();
      p->Print();
      Cout << "[rocALUTION] Time to build solver on device :" << (rocalution_time() - tick) / 1e6 << finl;

      // Save rocALUTION matrix to check
      // write_matrix(mat);
  }
  int N = mat.GetN();
  assert(N==b.size_array());
  assert(N==x.size_array());

  // Build rhs and initial solution:
  LocalVector<double> sol;
  LocalVector<double> rhs;
  sol.Allocate("a", N);
  sol.CopyFromData(x.addr());
  sol.MoveToAccelerator();
  rhs.Allocate("rhs", N);
  rhs.CopyFromData(b.addr());
  rhs.MoveToAccelerator();

  sol.Info();

  // Check before solves:
  rhs.Check();
  sol.Check();
  mat.Check();
  check(x, sol, "Before ||x||");
  check(b, rhs, "Before ||b||");
  // Petit bug rocALUTION (division par 0 si rhs nul, on contourne en mettant la verbosity a 0)
  ls->Verbose(limpr() && rhs.Norm()>0 ? 2 : 0);

  // Solve A x = rhs
  tick = rocalution_time();
  double res_initial =residual(a, b, x);
  ls->Solve(rhs, &sol);

  if (ls->GetSolverStatus()==3) Process::exit("Divergence for solver.");
  if (ls->GetSolverStatus()==4) Cout << "Maximum number of iterations reached." << finl;

  Cout << "[rocALUTION] Time to solve: " << (rocalution_time() - tick) / 1e6 << finl;

  int nb_iter = ls->GetIterationCount();
  //double res_final = ls->GetCurrentResidual();

  // Recupere la solution
  sol.MoveToHost();
  sol.CopyToData(x.addr());
  check(x, sol, "After ||x||");
  check(b, rhs, "After ||b||");

  // Check residual e=||Ax-rhs|| with TRUST
  double res_final = residual(a, b, x);
  if (res_final>atol_)
  {
      Cerr << "Solution not correct !" << finl;
      Cout << "||Ax-b|| = " << res_final << finl;
      Process::exit();
  }
  if (limpr()>-1)
  {
      double residu_relatif=(res_initial>0?res_final/res_initial:res_final);
      Cout << finl << "Final residue: " << res_final << " ( " << residu_relatif << " )"<<finl;
  }
  fixer_nouvelle_matrice(0);
  sol.Clear();
  rhs.Clear();
  return nb_iter;
/* #else
    return -1;
#endif */
}