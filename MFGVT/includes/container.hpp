/*
BSD 3-Clause License

Copyright (c) 2024, SYSJF
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __CONTAINER_HPP__
#define __CONTAINER_HPP__

#include "../includes/grpversion.hpp"

class VersionShower;

/// @brief class qui definit un conteneur commensemble de groupe de fichier de meme id
class VContainer : public std::vector<GrpVersion> 
{
    public: 
        VContainer( const std::string & _name);
        VContainer( const VContainer & _lref);
        VContainer( const VContainer && _rref);

        VContainer & operator=( const VContainer & _lref);
        VContainer & operator=( const VContainer && _rref);

        const std::string get_name(void)const;
        void addVersionShower(std::shared_ptr<VersionShower> _ptr_vs);
        const std::shared_ptr<VersionShower> & atVersionShower(void);

        void set_authExt(const std::string & _authExt);
        void set_reg_version(const std::string & _str);
        void set_reg_id(const std::string & _str);
        void set_reg_autor(const std::string & _str);
        void set_reg_part(const std::string & _str);

        const std::string get_authExt(void) const;

        const std::string get_reg_version(void) const;
        const std::string get_reg_id(void) const;
        const std::string get_reg_autor(void) const;
        const std::string get_reg_part(void) const;


        void add2GrpVersion(const Version && _v);
    protected:

    std::string m_name;
    std::shared_ptr<VersionShower> m_ptr_vs;
    std::string m_authext;

    std::string m_reg_version  , m_reg_autor , m_reg_id , m_reg_part;
};

/// @brief init du contenaire
/// @param _name nom du contenair 
VContainer::VContainer( const std::string & _name): std::vector<GrpVersion>() ,  m_name(_name)
{
    this->m_authext = "";
    this->m_reg_version = "";
    this->m_reg_autor= "";
    this->m_reg_id= "";
    this->m_reg_part= "";
}

/// @brief operateur de copy
/// @param _lref 
/// @return 
VContainer & VContainer::operator=( const VContainer & _lref)
{
    this->m_name = _lref.m_name;
    this->m_ptr_vs= _lref.m_ptr_vs;
    this->m_authext = _lref.m_authext;

    this->m_reg_version = _lref.m_reg_version;
    this->m_reg_autor =_lref.m_reg_autor;
    this->m_reg_id =_lref.m_reg_id;
    this->m_reg_part =_lref.m_reg_part;

    static_cast<std::vector<GrpVersion>&>(*this) = _lref;
    return *this;
}

/// @brief operateur de copy
/// @param _rref 
/// @return 
VContainer & VContainer::operator=( const VContainer && _rref)
{
    this->m_name = _rref.m_name;
    this->m_ptr_vs= _rref.m_ptr_vs;
    this->m_authext = _rref.m_authext;

    this->m_reg_version = _rref.m_reg_version;
    this->m_reg_autor =_rref.m_reg_autor;
    this->m_reg_id =_rref.m_reg_id;
    this->m_reg_part =_rref.m_reg_part;

    static_cast<std::vector<GrpVersion>&>(*this) = _rref;
    return *this;
}

/// @brief constructeur de copy
/// @param _lref 
VContainer::VContainer( const VContainer & _lref)
{
    *this = _lref;
}

/// @brief constructeur de copy
/// @param _rref 
VContainer::VContainer( const VContainer && _rref)
{
    *this = _rref;
}

/// @brief retourne ne nom du contenaire
const std::string VContainer::get_name(void)const
{
    return this->m_name;
}

/// @brief ajoute l'objet d'affichage des contenaire 
/// @param _ptr_vs pointeur vers l'objet d'affichage
void VContainer::addVersionShower(std::shared_ptr<VersionShower> _ptr_vs)
{
    this->m_ptr_vs = _ptr_vs;
}

/// @brief acces controler au pointeur de l'objet d'affichage
/// @return prt partager
const std::shared_ptr<VersionShower> & VContainer::atVersionShower(void)
{
    if(!this->m_ptr_vs)
        throw std::logic_error("bad VersionShower acces");

    return this->m_ptr_vs;
}

/// @brief ajoute une version au groupe qui  lui correspond dans le conteneur
/// @param _v 
void  VContainer::add2GrpVersion(const Version && _v)
{
    const Version * tmp1  = _v.is_lnk() ?_v.ptr_lnk.get() :  &_v;
    for(auto & grp : *this)
    {
        try
        {
            if(grp.get_id() == tmp1->id )
            {
                grp.emplace_back( _v);
                return ;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }

    GrpVersion tmp;
    tmp.emplace_back(_v);

    this->push_back(tmp);
}

/// @brief ajoute les extension de fichier autorisé dans le conteneur
/// @param _authExt 
void VContainer::set_authExt(const std::string & _authExt)
{
    this->m_authext = _authExt;
}

/// @brief renvoie les extention autorisé
/// @param  
/// @return 
const std::string VContainer::get_authExt(void) const
{
    return this->m_authext;
}

/// @brief ajoute un regex pour le versionning
/// @param _str 
void VContainer::set_reg_version(const std::string & _str)
{
    this->m_reg_version = _str;
}

/// @brief ajoute un regex pour identifier par ID
/// @param _str 
void VContainer::set_reg_id(const std::string & _str)
{
    this->m_reg_id = _str;
}

/// @brief ajoute un regex pour les auteur
/// @param _str 
void VContainer::set_reg_autor(const std::string & _str)
{
    this->m_reg_autor = _str;
}

/// @brief ajoute un regex pour les version en plusieur partie
/// @param _str 
void VContainer::set_reg_part(const std::string & _str)
{
    this->m_reg_part = _str;
}

/// @brief retourn le regex de versionning
const std::string VContainer::get_reg_version(void) const
{
    return this->m_reg_version;
}

/// @brief retourn le regex des ID
const std::string VContainer::get_reg_id(void) const
{
    return this->m_reg_id;
}

/// @brief retourn le regex des auteur
const std::string VContainer::get_reg_autor(void) const
{
    return this->m_reg_autor;
}

/// @brief retourn le regex des parties
const std::string VContainer::get_reg_part(void) const
{
    return this->m_reg_part;
}


#endif