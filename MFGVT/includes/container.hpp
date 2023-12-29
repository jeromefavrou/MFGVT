#ifndef __CONTAINER_HPP__
#define __CONTAINER_HPP__

#include "../includes/grpversion.hpp"

class VersionShower;

/// @brief class qui definit un conteneur commensemble de groupe de fichier de meme id
class Container : public std::vector<GrpVersion> 
{
    public: 
        Container( const std::string & _name);
        Container( const Container & _lref);
        Container( const Container && _rref);

        Container & operator=( const Container & _lref);
        Container & operator=( const Container && _rref);

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
Container::Container( const std::string & _name): std::vector<GrpVersion>() ,  m_name(_name)
{
    this->m_authext = "";
}

/// @brief operateur de copy
/// @param _lref 
/// @return 
Container & Container::operator=( const Container & _lref)
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
Container & Container::operator=( const Container && _rref)
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
Container::Container( const Container & _lref)
{
    *this = _lref;
}

/// @brief constructeur de copy
/// @param _rref 
Container::Container( const Container && _rref)
{
    *this = _rref;
}

/// @brief retourne ne nom du contenaire
const std::string Container::get_name(void)const
{
    return this->m_name;
}

/// @brief ajoute l'objet d'affichage des contenaire 
/// @param _ptr_vs pointeur vers l'objet d'affichage
void Container::addVersionShower(std::shared_ptr<VersionShower> _ptr_vs)
{
    this->m_ptr_vs = _ptr_vs;
}

/// @brief acces controler au pointeur de l'objet d'affichage
/// @return prt partager
const std::shared_ptr<VersionShower> & Container::atVersionShower(void)
{
    if(!this->m_ptr_vs)
        throw std::logic_error("bad VersionShower acces");

    return this->m_ptr_vs;
}

/// @brief ajoute une version au groupe qui  lui correspond dans le conteneur
/// @param _v 
void  Container::add2GrpVersion(const Version && _v)
{
    for(auto & grp : *this)
    {
        if(grp.get_id() == _v.id)
        {
            grp.emplace_back( _v);
            return ;
        }
    }

    GrpVersion tmp;
    tmp.emplace_back(_v);

    this->push_back(tmp);
}

/// @brief ajoute les extension de fichier autorisé dans le conteneur
/// @param _authExt 
void Container::set_authExt(const std::string & _authExt)
{
    this->m_authext = _authExt;
}

/// @brief renvoie les extention autorisé
/// @param  
/// @return 
const std::string Container::get_authExt(void) const
{
    return this->m_authext;
}

/// @brief ajoute un regex pour le versionning
/// @param _str 
void Container::set_reg_version(const std::string & _str)
{
    this->m_reg_version = _str;
}

/// @brief ajoute un regex pour identifier par ID
/// @param _str 
void Container::set_reg_id(const std::string & _str)
{
    this->m_reg_id = _str;
}

/// @brief ajoute un regex pour les auteur
/// @param _str 
void Container::set_reg_autor(const std::string & _str)
{
    this->m_reg_autor = _str;
}

/// @brief ajoute un regex pour les version en plusieur partie
/// @param _str 
void Container::set_reg_part(const std::string & _str)
{
    this->m_reg_part = _str;
}

/// @brief retourn le regex de versionning
const std::string Container::get_reg_version(void) const
{
    return this->m_reg_version;
}

/// @brief retourn le regex des ID
const std::string Container::get_reg_id(void) const
{
    return this->m_reg_id;
}

/// @brief retourn le regex des auteur
const std::string Container::get_reg_autor(void) const
{
    return this->m_reg_autor;
}

/// @brief retourn le regex des parties
const std::string Container::get_reg_part(void) const
{
    return this->m_reg_part;
}


#endif