/*
                    GNU GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.
*/

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

        void merge(const GrpVersion & _grp);

        void check(unsigned int _n_thread = 0);

        bool inError(void);

    private:

        void check_callback(const GrpVersion::iterator & it_s , const GrpVersion::iterator & it_e);
        void versionCheck(Version & _v1 , Version & _v2);
    
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
        throw std::logic_error("grp has 0 verion");

    std::string tmp =this->front().is_lnk() ? this->front().ptr_lnk->id : this->front().id;

    for(auto it = this->begin() +1 ; it != this->end() ; it++)
    {
        const Version * tmp1 = it->is_lnk() ? it->ptr_lnk.get() :  &*it;
        if( tmp != tmp1->id)
            throw std::logic_error("bad id in grp => " + tmp + " != " + tmp1->id);
    }

    return tmp;
}



/// @brief fusionne un groupe cible au groupe en instance
/// @param _grp 
void GrpVersion::merge(const GrpVersion & _grp)
{
    for(auto &  vers1 : _grp)
    {
        //Si la vereison n'est pas deja present la rajoute a l'actuel groupe
        bool found = false;

        const Version * tmp1 = vers1.is_lnk() ? vers1.ptr_lnk.get() :  &vers1;
        

        for( auto &  vers2 : *this)
        {
            const Version * tmp2 = vers2.is_lnk() ? vers2.ptr_lnk.get() :  &vers2;
            //si base de nom identique
            if( tmp1->cmpFile(*tmp2)  )
            {
                found = true;
                break;
            }
        }

        if( !found)
            this->push_back(vers1);
        
    }
}

/// @brief verifie si 2 version son ok
/// @param _v1 
/// @param _v2 
void GrpVersion::versionCheck(Version & _v1 , Version & _v2)
{
    //verifie les date de creaztion
    if( _v1.createDate != _v2.createDate )
    {
        _v1.add_error(Version::EF_CreateDate);
        _v2.add_error(Version::EF_CreateDate);
    }

    //verifie les date de modification
    if( _v1.modifDate != _v2.modifDate )
    {
        _v1.add_error(Version::EF_ModifDate);
        _v2.add_error(Version::EF_ModifDate);
    }
}

/// @brief verifi si il ya des inchoerance dans les version du groupe et ajoute les flag associer
void GrpVersion::check_callback(const GrpVersion::iterator & it_s ,const GrpVersion::iterator & it2_e)
{
    for( auto it = it_s ; it != it2_e - 1 ;it++)
    {
        for( auto it2 = this->begin() ; it2 != this->end() ;it2++)
        {
            Version * tmp1 = it->is_lnk() ? it->ptr_lnk.get() :  &*it;
            Version * tmp2 = it2->is_lnk() ? it2->ptr_lnk.get() :  &*it2;

            if( tmp1->cmpMeta( *tmp2 ) )
                versionCheck(*tmp1 , *tmp2 );
            
                            
            //ne regarde que les fichier strictement confodable ( jumeau )
            if( tmp1->cmpBaseName(*tmp2) && !tmp1->cmpFile(*tmp2) )
            {
                //verifie les date de creaztion

                try
                {
                    //regarde si les taille de fichier sont bien identique
                    if( !utilitys::cmpFileSize( tmp1->get_file() ,  tmp2->get_file()))
                    {
                        tmp1->add_error(Version::EF_MD5);
                        tmp2->add_error(Version::EF_MD5);
                    }
                    else
                    {
                        try
                        {
                            //regarde si le hash de contenue est trictement identique
                            if( !utilitys::cmpFilesHash( tmp1->get_file() ,  tmp2->get_file()) )
                            {
                                tmp1->add_error(Version::EF_MD5);
                                tmp2->add_error(Version::EF_MD5);
                            }
                        }
                        catch(const std::exception& e)
                        {
                            // pas pu accedé au fichier 

                            std::cerr << e.what() << std::endl;
                            tmp1->add_error(Version::EF_ACCES);
                            tmp2->add_error(Version::EF_ACCES);
                        }

                    } 
                }
                catch(const std::exception& e)
                {
                    // pas pu accedé au fichier 
                    std::cerr << e.what() << std::endl;
                    tmp1->add_error(Version::EF_ACCES);
                    tmp2->add_error(Version::EF_ACCES);
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
    utilitys::multi_thread_callback( std::bind(&GrpVersion::check_callback, this, std::placeholders::_1 , std::placeholders::_2 ) , *this , _n_thread );
}


/// @brief retourn si le groupe possede une erreur
bool GrpVersion::inError(void)
{
    for(auto const & vers : *this)
        if( vers.get_error() != 0 )
            return true;
    

    return false;
}

#endif