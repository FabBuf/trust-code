/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Echange_contact_VDF.h>
#include <Op_Diff_VDF_base.h>
#include <Champ_front_calc.h>
#include <Eval_Diff_VDF.h>
#include <Pb_Multiphase.h>
#include <TRUSTTrav.h>
#include <Champ_Don.h>
#include <Operateur.h>
#include <Motcle.h>

Implemente_base(Op_Diff_VDF_base, "Op_Diff_VDF_base", Operateur_Diff_base);

Sortie& Op_Diff_VDF_base::printOn(Sortie& s) const { return s << que_suis_je(); }
Entree& Op_Diff_VDF_base::readOn(Entree& s) { return s; }

void Op_Diff_VDF_base::completer()
{
  Operateur_base::completer();
  // Certains operateurs (Axi) n'ont pas d'iterateurs en VDF... Encore une anomalie dans la conception a corriger un jour !
  if (iter.non_nul())
    {
      iter->completer_();
      const Champ_Inc_base& cc = le_champ_inco.non_nul() ? le_champ_inco->valeur() : equation().inconnue();
      iter->associer_champ_convecte_ou_inc(cc, nullptr);
      iter->set_name_champ_inco(le_champ_inco.non_nul() ? nom_inconnue() : cc.le_nom().getString());
      iter->set_convective_op_pb_type(false /* diff op */, sub_type(Pb_Multiphase, equation().probleme()));
    }
}

int Op_Diff_VDF_base::impr(Sortie& os) const
{
  // Certains operateurs (Axi) n'ont pas d'iterateurs en VDF... Encore une anomalie dans la conception a corriger un jour !
  return (iter.non_nul()) ? iter->impr(os) : 0;
}

/*! @brief calcule la contribution de la diffusion, la range dans resu
 *
 */
DoubleTab& Op_Diff_VDF_base::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  resu = 0.;
  return ajouter(inco, resu);
}

void Op_Diff_VDF_base::init_op_ext() const
{
  const Domaine_VDF& zvdf = iter->domaine();
  const Domaine_Cl_VDF& zclvdf = iter->domaine_Cl();
  op_ext = { this };      //le premier op_ext est l'operateur local

  for (int n_bord = 0; n_bord < zvdf.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zclvdf.les_conditions_limites(n_bord);
      if (sub_type(Echange_contact_VDF, la_cl.valeur()))
        {
          const Echange_contact_VDF& cl = ref_cast(Echange_contact_VDF, la_cl.valeur());
          const Champ_front_calc& ch = ref_cast(Champ_front_calc, cl.T_autre_pb().valeur());
          const Equation_base& o_eqn = ch.equation();
          const Op_Diff_VDF_base *o_diff = &ref_cast(Op_Diff_VDF_base, o_eqn.operateur(0).l_op_base());

          if (std::find(op_ext.begin(), op_ext.end(), o_diff) == op_ext.end())
            op_ext.push_back(o_diff);
        }
    }
  op_ext_init_ = 1;
}

// Ajout du terme supplementaire en V/(R*R) dans le cas des coordonnees axisymetriques
void Op_Diff_VDF_base::ajoute_terme_pour_axi(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  if (equation().domaine_application() == Motcle("Hydraulique")) // On est dans le cas des equations de Navier_Stokes
    {
      const std::string& nom_inco = equation().inconnue().le_nom().getString();
      Matrice_Morse* mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL;
      const DoubleTab& inco = semi_impl.count(nom_inco) ? semi_impl.at(nom_inco) : equation().inconnue().valeur().valeurs();

      if (Objet_U::bidim_axi == 1)
        {
          const Domaine_VDF& zvdf = iter->domaine();
          const DoubleTab& xv = zvdf.xv();
          const IntVect& ori = zvdf.orientation();
          const IntTab& face_voisins = zvdf.face_voisins();
          const DoubleVect& volumes_entrelaces = zvdf.volumes_entrelaces();
          int face, nb_faces = zvdf.nb_faces();      //, cst;
          DoubleTrav diffu_tot(zvdf.nb_elem_tot());
          double db_diffusivite;
          Nom nom_eq = equation().que_suis_je();
          if ((nom_eq == "Navier_Stokes_standard") || (nom_eq == "Navier_Stokes_QC") || (nom_eq == "Navier_Stokes_FT_Disc"))
            {
              const Eval_Diff_VDF& eval = dynamic_cast<const Eval_Diff_VDF&>(iter->evaluateur());
              const Champ_base& ch_diff = eval.get_diffusivite();
              const DoubleTab& tab_diffusivite = ch_diff.valeurs();

              if (tab_diffusivite.size() == 1) diffu_tot = tab_diffusivite(0, 0);
              else diffu_tot = tab_diffusivite;

              for (face = 0; face < nb_faces; face++)
                if (ori(face) == 0)
                  {
                    const int elem1 = face_voisins(face, 0), elem2 = face_voisins(face, 1);

                    if (elem1 == -1) db_diffusivite = diffu_tot(elem2);
                    else if (elem2 == -1) db_diffusivite = diffu_tot(elem1);
                    else db_diffusivite = 0.5 * (diffu_tot(elem2) + diffu_tot(elem1));

                    double r = xv(face, 0);
                    if (r >= 1.e-24)
                      {
                        if (mat) (*mat)(face, face) += db_diffusivite * volumes_entrelaces(face) / (r * r);
                        secmem(face) -= inco(face) * db_diffusivite * volumes_entrelaces(face) / (r * r);
                      }
                  }
            }
          else if (equation().que_suis_je() == "Navier_Stokes_Interface_avec_trans_masse" || equation().que_suis_je() == "Navier_Stokes_Interface_sans_trans_masse"
                   || equation().que_suis_je() == "Navier_Stokes_Front_Tracking" || equation().que_suis_je() == "Navier_Stokes_Front_Tracking_BMOL" /* c'est quoi ce truc ???? un truc de 1900 ? */)
            {
              /* Voir le terme source axi dans Interfaces/VDF */
            }
          else
            {
              Cerr << "Probleme dans Op_Diff_VDF_base::ajoute_terme_pour_axi  avec le type de l'equation" << finl;
              Cerr << "on n'a pas prevu d'autre cas que Navier_Stokes_std" << finl;
              Process::exit();
            }
        }
    }
}
