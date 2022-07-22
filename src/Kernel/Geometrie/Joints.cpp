/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Joints.h>
Implemente_liste(Joint);
Implemente_instanciable(Joints,"Joints",LIST(Joint));


/*! @brief
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Joints::printOn(Sortie& os) const
{
  return LIST(Joint)::printOn(os);
}


/*! @brief
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Joints::readOn(Entree& is)
{
  return LIST(Joint)::readOn(is);
}


/*! @brief Associe une zone a tous les joints de la liste.
 *
 * Les joints de la liste feront partie de cette zone.
 *
 * @param (Zone& une_zone) la zone a associer aux joint
 */
void Joints::associer_zone(const Zone& une_zone)
{
  LIST_CURSEUR(Joint) curseur(*this);;;
  while(curseur)
    {
      curseur->associer_zone(une_zone);
      ++curseur;
    }
}


/*! @brief Renvoie le nombre de face total des Joints de la liste.
 *
 * (la somme des faces de tous les
 *     joints de la liste).
 *
 * @return (int) le nombre de face total des Joints de la liste
 */
int Joints::nb_faces() const
{
  CONST_LIST_CURSEUR(Joint) curseur(*this);;;
  int nombre=0;
  while(curseur)
    {
      nombre+=curseur->nb_faces();
      ++curseur;
    }
  return nombre;
}

/*! @brief Renvoie le nombre de faces du type specifie contenues dans la liste de joints.
 *
 *     (somme des faces de ce type sur tous les
 *      joints de la liste)
 *
 * @param (Type_Face type) le type des faces a compter
 * @return (int) le nombre de faces du type specifie contenues dans la liste de joints
 */
int Joints::nb_faces(Type_Face type) const
{
  CONST_LIST_CURSEUR(Joint) curseur(*this);
  int nombre=0;
  while(curseur)
    {
      if (type == curseur->faces().type_face())
        nombre+=curseur->nb_faces();
      ++curseur;
    }
  return nombre;
}


/*! @brief Comprime la liste de joints.
 *
 */
void Joints::comprimer()
{
  Cerr << "Joints::comprimer() - Start" << finl;
  IntVect fait(size());
  int rang1=0, rang2=0;
  LIST_CURSEUR(Joint) curseur1(*this);;
  while(curseur1)
    {
      if (!fait(rang1) )
        {
          fait(rang1)=1;
          Joint& joint1=curseur1.valeur();
          rang2=rang1;
          LIST_CURSEUR(Joint) curseur2(curseur1.list());;
          while(curseur2)
            {
              if (!fait(rang2) )
                {
                  Joint& joint2=curseur2.valeur();
                  if(joint1.PEvoisin() == joint2.PEvoisin())
                    {
                      Cerr << "agglomeration of joints " << joint1.le_nom()
                           << " and " << joint2.le_nom() << finl;
                      fait(rang2)=1;
                      joint1.nommer(joint1.le_nom()+joint2.le_nom());

                      // Concatenation des faces
                      joint1.ajouter_faces(joint2.les_sommets_des_faces());

                      // Concatenation des sommets ???
                      Cerr << "Concatenation of nodes ?" << finl;
                      exit();

                      joint2.dimensionner(0);
                    }
                }
              ++curseur2;
              rang2++;
            }
        }
      ++curseur1;
      ++rang1;
    }
  curseur1=*this;
  while(curseur1)
    {
      Joint& joint1=curseur1.valeur();
      if (joint1.nb_faces()==0)
        {
          suppr(curseur1.valeur());
          curseur1=*this;
        }
      else
        ++curseur1;
    }
  Cerr << "Joints::comprimer() - End" << finl;
}

/*! @brief Renvoie le nombre de Joint de la liste
 *
 * @return (int) le nombre de Joint de la liste
 */
int Joints::nb_joints() const
{
  return size();
}

/*! @brief Renvoie une reference sur le joint qui a pour voisin le PE specifie.
 *
 * @param (int pe) un numero de PE
 * @return (Joint&) le joint qui a pour voisin le PE specifie
 * @throws PE voisin specifie non trouve dans la liste des joints.
 */
Joint& Joints::joint_PE(int pe)
{
  LIST_CURSEUR(Joint) curseur(*this);
  for (; curseur; ++curseur)
    {
      Joint& joint = curseur.valeur();
      int pe_joint = joint.PEvoisin();
      if (pe == pe_joint)
        return joint;
    }
  Cerr << "Error in Joints::joint_PE : joint not found" << finl;
  assert(0);
  exit();
  return curseur.valeur();
}

