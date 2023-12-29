#ifndef __GRPVERSION_HPP__
#define __GRPVERSION_HPP__

#include "../includes/version.hpp"

#include <thread>

/// @brief regroupe différent fichier considérer comme apartement a une meme version
class GrpVersion : public std::vector<Version> , public utilitys::MainPathSharedTemplate
{
    public:
        GrpVersion(void);

        virtual ~GrpVersion(void);

        const std::string get_id(void) const;
        const std::string get_autor(void) const;
        const std::string get_createDate(void) const;
        const std::string get_modifDate(void) const;

        void merge(const GrpVersion & _grp);

        void check(unsigned int _n_thread = 0);

    private:

        void check_callback(const GrpVersion::iterator & it_s , const GrpVersion::iterator & it_e);
    
};

GrpVersion::~GrpVersion(void)
{
}

GrpVersion::GrpVersion(void):std::vector<Version>() , utilitys::MainPathSharedTemplate()
{
    
}

/// @brief retourn l'ID des version 
/// @return str
const std::string GrpVersion::get_id(void) const
{
    if( this->size() == 0)
        return "error no version in grp";

    
    std::string tmp = this->front().id;

    for(auto it = this->begin() +1 ; it != this->end() ; it++)
    {
        if( tmp != it->id)
            return "error incoherence dans les id des versions";
    }

    return tmp;
}

/// @brief retourn l'auteur des version 
/// @return str
const std::string GrpVersion::get_autor(void) const
{
    if( this->size() == 0)
        return "error no version in grp";

    std::string tmp = this->front().autor;

    for(auto it = this->begin() +1 ; it != this->end() ; it++)
    {
        if( tmp != it->autor)
            return "error incoherence dans les auteurs des versions";
    }

    return tmp;
}

/// @brief retourn la date de creation  
/// @return str
const std::string GrpVersion::get_createDate(void) const
{
    if( this->size() == 0)
        return "error no version in grp";

    std::string tmp = this->front().createDate;

    for(auto it = this->begin() +1 ; it != this->end() ; it++)
    {
        if( tmp != it->createDate)
            return "error incoherence dans les date de creation des versions";
    }

    return tmp;
}

/// @brief retourn la date de modification 
/// @return str
const std::string GrpVersion::get_modifDate(void) const
{
    if( this->size() == 0)
        return "error no version in grp";

    std::string tmp = this->front().modifDate;

    for(auto it = this->begin() +1 ; it != this->end() ; it++)
    {
        if( tmp != it->modifDate)
            return "error incoherence dans les date de modification des versions";
    }

    return tmp;
}

/// @brief fusionne un groupe cible au groupe en instance
/// @param _grp 
void GrpVersion::merge(const GrpVersion & _grp)
{
    for(const auto &  vers1 : _grp)
    {
        //Si la vereison n'est pas deja present la rajoute a l'actuel groupe
        bool found = false;
        for(const auto &  vers2 : *this)
        {
            //si base de nom identique
            if( vers1.cmpFile(vers2) )
            {
                found = true;
                break;
            }
        }

        if( !found )
            this->push_back(vers1);
        
    }
}

/// @brief verifi si il ya des inchoerance dans les version du groupe et ajoute les flag associer
void GrpVersion::check_callback(const GrpVersion::iterator & it_s ,const GrpVersion::iterator & it2_e)
{
    for( auto it = it_s ; it != it2_e - 1 ;it++)
    {
        for( auto it2 = this->begin() ; it2 != this->end() ;it2++)
        {
        //ne regarde que les fichier strictement confodable ( jumeau)
            if( it->cmpBaseName(*it2) && !it->cmpFile(*it2) )
            {
                //verifie les date de creaztion
                if( it->createDate != it2->createDate )
                {
                    it->add_error(Version::EF_CreateDate);
                    it2->add_error(Version::EF_CreateDate);
                }

                //verifie les date de modification
                if( it->modifDate != it2->modifDate )
                {
                    it->add_error(Version::EF_ModifDate);
                    it2->add_error(Version::EF_ModifDate);
                }

                try
                {
                    //regarde si les taille de fichier sont bien identique
                    if( !utilitys::cmpFileSize( *this->atMainPath() +"\\"+it->get_subPathFile() , *this->atMainPath() +"\\"+it2->get_subPathFile()))
                    {
                        it->add_error(Version::EF_MD5);
                        it2->add_error(Version::EF_MD5);
                    }
                    else
                    {
                        try
                        {
                            //regarde si le hash de contenue est trictement identique
                            if( !utilitys::cmpFilesHash( *this->atMainPath() +"\\"+it->get_subPathFile() , *this->atMainPath() +"\\"+it2->get_subPathFile()) )
                            {
                                it->add_error(Version::EF_MD5);
                                it2->add_error(Version::EF_MD5);
                            }
                        }
                        catch(const std::exception& e)
                        {
                            // pas pu accedé au fichier 

                            std::cerr << e.what() << std::endl;
                            it->add_error(Version::EF_ACCES);
                            it2->add_error(Version::EF_ACCES);
                        }

                    } 
                }
                catch(const std::exception& e)
                {
                    // pas pu accedé au fichier 
                    std::cerr << e.what() << std::endl;
                    it->add_error(Version::EF_ACCES);
                    it2->add_error(Version::EF_ACCES);
                }
            }
        }
    }
        
    

}

/// @brief verifi si il ya des inchoerance dans les version du groupe et ajoute les flag associer
/// @param  _n_thread fonction threadable , nombre de thread a utiliser pour la parralelisation conseiller 1/2 concurency 
void GrpVersion::check(unsigned int _n_thread)
{
    //tout dans le thread principal 
    if(_n_thread < 2 || this->size() < 2)
        this->check_callback( this->begin() , this->end() );
    
    else
    {
        std::vector< std::thread > tmp_th;

        int divider = this->size() / _n_thread;

        if( divider == 0)
        {
            this->check_callback( this->begin() , this->end() );
            return ;
        }

        for( unsigned int i = 0 ; i < _n_thread ; i++)
        {
            if( i == _n_thread-1)
            {
                tmp_th.emplace_back( &GrpVersion::check_callback, this, this->begin() + divider * i , this->end() );
            }
            else
            {
                tmp_th.emplace_back( &GrpVersion::check_callback, this, this->begin() + divider * i , this->begin() + divider * (i+1)  );
            }
        }

        for( auto & th : tmp_th)
            th.join();

    }
    
}


#endif