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
// File:        Champ_Fonc_Tabule_P0_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_Tabule_P0_CoviMAC.h>
#include <Zone_CoviMAC.h>
#include <Champ_Inc.h>
#include <Table.h>

Implemente_instanciable(Champ_Fonc_Tabule_P0_CoviMAC,"Champ_Fonc_Tabule_P0_CoviMAC",Champ_Fonc_P0_CoviMAC);

// printOn


Sortie& Champ_Fonc_Tabule_P0_CoviMAC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

// readOn

Entree& Champ_Fonc_Tabule_P0_CoviMAC::readOn(Entree& s)
{
  return s ;
}

void Champ_Fonc_Tabule_P0_CoviMAC::associer_param(const VECT(REF(Champ_base))& les_champs,
                                                  const Table& une_table)
{
  les_ch_param = les_champs;
  // for (int i = 0; i < les_champs.size(); i++)
  //   {
  //     REF(Champ_base) champ;
  //     champ = les_champs[i];
  //     les_ch_param.add(champ);
  //   }
  la_table = une_table;
}


void Champ_Fonc_Tabule_P0_CoviMAC::mettre_a_jour(double t)
{
  const Zone_CoviMAC& zone_CoviMAC = la_zone_CoviMAC.valeur();
  const Table& table = la_table.valeur();
  DoubleTab& mes_valeurs = valeurs();
  const int nb_elem = zone_CoviMAC.nb_elem(), nb_elem_tot = zone_CoviMAC.nb_elem_tot(), nb_param = les_ch_param.size();
  VECT(DoubleTab) val_params_aux_elems;
  for (int i = 0; i < nb_param; i++)
    {
      DoubleTab vp(nb_elem_tot, mes_valeurs.dimension(1));
      val_params_aux_elems.add(vp);
    }
  const DoubleTab& centres_de_gravites = zone_CoviMAC.xp();
  IntVect les_polys(nb_elem_tot);
  for(int elem = 0; elem < nb_elem_tot; elem++) les_polys(elem) = elem;

  // Estimate the field parameter on cells:
  for (int i = 0; i < nb_param; i++)
    les_ch_param[i].valeur().valeur_aux_elems(centres_de_gravites, les_polys, val_params_aux_elems[i]);
  // Compute the field according to the parameter field
  if (table.isfonction() != 2)
    {
      const int nbcomp = mes_valeurs.dimension(1);
      for (int num_elem = 0; num_elem < nb_elem; num_elem++)
        for (int ncomp = 0; ncomp < nbcomp; ncomp++)
          {
            std::vector<double> vals;
            for (int n = 0; n < nb_param; n++) vals.push_back(val_params_aux_elems[n](num_elem, ncomp));
            mes_valeurs(num_elem, ncomp) = table.val(vals, ncomp);
          }
    }
  else
    {
      abort();
      table.valeurs(val_params_aux_elems[0], centres_de_gravites, t, mes_valeurs);
    }

  Champ_Fonc_base::mettre_a_jour(t);
}

int Champ_Fonc_Tabule_P0_CoviMAC::initialiser(const double&)
{
  Champ_Fonc_Tabule_P0_CoviMAC::mettre_a_jour(temps());
  return 1;
}
