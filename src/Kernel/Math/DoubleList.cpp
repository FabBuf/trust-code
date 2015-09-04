/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        DoubleList.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////
//
// WARNING: DO NOT EDIT THIS FILE! Only edit the template file DoubleList.cpp.P
//
//
#include <DoubleList.h>
#include <Nom.h>

// Description:
//     Ecriture d'une liste sur un flot de sortie
//     les elements separes par des virgules figurent entre des accolades
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& DoubleList::printOn(Sortie& os) const
{
  DoubleList_Curseur curseur(*this);
  Nom accouverte = "{";
  Nom accfermee = "}";
  Nom virgule = ",";
  os << accouverte << " " ;
  while(curseur)
    {
      // if(est_dernier()) GF sinon on a une virgule de trop
      if(curseur.list().est_dernier())
        os << curseur.valeur() << " " ;
      else
        os << curseur.valeur() << " " << virgule << " " ;
      ++curseur;
    }
  os << accfermee;
  return os;
}



// Description:
//     Lecture d'une liste sur un flot d'entree
//     les elements separes par des virgules figurent entre des accolades
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& DoubleList::readOn(Entree& is)
{
  Nom accouverte = "{";
  Nom accfermee = "}";
  Nom virgule = ",";
  Nom nom;
  double t;
  is >> nom;
  assert(nom==accouverte);
  while(nom != accfermee)
    {
      is >> t;
      add(t);
      is >> nom;
      assert((nom==accfermee)||(nom==virgule));
    }
  return is ;
}

// Description:
//    Constructeur par copie
// Precondition:
// Parametre: const DoubleList& list
//    Signification: la liste a copier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleList::DoubleList(const DoubleList& a_list) : DoubleListElem()
{
  if(a_list.est_vide() )
    {
      suivant_=this;
    }
  else
    {
      data=a_list.data;
      min_data=a_list.min_data;
      max_data=a_list.max_data;
      dernier_=this;
      if(a_list.suivant_)
        {
          DoubleListElem* next = new DoubleListElem(*a_list.suivant_); //Recursif !!
          suivant_ = next;
        }
      else
        suivant_ =0;
    }
}

// Description:
//   Affectation
//   Les elements sont copies
// Precondition:
// Parametre: const DoubleList& list
//    Signification: la liste a copier
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleList&
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleList& DoubleList::operator=(const DoubleList& a_list)
{
  if(a_list.est_vide())
    {
      suivant_=this;
    }
  else
    {
      vide();
      DoubleList_Curseur curseur(a_list);
      while(curseur)
        {
          add(curseur.valeur());
          ++curseur;
        }
    }
  return *this;
}

// Description:
//     Renvoie le dernier element de la liste
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const DoubleList&
//    Signification: le dernier element de la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const DoubleListElem& DoubleList::dernier() const
{
  return *dernier_;
  /*  if (est_dernier()) return *this;
      DoubleList_Curseur curseur(*this);
      while(curseur)
      if (curseur.list().est_dernier()) break;
      else ++curseur;
      return curseur.list(); */
}

// Description:
//     Renvoie le dernier element de la liste
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleList&
//    Signification: le dernier element de la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleListElem& DoubleList::dernier()
{
  return *dernier_;
  /*
    if (est_dernier()) return *this;
    DoubleList_Curseur curseur(*this);
    while(curseur)
    if (curseur.list().est_dernier()) break;
    else ++curseur;
    return curseur.list();
  */
}

// Description:
//    insertion en queue
// Precondition:
// Parametre: double double_to_add
//    Signification: element a ajouter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleList&
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleList& DoubleList::add(double double_to_add)
{
  if (double_to_add<min_data)
    min_data=double_to_add;
  if (double_to_add>max_data)
    max_data=double_to_add;
  if( est_vide())
    {
      data=double_to_add;
      suivant_=0;
      return *this;
    }
  else
    {
      if(est_dernier())
        {
          DoubleListElem* next=new DoubleListElem(double_to_add);
          suivant_ = next;
          dernier_ = next;
        }
      else
        {
          dernier().add(double_to_add);
          dernier_ = &dernier_->suivant();
        }
      return *this;
    }
}

// Description:
//     Renvoie la taille de la liste
//     Une liste vide est de taille nulle
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: nombre d'elements de la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int DoubleList::size() const
{
  if(est_vide())
    return 0;
  int i=0;
  DoubleList_Curseur curseur(*this);
  while(curseur)
    {
      ++i;
      ++curseur;
    }
  return i;
}

// Description:
//     Ajout d'un element a la liste ssi il n'existe pas deja
// Precondition:
// Parametre: double x
//    Signification: l'element a ajouter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleList&
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleList& DoubleList::add_if_not(double x)
{
  if (!contient(x))
    return add(x);
  else
    return *this;
}


// Description:
//     Verifie si un element appartient ou non a la liste
// Precondition:
// Parametre: double x
//    Signification: L'element a rechercher
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si la liste contient l'element, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int DoubleList::contient(double x) const
{
  if(est_vide() || x>max_data || x<min_data)
    return 0;
  DoubleList_Curseur curseur(*this);
  while(curseur)
    {
      if (curseur.valeur()==x)
        return 1;
      ++curseur;
    }
  return 0;
}


// Description:
//     renvoie le rang d'un element dans la liste
//     si un element apparait plusieurs fois, renvoie le rang du premier.
// Precondition:
// Parametre: int i
//    Signification: L'element a rechercher
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le rang du premier element de la liste valant i,
//     -1 si la liste ne contient pas i.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int DoubleList::rang(double x) const
{
  if(est_vide() || x>max_data || x<min_data)
    return -1;
  int compteur=0;
  DoubleList_Curseur curseur(*this);
  while(curseur)
    {
      if (curseur.valeur()==x)
        return compteur;
      ++compteur;
      ++curseur;
    }
  return -1;
}

// Description:
//     Operateur d'acces au ieme int de la liste
// Precondition:
// Parametre: int i
//    Signification: l'indice de l'element a trouver
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double&
//    Signification: le ieme element de la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double& DoubleList::operator[](int i)
{
  DoubleList_Curseur curseur(*this);
  while(curseur && i--)
    ++curseur;
  if(i!=-1)
    {
      Cerr << "Overflow list " << finl;
      Process::exit();
    }
  return curseur.valeur();
}

// Description:
//     Operateur d'acces au ieme int de la liste
// Precondition:
// Parametre: int i
//    Signification: l'indice de l'element a trouver
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const double&
//    Signification: le ieme element de la liste
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const double& DoubleList::operator[](int i) const
{
  DoubleList_Curseur curseur(*this);
  while(curseur && i--)
    ++curseur;
  if(i!=-1)
    {
      Cerr << "Overflow list " << finl;
      Process::exit();
    }
  return curseur.valeur();
}

// Description:
//     Operateur de comparaison de deux listes
// Precondition:
// Parametre: const DoubleList& list1
//    Signification: premiere liste a comparer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: const DoubleList& list2
//    Signification: seconde liste a comparer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si les listes sont egales, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int operator ==(const DoubleList& list1 , const DoubleList& list2)
{
  int retour=1;
  if(list1.data != list2.data)
    retour= 0;
  if( (!list1.est_dernier()) && (list2.est_dernier()) )
    retour= 0;
  if( (list1.est_dernier()) && (!list2.est_dernier()) )
    retour= 0;
  if( (!list1.est_dernier()) && (!list2.est_dernier()) )
    retour= (*list1.suivant_ == *list2.suivant_);
  return retour;
}

// Description:
//     Supprime un element contenu dans la liste
// Precondition:
// Parametre: double obj
//    Signification: l'element a supprimer de la liste
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void DoubleList::suppr(double obj)
{
  if(valeur()==obj)
    {
      if(suivant_)
        {
          DoubleListElem* next=suivant_;
          suivant_=next->suivant_;
          data=next->valeur();
          next->suivant_=0;
          delete next;
        }
      else
        {
          suivant_=this;
          dernier_=this;
        }
      calcule_min_max();
      return;
    }
  DoubleList_Curseur curseur_pre=*this;
  DoubleList_Curseur curseur=*suivant_;
  while(curseur)
    {
      if(curseur.valeur()==obj)
        {
          DoubleListElem* next=&curseur_pre.list().suivant();
          curseur_pre.list().suivant_=curseur.list().suivant_;
          if (next->suivant_==0)
            dernier_=&curseur_pre.list();
          else
            next->suivant_=0;
          delete next;
          calcule_min_max();
          return ;
        }
      ++curseur;
      ++curseur_pre;
    }
  Cerr << "WARNING during deletion of an element in a list " << finl;
  Cerr << "One has not found object == : " << obj << finl;
}

void DoubleList::calcule_min_max()
{
  min_data=DMAXFLOAT;
  max_data=-DMAXFLOAT;
  DoubleList_Curseur curseur=*this;
  while(curseur)
    {
      double la_valeur=curseur.valeur();
      if (la_valeur<min_data)
        min_data=la_valeur;
      if (la_valeur>max_data)
        max_data=la_valeur;
      ++curseur;
    }
}

// Description:
//     Vide la liste
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void DoubleList::vide()
{
  if (!est_vide())
    if(suivant_)
      delete suivant_;
  suivant_=this;
  dernier_=this;
  calcule_min_max();
}

