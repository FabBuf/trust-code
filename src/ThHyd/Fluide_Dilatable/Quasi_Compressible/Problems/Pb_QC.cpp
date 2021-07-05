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
// File:        Pb_QC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Quasi_Compressible/Problems
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Pb_QC.h>
#include <Fluide_Quasi_Compressible.h>
#include <Equation_base.h>
#include <Loi_Fermeture_base.h>
#include <Probleme_Couple.h>
#include <Debog.h>
#include <Domaine.h>

Implemente_base(Pb_QC,"Pb_QC",Pb_Dilatable);

Sortie& Pb_QC::printOn(Sortie& os) const
{
  return Pb_Dilatable::printOn(os);
}

Entree& Pb_QC::readOn(Entree& is)
{
  return Pb_Dilatable::readOn(is);
}

void Pb_QC::associer_milieu_base(const Milieu_base& mil)
{
  if (sub_type(Fluide_Quasi_Compressible,mil))
    {
      Pb_Dilatable::associer_milieu_base(mil);
    }
  else
    {
      Cerr << "Un milieu de type " << mil.que_suis_je()
           << " ne peut etre associe a "<< finl
           << "un probleme quasi-compressible" << finl;
      Process::exit();
    }
}

void Pb_QC::preparer_calcul()
{
  Fluide_Quasi_Compressible& le_fluide = ref_cast(Fluide_Quasi_Compressible,milieu());
  //Le nom sera a nouveau modifie dans Loi_Etat_GR_rhoT::initialiser()
  if (le_fluide.type_fluide()=="Gaz_Reel") equation(1).inconnue()->nommer("temperature");
  if (le_fluide.type_fluide()=="Melange_Binaire") equation(1).inconnue()->nommer("fraction_massique");

  le_fluide.completer(*this);
  le_fluide.preparer_calcul();
  Pb_Dilatable::preparer_calcul();
  le_fluide.calculer_masse_volumique();
  le_fluide.preparer_calcul();
  le_fluide.calculer_masse_volumique(); // XXX : encore utile ?
  le_fluide.preparer_calcul();
}

bool Pb_QC::initTimeStep(double dt)
{
  bool ok = Pb_Dilatable::initTimeStep(dt);
  Fluide_Quasi_Compressible& le_fluide = ref_cast(Fluide_Quasi_Compressible,le_fluide_.valeur());
  le_fluide.preparer_pas_temps();
  return ok;
}

void Pb_QC::solve_pressure_thermo()
{
  Fluide_Quasi_Compressible& le_fluide_QC = ref_cast(Fluide_Quasi_Compressible,le_fluide_.valeur());
  le_fluide_QC.Resoudre_EDO_PT();
}


