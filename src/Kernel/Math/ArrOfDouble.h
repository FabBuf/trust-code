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
// File:        ArrOfDouble.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////
//
// WARNING: DO NOT EDIT THIS FILE! Only edit the template file ArrOfDouble.h.P
//
//

#ifndef ArrOfDouble_included
#define ArrOfDouble_included

#include <Array_base.h>
#include <Double.h>

class FArrOfDouble;
class VDoubledata;

/////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//  Represente un tableau d'elements de type double.
//
//  L'etat du tableau est caracterise par la valeur de p_ et de data_ :
//
//  * detache : on a alors p_==0, data_==0, size_array_==0
//     (etat obtenu par le constructeur par defaut, detach_array()
//      et resize_array(0) dans certains cas)
//
//  * normal : alors p pointe vers une structure Vdbldata, data_==p->data_
//    et size_array_ <= p->size_. A la destruction du tableau, si
//    p->ref_count_ est nul on libere la memoire.
//    data_ n'est pas nul.
//    L'espace memoire du tableau peut etre partage entre plusieurs
//    ArrOfDouble (le pointeur "p" pointe alors sur une meme structure).
//    Ce type de tableau est produit par ref_array().
//    Le dernier ArrOfDouble a utiliser une zone de memoire
//    la detruit lorsqu'il a fini (voir detach_array())
//    Si smart_resize_==0, alors on a toujours size_array_==p->size_.
//
//  * ref_data : p_==0, data_ pointe vers un tableau existant, la memoire
//    n'est pas liberee a la destruction du ArrOfDouble.
//    Ces tableaux sont obtenus par appel a ref_data(...) et peuvent servir
//    pour importer un tableau fourni par une routine exterieure (C ou fortran)
//
//  Valeur initiale par defaut : voir "fill_default_value".
//  Priere de supposer desormais que les tableaux ne sont pas initialises
//  par defaut.
//
// .SECTION voir aussi
/////////////////////////////////////////////////////////////////////////

class ArrOfDouble : public Array_base
{

  Declare_instanciable_sans_constructeur_ni_destructeur(ArrOfDouble);
public:
  //
  // Destructeur
  //
  ~ArrOfDouble();
  //
  // Constructeurs
  //
  ArrOfDouble();
  ArrOfDouble(int size);
  ArrOfDouble(int size, double initial_value);
  // Constructeur par copie : deep copy (on duplique les donnees)
  ArrOfDouble(const ArrOfDouble& );
  //
  // Methodes de construction tardive (on cree un tableau vide avec ArrOfDouble()
  // puis on appelle ces methodes pour modifier les caracteristiques du tableau :
  // Change le nombre d'elements du tableau
  inline  void resize_array(int new_size, Array_base::Resize_Options opt = COPY_INIT);
  inline  void resize(int new_size, Array_base::Resize_Options opt = COPY_INIT);
  virtual void resize_tab(int n, Array_base::Resize_Options opt = COPY_INIT);

  // Methodes de gestion de l'allocation memoire:
  // Assigne une valeur au drapeau "smart_resize"
  // (reallocation uniquement si la taille augmente)
  void    set_smart_resize(int flag);
  // Gestion du type de memoire alouee (standard ou pool de memoire Trio-U)
  enum    Storage { STANDARD, TEMP_STORAGE };
  void    set_mem_storage(const Storage storage);
  Storage get_mem_storage() const;

  // Construction de tableaux qui pointent vers des donnees existantes
  // !!! Utiliser ref_data avec precaution (attention a size_array_)
  virtual void ref_data(double* ptr, int size);
  virtual void ref_array(ArrOfDouble&, int start = 0, int sz = -1);
  // Operateur copie
  ArrOfDouble& operator=(const ArrOfDouble&);
  // Remise du tableau dans l'etat initial (obtenu par le constructeur par defaut)
  virtual void reset();

  //
  // Methodes d'acces aux donnees et aux caracteristiques du tableau
  //
  // Remplit le tableau avec la valeur en parametre
  inline ArrOfDouble& operator=(double valeur);
  inline       double& operator[](int i);
  inline const double& operator[](int i) const;
  inline       double& operator()(int i);
  inline const double& operator()(int i) const ;

  // Ces methodes renvoient un pointeur vers le premier element du tableau.
  const double * addr() const;
  double * addr();
  // Renvoie le nombre d'elements du tableau (et non la taille allouee)
  inline int size_array() const;
  // Renvoie le nombre de tableaux qui pointent vers la stucture "*p_"
  int ref_count() const;
  // Ajoute une case en fin de tableau et y stocke la "valeur"
  inline void   append_array(double valeur);

  //
  // Operateurs divers
  //
  ArrOfDouble& operator+=(const ArrOfDouble&);
  ArrOfDouble& operator+=(const double);
  ArrOfDouble& operator-=(const ArrOfDouble&);
  ArrOfDouble& operator-=(const double);
  ArrOfDouble& inject_array(const ArrOfDouble& source,
                            int nb_elements = -1,
                            int first_element_dest = 0,
                            int first_element_source = 0);
  ArrOfDouble& copy_array(const ArrOfDouble&);

  ArrOfDouble& operator=(const FArrOfDouble&);
  ArrOfDouble& operator*= (const double) ;
  ArrOfDouble& operator/= (const double) ;

  void        ordonne_array();
  void        array_trier_retirer_doublons();

protected:
  //
  // Methodes accessibles depuis les descendants de ArrOfDouble
  //
  void   attach_array(const ArrOfDouble& a, int start = 0, int size = -1);
  int detach_array();
  void   fill_default_value(Array_base::Resize_Options opt, int first, int nb);
  inline void resize_array_(int n, Array_base::Resize_Options opt = COPY_INIT);
private:
  // B. Mathieu 22/06/2004 : je mets ces membres "private" pour forcer
  // le passage par les accesseurs dans les classes derivees, au cas ou
  // on voudrait modifier l'implementation.

  // Zone de memoire contenant les valeurs du tableau.
  // Pointeur nul => le tableau est "detache" ou "ref_data"
  // Pointeur non nul => le tableau est "normal"
  VDoubledata* p_;

  // Pointeur vers le premier element du tableau=
  // Pointeur nul => le tableau est "detache".
  // Pointeur non nul => le tableau est "normal" ou "ref_data"
  //  Si p_ est non nul, data_ pointe quelque part a l'interieur de la zone allouee
  //  (en general au debut, sauf si le tableau a ete initialise
  //   avec ref() ou attach_array() avec start > 0)
  double*   data_;

  // Nombre d'elements du tableau (inferieur ou egal a memory_size_).
  // Si le tableau est "detache", alors size_array_=0
  int    size_array_;
  // Taille memoire reellement allouee pour le tableau
  // (pour le mecanisme smart_resize_). memory_size_ est nul
  // si le tableau est de type "ref_data". Sinon memory_size_
  // est egal a p_->size_.
  int    memory_size_;

  // Drapeau indiquant si on applique une strategie d'allocation
  // preventive (la memoire alouee augmente d'un facteur constant
  // si la taille devient insuffisante).
  // Si smart_resize_ == 0, alors on a toujours p_->size_ == size
  int    smart_resize_;

  // Drapeau indiquant si l'allocation memoire a lieu avec un new classique
  // ou dans le pool de memoire temporaire de Trio
  Storage   storage_type_;

  // Partie non inline de resize_array():
  void memory_resize(int new_size, Array_base::Resize_Options opt = COPY_INIT);
};

//
// Declarations des fonctions non membres de la classe ArrOfDouble
//
int operator==(const ArrOfDouble& x, const ArrOfDouble& y) ;
int imin_array(const ArrOfDouble&) ;
int imax_array(const ArrOfDouble&) ;
double min_array(const ArrOfDouble&) ;
double max_array(const ArrOfDouble&) ;
double max_abs_array(const ArrOfDouble&);
double min_abs_array(const ArrOfDouble&);

double dotproduct_array(const ArrOfDouble&, const ArrOfDouble&)  ;
double norme_array(const ArrOfDouble&)  ;

// ******************************************************************
//                   FONCTIONS MEMBRES DE ArrOfDouble
// ******************************************************************

// Description :
//  Change le nombre d'elements du tableau. Cette fonction est inline
//  car elle doit etre tres rapide dans le cas ou smart_resize_==1
//  (utilisation frequente de resize_array())
//  Si smart_resize est non nul :
//   Si la nouvelle taille est inferieure ou egale a la taille
//   alouee (p->get_size()) on ne realloue pas de memoire
//   sinon, on realloue et on copie les donnees existantes.
//  Astuce pour ne pas copier les anciennes donnees:
//   resize(0); resize(n);
//  Si smart_resize est nul, on realloue une nouvelle zone memoire
//   uniquement si la nouvelle taille est differente de l'ancienne.
// Parametre: new_size
// Signification: nouvelle valeur demandee pour size_array() (doit etre >= 0)
// Parametre: opt
// Signification: quoi faire des anciennes case et des nouvelles cases (voir memory_resize())
//  Attention, pour un tableau smart_resize, l'option "INIT"
// Precondition :
//  - Si "new_size" est egal a la taille du tableau, aucune condition.
//  - Sinon,
//    * le tableau ne doit pas etre un type derive de ArrOfDouble !!!
//    * le tableau doit etre "resizable", c'est a dire:
//      soit detache, soit normal (pas de type "ref_data") avec ref_count egal a 1
inline void ArrOfDouble::resize_array(int new_size, Array_base::Resize_Options opt)
{
  // Si le tableau change de taille, il doit etre du type ArrOfDouble
  assert(new_size == size_array_ || get_info() == ArrOfDouble::info());
  resize_array_(new_size, opt);
}

// Description: appel a ArrOfDouble::resize_array()
inline void ArrOfDouble::resize(int new_size, Array_base::Resize_Options opt)
{
  resize_array(new_size, opt);
}

// Description: methode protegee de changement de taille, appelable par
//  les classes derivees (idem que resize_array() mais sans condition sur le
//  type derive de l'objet)
inline void ArrOfDouble::resize_array_(int new_size, Array_base::Resize_Options opt)
{
  assert(new_size >= 0);
  // Soit le tableau est detache (data_==0), soit il est normal (p_!=0)
  // S'il est normal, il ne faut pas qu'il y ait d'autre reference au tableau,
  // ou alors la taille ne doit pas changer.
  assert(new_size == size_array_ || data_ == 0 || (p_ != 0 && ref_count() == 1));
  // Le seul cas ou on n'appelle pas memory_resize est le suivant:
  //  le tableau est smart_resize
  //  ET la taille reste inferieure ou egale a la taille allouee
  //  ET on ne veut pas initialiser les nouvelles cases
  if ((opt == COPY_INIT) || (new_size > memory_size_) || (smart_resize_ == 0))
    // attention, il existe un cas ou memory_resize realloue un buffer meme si size_array_==new_size:
    // si on est passe de smart_resize=1 a smart_resize=0, on realloue juste ce qu'il faut.
    memory_resize(new_size, opt);
  size_array_ = new_size;
}

// Description:
//     x est affecte a toutes les cases
// Precondition:
// Parametre: double x
//    Signification: la valeur a affecter a toutes les cases du tableau
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: ArrOfDouble&
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline ArrOfDouble& ArrOfDouble::operator=(double x)
{
  const int n = size_array_;
  double *data = data_;
  for (int i = 0; i < n; i++)
    {
      data[i] = x;
    }
  return *this;
}

// Description:
//     operateur [] retourne le ieme element du tableau
// Precondition:
// Parametre: int i
//    Signification: indice dans l'intervalle 0 <= i < size_array()
// Exception:
//    assert si la valeur sort de l'intervalle : [ -DMAXFLOAT,DMAXFLOAT ]
//    assert si i n'est pas dans l'intervalle
inline double& ArrOfDouble::operator[](int i)
{
  assert(i >= 0 && i < size_array_);
  assert(data_[i] > -DMAXFLOAT && data_[i] < DMAXFLOAT);
  return data_[i];
}

// Description:
//     operateur [] retourne le ieme element du tableau
// Precondition:
// Parametre: int i
//    Signification: indice dans l'intervalle 0 <= i < size_array()
// Exception:
//    assert si la valeur sort de l'intervalle : [ -DMAXFLOAT,DMAXFLOAT ]
//    assert si i n'est pas dans l'intervalle
inline const double& ArrOfDouble::operator[](int i) const
{
  assert(i >= 0 && i < size_array_);
  assert(data_[i] > -DMAXFLOAT && data_[i] < DMAXFLOAT);
  return data_[i];
}

// Description:
//     idem operator[]
inline double& ArrOfDouble::operator()(int i)
{
  return operator[](i);
}

inline const double& ArrOfDouble::operator()(int i) const
{
  return operator[](i);
}

// Description:
//    Renvoie la taille du tableau (nombre d'elements declares
//    a la construction ou a resize_array()).
//    C'est le nombre d'elements accessibles a operator[]
// Retour: int
inline int  ArrOfDouble::size_array() const
{
  return size_array_;
}

// Description:
//  Ajoute une case en fin de tableau et y stocke la "valeur"
// Precondition:
//  Tableau doit etre de type "smart_resize" (sinon, ecroulement
//  des performances). De plus, le tableau ne doit pas etre "ref_data",
//  et il ne doit pas y avoir plus d'une reference a la zone de
//  memoire pointee (meme precondition que resize_array())
//  Le tableau doit etre de type ArrOfDouble (pas un type derive)
inline void   ArrOfDouble::append_array(double valeur)
{
  assert(smart_resize_);
  const int n = size_array_;
  resize_array(size_array_+1, COPY_NOINIT);
  data_[n] = valeur;
}

// Description:
//   Renvoie un pointeur sur le premier element du tableau.
//   Le pointeur est nul si le tableau est "detache".
//   Attention, l'adresse peut changer apres un appel
//   a resize_array(), ref_data, ref_array, ...
// Precondition:
// Retour: const double*
//   Signification: pointeur sur le premier element du tableau
inline const double* ArrOfDouble::addr() const
{
  return data_;
}

// Description:
//   Renvoie un pointeur sur le premier element du tableau.
//   Le pointeur est nul si le tableau est "detache".
// Precondition:
// Retour: const double*
//    Signification: la zone memoire du tableau
inline double* ArrOfDouble::addr()
{
  return data_;
}

// ArrOfDouble_H
#endif

