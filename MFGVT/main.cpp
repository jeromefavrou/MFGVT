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
#include <string>

#define __MFVGT_VERSION_ARCH 64
#define __MFVGT_VERSION_MAJOR 0
#define __MFVGT_VERSION_MINOR 9
#define __MFVGT_VERSION_PART 0

//#define __MFVGT_VERSION_DEBUG 

#ifndef __MFVGT_VERSION_DEBUG
    #define __MFVGT_VERSION_RELEASE
#else
    #define  __MFVGT_PROJECT_DIR std::string("C:\\Users\\Rigel85\\Desktop\\MFGVT64")
#endif


#include "includes/gtkmmcomponent.hpp"


int main(int argc, char *argv[]) 
{
    std::stringstream ss ;
    // affichage de la version en console
    ss << "version : " << __MFVGT_VERSION_MAJOR <<"." <<__MFVGT_VERSION_MINOR << "." << __MFVGT_VERSION_PART << " " << __MFVGT_VERSION_ARCH << " bits : " ;

    #ifdef __MFVGT_VERSION_RELEASE
        ss << "RELEASE"<< std::endl;
    #else
        ss << "DEBUG"<< std::endl;
    #endif

    std::string vers("");

    std::getline(ss , vers);

    std:: cout << vers << std::endl;

    //séparte une chemin et son fichier
    // ici correspond au chemin absolut du programme 

    auto progmPath = File::sep_sub_and_name(argv[0]);

    Gtk::Main app(argc, argv);

    try
    {
        //fenetre principal avec chemin absolut partager
        WindowMain fenetre(std::get<0>(progmPath));

	    fenetre.set_title("Maintenance File Grouper Version Tool     " + vers);

        //! charge les donné , doit etre fait avant WindowMain::init()
        // cherche les donné dans le chamin de Ressource
        auto tload = Device::load( fenetre.get_ressourcePath() );

        fenetre.addMainPath( std::shared_ptr< const std::string >(new const std::string(std::get<0>(tload))) );

        //ajoute tout les materiel charger dan s lezs donné a partagé
        fenetre.addDevice( std::get<1>(tload) );
        
        //partage le chemin principal a tout les materiel
        for(auto & dev : *fenetre.atDevice() )
            dev.addMainPath(fenetre.atMainPath());
        
        
        fenetre.init();
        
        fenetre.show_all();
        
        Gtk::Main::run(fenetre);
    }
    catch(LogicExceptionDialog const & _e)
    {
        _e.show();
        std::cin.get();
    }
    catch(std::exception const & _e)
    {
        std::cerr << _e.what() << std::endl;

        std::cin.get();
    }
    
    

    return 0;
}

