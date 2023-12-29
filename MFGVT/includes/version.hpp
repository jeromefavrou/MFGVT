#ifndef __VERSION_HPP__
#define __VERSION_HPP__

#include "../includes/utilitys.hpp"

/// @brief definit un fichier en une version par ces caracteristique
class Version
{
    public : 

        enum Error_flag : const byte{EF_CreateDate = 0b00000001, EF_ModifDate = 0b00000010  , EF_MD5 = 0b00000100 , EF_ACCES= 0b00001000};
        
        Version(void);
        Version(const Version & _cpy);
        Version(const Version && _rref);

        Version & operator=(const Version & _cpy );
        Version & operator=(const Version && _other );

        virtual ~Version(void);

        const bool cmpFile(const Version & _cpy) const ;
        const bool cmpBaseName(const Version & _cpy) const ;

        std::string get_subPathFile(void);

        const byte get_error( void )const;
        
        void add_error(Error_flag const _err);

        static const std::string VersionError( Version const & _vers);
        static std::string EF2Str(Error_flag const _err);

        //acces directe au element
        std::string name ,dir ,id , createDate , modifDate, autor  , extension ,version , part , device; 
    protected :

        byte m_id_error;
};

/// @brief destructeur defaut 
/// @param  
Version::~Version(void)
{
}

//
Version::Version(void):m_id_error(0)
{
}

/// @brief copy de l'objet
/// @param _cpy copy
Version::Version(const Version & _cpy)
{
    *this =  _cpy;
}

Version & Version::operator=(const Version & _cpy )
{
    this->dir=_cpy.dir;
    this->name=_cpy.name;
    this->id=_cpy.id;
    this->createDate=_cpy.createDate;
    this->autor=_cpy.autor;
    this->extension =_cpy.extension;
    this->version =_cpy.version;
    this->modifDate = _cpy.modifDate;
    this->part=_cpy.part;
    this->device = _cpy.device;
    this->m_id_error = _cpy.get_error();

    return *this;
}

Version & Version::operator=(const Version && _other )
{
    this->dir=_other.dir;
    this->name=_other.name;
    this->id=_other.id;
    this->createDate=_other.createDate;
    this->autor=_other.autor;
    this->extension =_other.extension;
    this->version =_other.version;
    this->modifDate = _other.modifDate;
    this->part=_other.part;
    this->device = _other.device;
    this->m_id_error = _other.get_error();

    return *this;
}

/// @brief passage par right ref value
/// @param _rref 
Version::Version(const Version && _rref)
{
    *this =  _rref;
}

/// @brief compare si le chemin , nom et extension pour deteerminer si 2 fichier identique
/// @param _cpy const ref
/// @return vrai ou faux
const bool Version::cmpFile(const Version & _cpy) const 
{
    return _cpy.dir == this->dir && _cpy.name == this->name && _cpy.extension == this->extension ;
}

/// @brief compare si om et extension pour deteerminer si 2 fichier son jumeau
/// @param _cpy 
/// @return vrai ou faux
const bool Version::cmpBaseName(const Version & _cpy) const 
{
    return  _cpy.name == this->name && _cpy.extension == this->extension ;
}

/// @brief return le sous chemin complet d'un fichier
/// @return str
std::string Version::get_subPathFile(void)
{
    return this->dir + "\\" + this->name + "." +this->extension;
}

/// @brief donna la valeur du defaut
/// @return byte de defaut
const byte Version::get_error( void) const
{
    return this->m_id_error;
}


/// @brief ajoute un flag d'erreur au byte de défaut
/// @param _err 
void Version::add_error( Version::Error_flag const _err) 
{
    this->m_id_error |=  static_cast<byte>(_err);
}


/// @brief static , transforme l'erreur d'un fichier en verbose 
/// @param _vers 
/// @return str
const std::string Version::VersionError( Version const & _vers)
{
    std::string tmp = "";

    if( _vers.get_error() & EF_CreateDate)
    {
        tmp += EF2Str( EF_CreateDate );
        tmp+=";";
    }
        
    if( _vers.get_error() & EF_ModifDate)
    {
        tmp += EF2Str( EF_ModifDate );
        tmp+=";";
    }

    if( _vers.get_error() & EF_MD5)
    {
        tmp += EF2Str( EF_MD5 );
        tmp+=";";
    }

    if( _vers.get_error() & EF_ACCES)
    {
        tmp += EF2Str( EF_ACCES );
        tmp+=";";
    }

    return tmp;
}

/// @brief convetie un flag d'eurreur en string
/// @param _err flag d'erreur
/// @return str
std::string  Version::EF2Str(Version::Error_flag const _err)
{
    std::string tmp = "";

    switch (_err)
    {
        case EF_CreateDate:
            tmp =  "EF_CreateDate";
            break;

        case EF_ModifDate:
            tmp ="EF_ModifDate";
            break;

        case EF_MD5:
            tmp = "EF_MD5";
            break;
        
        case EF_ACCES:
            tmp = "EF_Acces";
            break;
        
        default:
            break;
    }

    return tmp;
}


#endif