#ifndef __VERSION_HPP__
#define __VERSION_HPP__

#include "../includes/utilitys.hpp"
#include <format>
#include <chrono>

/// @brief definit un fichier en une version par ces caracteristique
class Version
{
    public : 

        enum Error_flag : const byte{EF_CreateDate = 0b00000001, EF_ModifDate = 0b00000010  , EF_MD5 = 0b00000100 , EF_ACCES= 0b00001000 , EF_LNK= 0b00100000};
        
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

        bool is_lnk(void) const;

        void createLnkVersion(std::string const & main_path="");
        void update_dates(std::string const & main_path="");
        

        static const std::string VersionError( Version const & _vers);
        static std::string EF2Str(Error_flag const _err);

        //acces directe au element
        std::string name ,dir ,id , createDate , modifDate, autor  , extension ,version , part , device ; 
        std::shared_ptr<Version> ptr_lnk;
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
    this->ptr_lnk = _cpy.ptr_lnk;

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
    this->ptr_lnk = _other.ptr_lnk;

    return *this;
}

/// @brief passage par right ref value
/// @param _rref 
Version::Version(const Version && _rref)
{
    *this =  _rref;
}

/// @brief regarde si la version est un raccorci
/// @param  
/// @return vrai ou faux
bool Version::is_lnk(void) const
{
    return this->ptr_lnk ? true : false;
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

    if( _vers.get_error() & EF_LNK)
    {
        tmp += EF2Str( EF_LNK );
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

        case EF_LNK:
            tmp = "EF_LNK";
            break;
        
        default:
            break;
    }

    return tmp;
}

/// @brief crée le pointeur sur le fichier lien de cette vresion
/// @param  
void Version::createLnkVersion( std::string const & main_path  )
{
    std::string && target = utilitys::get_targetOfLnk(main_path + this->get_subPathFile());

    if(target.size() == 0)
    {
        this->add_error(EF_LNK);
        return ;
    }

    this->ptr_lnk = std::shared_ptr<Version>( new Version() );

    auto pathName = utilitys::sep_sub_and_name(target);
    auto nameExt = utilitys::sep_name_and_ext(std::get<1>(pathName));
    
    this->ptr_lnk->dir= std::get<0>(pathName);
    this->ptr_lnk->name= std::get<0>(nameExt);
    this->ptr_lnk->extension = std::get<1>(nameExt);

    this->ptr_lnk->device = this->device;
}

/// @brief met a jour la date de creation et la date de modification du fichier
/// @param main_path 
void Version::update_dates(std::string const & main_path)
{
    std::string tmpFileName =  main_path + this->get_subPathFile();

    // Obtenez le temps de dernière modification du fichier
    try
    {
        std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(tmpFileName);
        this->modifDate = std::string(std::format("{}",lastWriteTime)).substr(0,10) ;
    }
    catch(const std::exception & e)
    {
        this->add_error(Version::EF_ACCES);
        std::cerr << e.what() << std::endl;
    }

    try
    {
        this->createDate = utilitys::get_createDate(tmpFileName);
    }
    catch(const std::exception & e)
    {
        this->add_error(Version::EF_ACCES);
        std::cerr << e.what() << std::endl;
    }
}


#endif