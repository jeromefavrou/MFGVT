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


#ifndef __FILE_HPP__
#define __FILE_HPP__

#include <string>
#include <memory>
#include <vector>
#include <tuple>


/// @brief objet de gestion d'enregistrement d'un fichier ou et de c'est lien
class File
{
    public :

        File( std::string const & _file);
        File( std::string const && _file);

        File( File const & _cpy );
        File( File const && _other );

        File & operator=(File const & _cpy);
        File & operator=(File const && _other);

        virtual ~File(void);

        const std::shared_ptr<std::vector<File>> & get_lnk(void ) const;
        std::shared_ptr<std::vector<File>> & get_ptr_lnk(void ) ;
        const std::string get_file(void )const;

        const std::string get_ext(void) const;
        const std::string get_dir(void) const;
        const std::string get_name(void) const;

        const bool cmpFile(const File & _cpy) const ;
        const bool cmpBaseName(const File & _cpy) const ;


        const bool is_symLnk(void) const;


        static std::tuple< std::string ,  std::string > sep_sub_and_name(std::string const & _str);
        static std::tuple< std::string , std::string > sep_name_and_ext(std::string const & _str);

    protected:
        std::string m_dir , m_name , m_ext  ;
        std::shared_ptr<std::vector<File>> m_ptr_lnk ;
};

File::~File(void)
{
    this->m_dir ="";
    this->m_name = "";
    this->m_ext = "" ;
}

File::File(std::string const & _file):m_ptr_lnk( std::shared_ptr<std::vector<File>>( new std::vector<File>()) )
{
    auto pathName = sep_sub_and_name( _file );
    auto nameExt = sep_name_and_ext(std::get<1>(pathName));

    this->m_dir = std::get<0>(pathName) ;
    this->m_name = std::get<0>(nameExt);
    this->m_ext = std::get<1>(nameExt);
}

File::File(std::string const && _file):m_ptr_lnk( std::shared_ptr<std::vector<File>>( new std::vector<File>()) )
{
    auto pathName = sep_sub_and_name( _file );
    auto nameExt = sep_name_and_ext(std::get<1>(pathName));
    this->m_dir = std::get<0>(pathName) ;
    this->m_name = std::get<0>(nameExt);
    this->m_ext = std::get<1>(nameExt);
}

File & File::operator=(File const & _cpy)
{
    this->m_dir = _cpy.m_dir;
    this->m_name = _cpy.m_name;
    this->m_ext = _cpy.m_ext;
    this->m_ptr_lnk = _cpy.m_ptr_lnk;

    return *this;
}

File & File::operator=(File const && _other)
{
    this->m_dir = _other.m_dir;
    this->m_name = _other.m_name;
    this->m_ext = _other.m_ext;
    this->m_ptr_lnk = _other.m_ptr_lnk;

    return *this;
}

File::File( File const & _cpy )
{
    *this = _cpy;
}
File::File( File const && _other )
{
    *this = _other;
}

        
/// @brief retourn si le fichier est un raccrourci
/// @return vrai ou faux
const bool File::is_symLnk( void ) const
{
    return this->m_ptr_lnk->size() > 0;
}

/// @brief retourn les raccourci
/// @return ptr
const std::shared_ptr<std::vector<File>> &File::get_lnk(void ) const
{
        return this->m_ptr_lnk;
}

/// @brief donne un acces au pointeur
/// @return ptr
std::shared_ptr<std::vector<File>> & File::get_ptr_lnk(void )
{
    return this->m_ptr_lnk;
}

/// @brief retourn le chamin du fichier
/// @return str
const std::string File::get_file( void )const
{
    return this->m_dir + "\\" + this->m_name +"." + this->m_ext;
}

/// @brief retourn l'extention
/// @return str
const std::string File::get_ext(void) const
{
    return this->m_ext;
}

/// @brief retour le chemin
/// @return str
const std::string File::get_dir(void) const
{
    return this->m_dir;
}

/// @brief retourn le nom sans l'extention
/// @return str
const std::string File::get_name(void) const
{
    return this->m_name;
}

/// @brief compare si le chemin , nom et extension pour deteerminer si 2 fichier identique
/// @param _cpy const ref
/// @return vrai ou faux
const bool File::cmpFile(const File & _cpy) const 
{
    return _cpy.get_dir() == this->m_dir && _cpy.get_name() == this->m_name && _cpy.get_ext() == this->m_ext ;
}

/// @brief compare si om et extension pour deteerminer si 2 fichier son jumeau
/// @param _cpy 
/// @return vrai ou faux
const bool File::cmpBaseName(const File & _cpy) const 
{
    return  _cpy.get_name() == this->m_name && _cpy.get_ext() == this->m_ext ;
}


/// @brief sépare une chemin de fichier en chemin et nom de fichier
/// @param _str chemin a séparer
/// @return tuple de chaine str contenant le chemin <0> et le nom de fichier <1>
std::tuple<std::string , std::string> File::sep_sub_and_name(std::string const & _str)
{
    int idx = 0;

    for(auto i = _str.size() ; i >= 0 ; i--)
    {
        if(_str[i] == '\\' || _str[i] == '/' )
        {
            idx = i ; 
            break;
        }
    }

    if( idx <= 0)
        return { "" , _str};

    return {_str.substr(0 , idx) , _str.substr(idx+1 , _str.size())};
}

/// @brief sépare une nom de fichier et son extention
/// @param _str 
/// @return tuple de str contenant les nom fichier <0> et son extension<1>
std::tuple<std::string , std::string> File::sep_name_and_ext(std::string const & _str)
{
    int idx = 0;

    for(auto i = _str.size() ; i >= 0 ; i--)
    {
        if(_str[i] == '.' )
        {
            idx = i ; 
            break;
        }
    }

    if( idx <= 0)
        return {_str , ""};

    return {_str.substr(0 , idx) , _str.substr(idx+1 , _str.size())};
}
    

#endif