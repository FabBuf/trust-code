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
// File:        DoubleList.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////
//
// WARNING: DO NOT EDIT THIS FILE! Only edit the template file DoubleList.h.P
//
//
#ifndef DoubleList_included
#define DoubleList_included

#include <Double.h>
#include <arch.h>
class Sortie;
class Entree;
#include <DoubleListElem.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//      Classe qui sert a representer une liste de reels double
//      precision. On ne peut pas utiliser la classe container
//      List avec des objets du type double car double est un type
//      predefini du C++ qui ne possede pas les fonctions exigees
//      par List< >.
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class DoubleList : public DoubleListElem
{

public :
  friend class DoubleList_Curseur;

  DoubleList(const DoubleList&);
  inline DoubleList();
  Sortie& printOn(Sortie& os) const;
  Entree& readOn(Entree& is);
  int size() const;
  int contient(double) const;
  int rang(double) const;
  DoubleListElem& dernier() ;
  const DoubleListElem& dernier() const;

  double& operator[](int );
  const double& operator[](int ) const;
  DoubleList& add(double ) ;
  DoubleList& add_if_not(double ) ;
  DoubleList& operator=(const DoubleList& );

  friend int operator == (const DoubleList& , const DoubleList& );
  void suppr(double );
  void vide();
  void calcule_min_max();

private:
  DoubleListElem* dernier_;
  double min_data;
  double max_data;
};

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//      List_Curseur de reels double precision
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class DoubleList_Curseur
{
public :
  inline DoubleList_Curseur(const DoubleList& a_list);
  inline DoubleList_Curseur(const DoubleListElem& a_list);
  inline operator int() const;
  inline void operator++();
  inline double valeur() const;
  inline double& valeur();
  inline void operator=(const DoubleList& a_list);
  inline const DoubleListElem& list() const;
  inline DoubleListElem& list();

private :
  DoubleListElem* curseur;
};

// Description:
//    Construit une liste vide
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

inline DoubleList::DoubleList() :
  DoubleListElem(),min_data(DMAXFLOAT),max_data(-DMAXFLOAT)
{
  dernier_=this;
}

// Description:
//    Constructeur
//    Construit une liste_curseur a partir d'une liste
// Precondition:
// Parametre: const DoubleList& list
//    Signification: la liste a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline DoubleList_Curseur::DoubleList_Curseur(const DoubleList& a_list)
  : curseur(&(DoubleListElem&)a_list)
{
  if (a_list.est_vide())
    curseur=0;
}

// Description:
//    Constructeur
//    Construit une liste_curseur a partir d'une liste
// Precondition:
// Parametre: const DoubleList& list
//    Signification: la liste a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline DoubleList_Curseur::DoubleList_Curseur(const DoubleListElem& a_list)
  : curseur(&(DoubleListElem&)a_list)
{
  if (a_list.est_vide())
    curseur=0;
}

// Description:
//    Teste si le curseur est non nul
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si le curseur est non nul, 0 si il est nul
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline DoubleList_Curseur::operator int() const
{
  return (curseur!=0);
}


// Description:
//    Avance le curseur dans la liste
//    Si le curseur est sur le dernier element, il devient nul
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
inline void DoubleList_Curseur::operator++()
{
  if(curseur->est_dernier())
    curseur=0;
  else
    curseur=&(curseur->suivant());
}


// Description:
//    retourne la valeur du curseur
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: valeur du curseur
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double DoubleList_Curseur::valeur() const
{
  return curseur->valeur();
}


// Description:
//    retourne la valeur du curseur
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double&
//    Signification: valeur du curseur
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline double& DoubleList_Curseur::valeur()
{
  return curseur->valeur();
}


// Description:
//    Affectation d'une liste a une liste_curseur
// Precondition:
// Parametre: const DoubleList& list
//    Signification: la liste a affecter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
//    Mise a jour du curseur sur le premier element
// Postcondition:
inline void DoubleList_Curseur::operator=(const DoubleList& a_list)
{
  curseur=(&(DoubleListElem&)a_list);
  if (a_list.est_vide())
    curseur=0;
}


// Description:
//    Retourne la liste associee a la liste_curseur
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: const DoubleList&
//    Signification: la liste associee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline const DoubleListElem& DoubleList_Curseur::list() const
{
  return *curseur;
}


// Description:
//    Retourne la liste associee a la liste_curseur
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleList&
//    Signification: la liste associee
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline DoubleListElem& DoubleList_Curseur::list()
{
  return *curseur;
}

#endif // DoubleList_H

